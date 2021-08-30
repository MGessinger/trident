#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

extern int yylex_destroy();
extern FILE *yyin;

void printUsage ()
{
		fprintf(stderr, "USAGE: multipipe CMD [ARGS]\n");
		fprintf(stderr, "Calls the command CMD with the optional arguments ARGS.\n\n");
		fprintf(stderr, "If CMD contains a '/' character, it is interpreted as the exact path to an executable file (can be relative or absolute).\n");
		fprintf(stderr, "Otherwise, the environment variable PATH is used to search an executable with the corresponding name.\n");
}

int main (int argc, char **argv)
{
	yyin = stdin;
	yyparse();
	yylex_destroy();
	return 0;
}
