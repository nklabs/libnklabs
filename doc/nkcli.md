# Command Line Interface

## nk_init_cli()

```c
void nk_init_cli();
```

Start command line interface, print first prompt.  This schedules a task, so
prompt is issued on next return to the scheduler loop.

## COMMAND(), HIDDEN_COMMAND()

```c
COMMAND(name,
  "short help text\n",
  "main help text first line\n"
  "main help text second line\n"
  "main help text third line\n"
  "etc.\n",
  "hidden help test\n"
)
```

Register a CLI command function and provide help for it.  The function must
be named cmd_\<name>, where \<name> is provided to the COMMAND macro.

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

COMMAND(test,
    "test        Test command\n",
    "test        Test with no arguments\n"
    "test foo    Try the foo argument\n",
    ""
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

The command function should always return 0.  The return value is a future
provision for command status.

## HIDDEN_COMMAND()

```c
COMMAND(test,
    "test        Test command\n",
    "test        Test with no arguments\n"
    "test foo    Try the foo argument\n",
    ""
)
```

This is just like the COMMAND macro above, except the command is marked as
hidden.  This means that the command will only operate and show in the help
if the "factory mode" flag is set.

## nk_cli_set_facmode(), nk_cli_get_facmode()

```c
int nk_cli_set_facmode(int mode);
int nk_cli_get_facmode();
```

Enable or disable factory mode (which allows hidden commands to operate). 
The default is 0, which means disabled.  1 enables factory mode. 
nk_cli_set_facmode() returns the previous mode.

