#ifndef TARGETS_H_
#define TARGETS_H_

#include <regex.h>
#include <stdlib.h>

typedef enum {
	TARGET_FILE,
	TARGET_JOB
} target_type;

typedef struct {
	char * name;
	regex_t * filter;
	target_type type;
} target_t;

typedef struct {
	int length;
	int alloc;
	target_t * targets;
} targets_t;

target_t * newSingleTarget (char * name, target_type type);
target_t * addFilter (target_t * T, regex_t * filter);

targets_t * newTargets (target_t * T);
targets_t * appendTarget (targets_t * Ts, target_t * T);
void clearTargets (targets_t * Ts);

#endif /* TARGETS_H_ */
