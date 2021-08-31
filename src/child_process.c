#include "child_process.h"
#include <fcntl.h>
#include <errno.h>

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
		fprintf(stderr, "Creation of Pipe failed.\n");
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
		perror("Fork failed.\n");
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
