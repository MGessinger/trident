#ifndef CHILD_PROCESS_H_
#define CHILD_PROCESS_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int create_child_process (int * parent_io, int argc, char * const argv[]);

#endif /* CHILD_PROCESS_H_ */
