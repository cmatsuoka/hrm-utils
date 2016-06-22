#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "hrm.h"
#include "cpu.h"

static char *ex[] = {
	[E_DATA_OVERFLOW]	= "Integer overflow",
	[E_ILLEGAL_INSTRUCTION]	= "Illegal instruction",
	[E_END_OF_EXECUTION]	= "End of execution",
	[E_EMPTY_DATA]		= "Empty data register",
	[E_EMPTY_ACC]		= "Empty accumulator",
	[E_LETTER_ARITH]	= "Letter in arithmetic operation",
	[E_OUT_OF_BOUNDS]	= "Out of bounds indirection",
};

// Fibonacci code
static uint8_t code[] = {
	0x03, 0x09, 0x41, 0x00, 0x05, 0x00, 0x06, 0x09,
	0x05, 0x01, 0x05, 0x02, 0x40, 0x00, 0x06, 0x00,
	0x02, 0x02, 0x22, 0xf7, 0x06, 0x01, 0x01, 0x02,
	0x05, 0x02, 0x02, 0x01, 0x05, 0x01, 0x20, 0xf6
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
	return scanf("%hd", val) == 1;
}

static int outbox(Word val)
{
	printf("OUTBOX: %d\n", val);
	return 0;
}

static void usage(char *cmd)
{
	printf("Usage: %s [-d]\n", cmd);
	printf("  -d   show debug information\n");
}

int main(int argc, char **argv)
{
	int datasize = 10;
	int debug = 0;
	int o;

	while ((o = getopt(argc, argv, "d")) != -1) {
		switch (o) {
		case 'd':
			debug = 1;
			break;
		default:
			usage(argv[0]);
			exit(EXIT_SUCCESS);
		}
	}

	struct cpu *cpu = new_cpu(datasize);
	if (cpu == NULL) {
		fprintf(stderr, "error: can't create CPU\n");
		exit(EXIT_FAILURE);
	}

	cpu->debug = debug;
	cpu->exception = exception;
	cpu->inbox = inbox;
	cpu->outbox = outbox;

	if (load_code(cpu, code, sizeof (code)) < 0) {
		fprintf(stderr, "error: can't load code\n");
		exit(EXIT_FAILURE);
	}

	cpu->data[9] = 0;

	run_cpu(cpu);

	printf("Executed %d instructions\n", cpu->clock);
	
	exit(EXIT_SUCCESS);
}
