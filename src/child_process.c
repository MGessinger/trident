#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <regex.h>
#include "child_process.h"

#define TYPE (O_RDWR | O_CREAT | O_TRUNC)
#define MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

extern stack_t stack;

static inline int is_valid_fd (int fd)
{
	return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

int create_child_process (int * parent_io, int argc, char * const argv[])
{
	int from_child[2];

	/* Create the output. */
	if (pipe(from_child) == -1)
	{
		perror("pipe");
		return EXIT_FAILURE;
	}

	/* Create the child process. */
	int f = fork();
	if (f < 0)
	{
		/* This is an error. */
		close(from_child[0]);
		close(from_child[1]);
		*parent_io = -1;
		perror("fork");
		return EXIT_FAILURE;
	}
	else if (f == 0)
	{
		/* This is the child. */
		close(from_child[0]);

		dup2(*parent_io, STDIN_FILENO);
		dup2(from_child[1], STDOUT_FILENO);

		char * args[argc + 1];
		for (int i = 0; i < argc; i++)
			args[i] = argv[i];
		args[argc] = NULL;

		execvp(args[0], args);

		return EXIT_FAILURE;
	}
	else
	{
		/* This is the parent. */
		close(from_child[1]);
		*parent_io = from_child[0];
		return EXIT_SUCCESS;
	}
}

void startTargets (targets_t * Ts)
{
	target_t * T;
	job_t * targetJob;
	int variableFD[2];

	for (int i = 0; i < Ts->length; i++)
	{
		T = Ts->targets + i;
		if (T->type == TARGET_JOB)
		{
			targetJob = query(T->name, stack);
			if (targetJob == NULL)
			{
				fprintf(stderr, "Invalid name: No job with the name %s. Skipping Target.\n", T->name);
				T->file = -1;
				continue;
			}
			if (pipe(variableFD) != 0)
			{
				perror("startTargets: pipe");
				T->file = -1;
				continue;
			}
			int f = fork();
			if (f < 0)
			{
				perror("startTargets: fork");
				continue;
			}
			else if (f == 0)
			{
				close(variableFD[1]);
				executeJob(targetJob, variableFD[0]);
				return;
			}
			else
			{
				close(variableFD[0]);
				T->file = variableFD[1];
			}
		}
		else
		{
			if (strncmp(T->name, "stdout", 6) == 0)
				T->file = STDOUT_FILENO;
			else if (strncmp(T->name, "stderr", 6) == 0)
				T->file = STDERR_FILENO;
			else
				T->file = open(T->name, TYPE, MODE);

			if (!is_valid_fd(T->file))
				perror(T->name);
		}
	}
}

void runDistributor (targets_t * Ts, int inputFD)
{
	target_t * T;
	FILE * nonstdout;
	char * line = NULL;
	size_t length = 0;
	ssize_t bytes;
	int match;

	nonstdout = fdopen(inputFD, "r");
	if (nonstdout == NULL)
		return;

	bytes = getline(&line, &length, nonstdout);
	while (bytes > 0)
	{
		for (int i = 0; i < Ts->length; i++)
		{
			T = Ts->targets + i;
			match = 1;
			if (!is_valid_fd(T->file))
				continue;
			if (T->filter != NULL)
				match = (regexec(T->filter, line, 0, NULL, REG_NOTEOL) != REG_NOMATCH);
			if (T->invert)
				match = !match;

			if (match)
				write(T->file, line, bytes);
		}
		bytes = getline(&line, &length, nonstdout);
	}
	for (int i = 0; i < Ts->length; i++)
	{
		T = Ts->targets + i;
		if (!is_valid_fd(T->file))
			continue;
		close(T->file);
		T->file = -1;
	}

	free(line);
	fclose(nonstdout);
}

void executeJob (job_t * job, int fd)
{
	if (job == NULL)
		return;

	create_child_process(&fd, job->argc, job->argv);

	startTargets(job->targets);
	runDistributor(job->targets, fd);
}
