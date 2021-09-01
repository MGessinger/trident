#include <stdio.h>
#include <unistd.h>
#include "child_process.h"
#include "parser.h"
#include "jobs.h"

extern int yyparse ();
extern int yylex_destroy ();
extern FILE * yyin;

stack_t stack;

void printUsage ()
{
	fprintf(stderr, "USAGE: trident [OPTIONS] FILE[S]\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "   -s JOBNAME\t\tRun the job named JOBNAME as the intial job,\n\t\t\tinstead of the first one in the input file.\n");
	fprintf(stderr, "\t\t\tIf more than one start job is specified,\n\t\t\tthe last one takes precedence.\n");
	fprintf(stderr, "\nRead Trident jobs from one or more input files. If more than one file is specified, they are read as one long, continuous input file.\n");
}

int main (int argc, char **argv)
{
	job_t * startJob = NULL;
	char * startName = NULL;
	int any = 0;

	if (argc <= 1)
	{
		printUsage();
		return -1;
	}

	stack = init();
	yyin = NULL;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-')
		{
			yyin = fopen(argv[i], "r");
			if (yyin == NULL)
			{
				fprintf(stderr, "Failed to open file %s.\n", argv[i]);
				continue;
			}
			any = 1;
			yyparse();
			fclose(yyin);
			continue;
		}
		switch(argv[i][1])
		{
			case 's':
				startName = argv[++i];
				break;
			case 'h':
			case '?':
				printUsage();
				clear(stack);
				return 0;
			default:
				fprintf(stderr, "Unrecognized option: %s", argv[i]);
		}
	}

	if (any == 0)
	{
		printUsage();
		return -1;
	}
	yylex_destroy();

	startJob = stack.data[0];
	if (startName != NULL)
	{
		startJob = query(startName, stack);
		if (startJob == NULL)
			fprintf(stderr, "No job found with name %s\n", startName);
	}
	if (startJob != NULL)
		executeJob(startJob, STDIN_FILENO);

	clear(stack);
	return 0;
}
