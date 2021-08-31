#include <stdio.h>
#include <unistd.h>
#include "child_process.h"
#include "parser.h"
#include "jobs.h"

extern int yylex_destroy();
extern FILE *yyin;

stack_t stack;

void printUsage ()
{
		fprintf(stderr, "USAGE: multipipe CMD [ARGS]\n");
		fprintf(stderr, "Calls the command CMD with the optional arguments ARGS.\n\n");
		fprintf(stderr, "If CMD contains a '/' character, it is interpreted as the exact path to an executable file (can be relative or absolute).\n");
		fprintf(stderr, "Otherwise, the environment variable PATH is used to search an executable with the corresponding name.\n");
}

int main (int argc, char **argv)
{
	stack = init();
	yyin = stdin;

	yyparse();
	executeJob(stack.data[0], STDIN_FILENO);

	yylex_destroy();
	clear(stack);

	return 0;
}
