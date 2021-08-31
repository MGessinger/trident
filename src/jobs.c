#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "jobs.h"
#include "child_process.h"

#define MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

job_t * newJob (char * name, command_t * cmd, targets_t * targets)
{
	if (name == NULL || cmd == NULL)
		return NULL;

	job_t * j = malloc(sizeof(job_t));
	if (j == NULL)
		return NULL;

	j->name = name;
	j->argc = cmd->argc;
	j->argv = cmd->argv;
	j->targets = targets;

	free(cmd);

	return j;
}

void startTargets (int ** fds, targets_t * Ts)
{
	target_t T;
	job_t * targetJob;
	int variableFD[2];
	int * files = malloc(Ts->length * sizeof(int));
	if (files == NULL)
	{
		*fds = NULL;
		return;
	}

	for (int i = 0; i < Ts->length; i++)
	{
		T = Ts->targets[i];
		switch (T.type)
		{
			case TARGET_FILE:
				files[i] = open(T.name, O_RDWR | O_CREAT, MODE);
				if (files[i] == -1)
					perror(T.name);
				break;
			case TARGET_JOB:
				targetJob = NULL; /* Implement findJobOnStack (char * name) */
				if (pipe(variableFD) != 0)
				{
					perror("pipe");
					files[i] = -1;
					continue;
				}
				executeJob(targetJob, variableFD[0]);
				files[i] = variableFD[1];
		}
	}
	*fds = files;
}

void runDistributor (int * files, targets_t * Ts, int inputFD)
{
	if (files == NULL)
		return;

	target_t T;
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
			if (files[i] == -1)
				continue;
			T = Ts->targets[i];
			if (T.filter != NULL)
				match = (regexec(T.filter, line, 0, NULL, REG_NOTEOL) != REG_NOMATCH);
			else
				match = 1;
			if (match)
				write(files[i], line, bytes);
		}
		bytes = getline(&line, &length, nonstdout);
	}
	int EOT = 0x04;
	for (int i = 0; i < Ts->length; i++)
	{
		if (files[i] == -1)
			continue;
		if (Ts->targets[i].type == TARGET_FILE)
			close(files[i]);
		else
			write(files[i], &EOT, 4);
	}

	free(line);
	fclose(nonstdout);
}

void executeJob (job_t * job, int fd)
{
	if (job == NULL)
		return;

	FILE * nonstdout;
	int * files;

	create_child_process(&fd, job->argc, job->argv);

	startTargets(&files, job->targets);
	runDistributor(files, job->targets, fd);
	free(files);
}

void clearJob (job_t * job)
{
	if (job == NULL)
		return;

	for (int i = 0; i < job->argc; i++)
		free(job->argv[i]);
	free(job->argv);
	free(job->name);
	clearTargets(job->targets);

	free(job);
}
