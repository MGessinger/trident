#ifndef COMMAND_H_
#define COMMAND_H_

typedef struct {
	int argc;
	int alloc;
	char ** argv;
} command_t;

command_t * newCommand (char * name);
command_t * appendArgument (command_t * cmd, char * arg);
void clearCommand (command_t * cmd);

#endif /* COMMAND_H_ */
