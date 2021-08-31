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

typedef struct {
	job_t ** data;
	int depth;
	int alloc;
} stack_t;

job_t * newJob (char * name, command_t * cmd, targets_t * targets);
void clearJob (job_t * job);

stack_t init ();
stack_t push (stack_t in, job_t * elem);
job_t * query (char * name, stack_t in);
void clear (stack_t s);

#endif /* JOBS_H_ */
