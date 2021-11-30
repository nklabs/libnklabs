#if 0
int cmd_ctest(char *args)
{
	char str[20];
	int loops;
	if (nk_scan(&args, "%w %d %e", str, sizeof(str), &loops))
	{
		int x;
		double f = 0.0;
		cmd_date("");
		for (x = 0; x != loops; ++x)
			f += wbstrtod(str, NULL);
		cmd_date("");
		nk_printf("%g\n", f);
	}
	else
	{
		nk_printf("Syntax error\n");
	}
	return 0;
}

COMMAND(ctest,
    "ctest                     Test strtod\n",
    "ctest                     Test strtod\n",
    ""
)
#endif
