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

// Fibonacci code
static uint8_t code[] = {
	0x89, 0x21, 0xc0, 0xe9, 0xc1, 0xc2, 0x20, 0xe0, 0x62,
	0x12, 0xf6, 0xe1, 0x42, 0xc2, 0x61, 0xc1, 0x10, 0xf4
};
 
static void exception(struct cpu *cpu, int num)
{
	char *desc = "unknown";

	if (num < sizeof(ex)) {
		desc = ex[num];
	}

	printf("** Exception %d (%s) IP=%d\n", num, desc, cpu->last_ip);
}

static int inbox(Word *val)
{
	printf("INBOX: ");
	fflush(stdout);
	scanf("%hd", val);
	printf("\n");
	return 0;
}

static int outbox(Word val)
{
	printf("OUTBOX: %d\n", val);
	return 0;
}

int main(int argc, char **argv)
{
	int datasize = 10;

	struct cpu *cpu = new_cpu(datasize);
	if (cpu == NULL) {
		fprintf(stderr, "error: can't create CPU\n");
		exit(EXIT_FAILURE);
	}

	install_exception_handler(cpu, exception);
	install_inbox_handler(cpu, inbox);
	install_outbox_handler(cpu, outbox);

	if (load_code(cpu, code, sizeof (code)) < 0) {
		fprintf(stderr, "error: can't load code\n");
		exit(EXIT_FAILURE);
	}

	run_cpu(cpu);
	
	exit(EXIT_SUCCESS);
}
