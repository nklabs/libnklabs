# Command Line Interface

## Files

[nkcli.h](../inc/nkcli.h),
[nkcli.c](../src/nkcli.c)

## Description

## nk_init_cli()

```c
void nk_init_cli();
```

Start command line interface, print first prompt.  This schedules a task, so
prompt is issued on next return to the scheduler loop.

## COMMAND()

```c
COMMAND(function-name,
  "-command      short help text\n"
  "-command arg  main help text first line\n"
  "-command arg  main help text second line\n"
  "-command      main help text third line\n"
  "!command      hidden help text (factory mode only)\n"
)

COMMAND(function-name,
  "!command      short help text for hidden command\n"
  "!command      main help text first line\n"
  "!command      main help text second line\n"
  "!command arg  main help text third line\n"
)```

Register a CLI command function and provide help text for it.  The first
help line has the command as it is parsed- it's the first word, "command"
above.  It also provides short help, which is shown when the user types
"help" and all available commands are listed.

If the first line begins with '!' it's a hidden command, accessible only
when the system is in "factory service mode".  If it begins with any other
character, the command is customer visible.

Successive lines provide detailed help which is printed when the user types
"help command".  Help lines that are prefixed with '!' are only shown in
factory service mode.

An example of a typical CLI command is as follows:

```c
int cmd_test(nkinfile_t *args)
{
	if (nk_fscan(args, "foo ")) {
		printf("You gave the foo argument\n");
	} else if (nk_fscan(args, "")) {
		printf("You gave no arguments\n");
	} else {
		printf("Syntax error\n");
	}
	return 0;
}

COMMAND(cmd_test,
    ">test        Test command\n"
    "-test        Test with no arguments\n"
    "-test foo    Try the foo argument\n"
)
```

When the user types the command, the arguments are packaged up as an
nkinfile_t and are passed to the command function (cmd_test in the above
example).  The start of this file is the first non-whitespace character
given on the command line.

The command function should parse the arguments, most conveniently with
nk_fscan.  If the arguments are invalid, it should print "Syntax error\n". 
Note that C compilers merge common constant strings, so this "Syntax
error\n" string will appear only once in the binary image.

The user can get a list of commands by typing the "help" command.

They can get command specific help by typeing "help test" or "test help". 
The command specific help is provided by the HELP macro as shown above. 
Many lines of HELP can be provided.

COMMAND uses linker tricks to build the command table in a distributed
fashion.  A dedicated linker section is provided for this command table. 
COMMAND appends "struct nk_cli" structures to this section.

The commands are sorted so that help and completions are shown in
alphabetical order.

The command function should always return 0.  The return value is a future
provision for command status.

## nk_cli_set_facmode(), nk_cli_get_facmode()

```c
int nk_cli_set_facmode(int mode);
int nk_cli_get_facmode();
```

Enable or disable factory mode (which allows hidden commands to operate). 
The default is 0, which means disabled.  1 enables factory mode. 
nk_cli_set_facmode() returns the previous mode.

