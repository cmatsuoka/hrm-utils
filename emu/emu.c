#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
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

#if 0
// Fibonacci code
static uint8_t code[] = {
	0x60, 0x09, 0x11, 0x00, 0xa0, 0x00, 0xc0, 0x09,
        0xa0, 0x01, 0xa0, 0x02, 0x10, 0x00, 0xc0, 0x00,
        0x40, 0x02, 0x02, 0xf7, 0xc0, 0x01, 0x20, 0x02,
	0xa0, 0x02, 0x40, 0x01, 0xa0, 0x01, 0x00, 0xf6
};
#endif
 
static void exception(struct cpu *cpu, int num)
{
	char *desc = "unknown";

	if (num == E_END_OF_EXECUTION)
		return;
	
	if (num < sizeof(ex)) {
		desc = ex[num];
	}

	printf("** Exception %d (%s) IP=%d\n", num, desc, cpu->last_ip);
}

static int inbox(DataWord *val)
{
	printf("INBOX: ");
	fflush(stdout);
	return scanf("%hd", val) == 1;
}

static int stdinbox(DataWord *val)
{
	return scanf("%hd", val) == 1;
}

static int outbox(DataWord val)
{
	printf("OUTBOX: %d\n", val);
	return 0;
}

static void usage(char *cmd)
{
	printf("Usage: %s [-d] [-s] <binfile>\n", cmd);
	printf("  -d   show debug information\n");
	printf("  -s   read INBOX values from STDIN\n");
}

int main(int argc, char **argv)
{
	int datasize = 10;
	int debug = 0;
	int usestdin = 0;
	int o;
	extern int optind;

	while ((o = getopt(argc, argv, "ds")) != -1) {
		switch (o) {
		case 'd':
			debug = 1;
			break;
		case 's':
			usestdin = 1;
			break;
		default:
			usage(argv[0]);
			exit(EXIT_SUCCESS);
		}
	}

	if (optind >= argc) {
		usage(argv[0]);
		exit(EXIT_SUCCESS);
	}
	char *input = argv[optind];

	FILE *f;
	struct stat st;
	if ((f = fopen(input, "rb")) == NULL) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (fstat(fileno(f), &st) < 0) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	uint8_t *code;
	if ((code = malloc(st.st_size)) == NULL) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}
	if ((fread(code, 1, st.st_size, f)) != st.st_size) {
		fprintf(stderr, "%s: error reading input file\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	fclose(f);

	struct cpu *cpu = new_cpu(datasize);
	if (cpu == NULL) {
		fprintf(stderr, "error: can't create CPU\n");
		exit(EXIT_FAILURE);
	}

	cpu->debug = debug;
	cpu->exception = exception;
	if (usestdin) {
		cpu->inbox = stdinbox;
	} else {
		cpu->inbox = inbox;
	}
	cpu->outbox = outbox;

	if (load_code(cpu, code, st.st_size) < 0) {
		fprintf(stderr, "error: can't load code\n");
		exit(EXIT_FAILURE);
	}
	free(code);

	cpu->data[9] = 0;

	run_cpu(cpu);

	printf("Executed %d instructions\n", cpu->clock);
	
	exit(EXIT_SUCCESS);
}
