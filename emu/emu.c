#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

int main(int argc, char **argv)
{
	int datasize = 16;

	CPU *c = new_cpu(datasize);
	if (c == NULL) {
		fprintf(stderr, "error: can't create CPU\n");
		exit(EXIT_FAILURE);
	}

	run_cpu(c);
	
	exit(EXIT_SUCCESS);
}
