#ifndef JOBS_H_
#define JOBS_H_

#include "targets.h"
#include "command.h"

typedef struct {
	char * name;
	int argc;
	char ** argv;
	targets_t * targets;
} job_t;

extern job_t ** jobsStack;

job_t * newJob (char * name, command_t * cmd, targets_t * targets);
void clearJob (job_t * job);

#endif /* JOBS_H_ */
