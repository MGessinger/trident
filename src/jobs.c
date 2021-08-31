#include <stdlib.h>
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
