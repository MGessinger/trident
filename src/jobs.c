#include <stdlib.h>
#include <string.h>
#include "jobs.h"

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

stack_t init ()
{
	stack_t s;

	s.depth = 0;
	s.alloc = 0;
	s.data = NULL;
	return s;
}

stack_t push (stack_t in, job_t * elem)
{
	if (in.depth >= in.alloc)
	{
		in.data = realloc(in.data, (in.alloc + 4) * sizeof(job_t *));
		if (in.data == NULL)
		{
			in.alloc = 0;
			in.depth = 0;
			return in;
		}
		in.alloc += 4;
	}
	in.data[in.depth] = elem;
	in.depth++;
	return in;
}

job_t * query (char * name, stack_t in)
{
	if (in.data == NULL)
		return NULL;

	for (int i = 0; i < in.depth; i++)
	{
		if (strcmp(name, in.data[i]->name) == 0)
			return in.data[i];
	}
	return NULL;
}

void clear (stack_t s)
{
	for (int i = 0; i < s.depth; i++)
		clearJob(s.data[i]);
	free(s.data);
}
