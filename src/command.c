#include <stdlib.h>
#include "command.h"

command_t * newCommand (char * name)
{
	command_t * cmd = malloc(sizeof(command_t));
	if (cmd == NULL)
		return NULL;

	cmd->argc = 1;
	cmd->alloc = 4;
	cmd->argv = malloc(4 * sizeof(char *));
	cmd->argv[0] = name;
	return cmd;
}

command_t * appendArgument (command_t * cmd, char * arg)
{
	cmd->argv[cmd->argc] = arg;
	cmd->argc ++;

	if (cmd->argc == cmd->alloc)
	{
		cmd->argv = realloc(cmd->argv, cmd->alloc * 2 * sizeof(char *));
		if (cmd->argv == NULL)
		{
			free(cmd);
			return NULL;
		}
		else
			cmd->alloc *= 2;
	}

	return cmd;
}

void clearCommand (command_t * cmd)
{
	for (int i = 0; i < cmd->argc; i++)
		free(cmd->argv[i]);
	free(cmd->argv);
	free(cmd);
}
