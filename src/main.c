#include "child_process.h"

int main (int argc, char **argv)
{
	if (argc <= 1)
	{
		fprintf(stderr, "USAGE: multipipe CMD [ARGS]\n");
		fprintf(stderr, "Calls the command CMD with the optional arguments ARGS.\n");
		fprintf(stderr, "If CMD contains a '/' character, it is interpreted as the exact path to an executable file (can be relative or absolute). Otherwise, the environment variable PATH is used to search an executable with the corresponding name.\n");
		return EXIT_FAILURE;
	}

	int child_io[2];
	int return_value;

	return_value = create_child_process(child_io, argc - 1, argv + 1);
	if (return_value != EXIT_SUCCESS)
		return return_value;

	/* Do stuff with the child process pipes */

	return EXIT_SUCCESS;
}
