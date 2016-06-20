#include <stdio.h>
#include <stdlib.h>
#include "hrm.h"
#include "cpu.h"

static char *ex[] = {
	[E_DATA_OVERFLOW]	= "Integer overflow",
	[E_ILLEGAL_INS]		= "Illegal instruction",
	[E_END_OF_EXECUTION]	= "End of execution",
	[E_EMPTY_DATA]		= "Empty data register",
	[E_EMPTY_ACC]		= "Empty accumulator",
	[E_LETTER_ARITH]	= "Letter in arithmetic operation",
	[E_OUT_OF_BOUNDS]	= "Out of bounds indirection",
};

static void exception(struct cpu *cpu, int num)
{
	char *desc = "unknown";

	if (num < sizeof(ex)) {
		desc = ex[num];
	}

	printf("received exception %d! (%s)\n", num, desc);
}

static int inbox(Word *val)
{
	*val = 0;
	printf("INBOX: %d\n", *val);
	return 0;
}

static int outbox(Word val)
{
	printf("OUTBOX: %d\n", val);
	return 0;
}

int main(int argc, char **argv)
{
	int datasize = 16;

	struct cpu *cpu = new_cpu(datasize);
	if (cpu == NULL) {
		fprintf(stderr, "error: can't create CPU\n");
		exit(EXIT_FAILURE);
	}

	install_exception_handler(cpu, exception);
	install_inbox_handler(cpu, inbox);
	install_outbox_handler(cpu, outbox);

	run_cpu(cpu);
	
	exit(EXIT_SUCCESS);
}
