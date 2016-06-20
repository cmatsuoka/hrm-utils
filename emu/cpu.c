#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "cpu.h"


struct cpu {
	uint32_t clock;
	uint8_t datasize;		/* data memory size */
	uint8_t ip;			/* instruction pointer */
	uint8_t flags;			/* flags register */
	uint8_t ir;			/* instruction register */
	Word dr;			/* data register */
	Word acc;			/* accumulator */
	Word data[HRM_DATASIZE];	/* data memory */
	uint8_t text[HRM_TEXTSIZE];	/* text memory */
	uint8_t dest;			/* jump destination */
	int (*inbox)(void);		/* data input callback */
	int (*outbox)(void);		/* data output callback */
	int (*exception)(struct cpu *, int);
	jmp_buf jmpbuf;
};

static void Exception(struct cpu *cpu, int num)
{
	cpu->exception(cpu, num);
	longjmp(cpu->jmpbuf, num);
}


static void read_instruction(struct cpu *cpu)
{
	cpu->ir = cpu->text[cpu->ip++];

	if (cpu->ir & 0xf0 == JUMP) {
		cpu->dest = cpu->text[cpu->ip++];
	}
}

static void read_data(struct cpu *cpu)
{
	Word data;

	switch (cpu->ir & 0xf0) {
	case RSVD1:
	case RSVD2:
		Exception(cpu, E_ILLEGAL_INS);
		break;
	case JUMP:
	case IO:
		/* no data */
		break;
	default: {
		Word data = cpu->data[cpu->ir & 0x0f];

		if (IS_EMPTY(data)) {
			Exception(cpu, E_EMPTY_DATA);
		}

		if (cpu->ir & 0x10) {
			if (data >= cpu->datasize) {
				Exception(cpu, E_OUT_OF_BOUNDS);
			}
			cpu->dr = cpu->data[data];
			break;
		} else {
			cpu->dr = data;
		}
		break; }
	}
	
}

static void execute(struct cpu *cpu)
{
	switch (cpu->ir & 0xe0) {
	case JUMP:
		break;
	case IO:
		break;
	case ADD:
		if (IS_EMPTY(cpu->acc)) {
			Exception(cpu, E_EMPTY_ACC);
		}
		if (!IS_NUMBER(cpu->acc) || !IS_NUMBER(cpu->dr)) {
			Exception(cpu, E_LETTER_ARITH);
		}

		cpu->acc += cpu->dr;
		break;
	case SUB:
		if (IS_EMPTY(cpu->acc)) {
			Exception(cpu, E_EMPTY_ACC);
		}
		if (!IS_NUMBER(cpu->acc) || !IS_NUMBER(cpu->dr)) {
			Exception(cpu, E_LETTER_ARITH);
		}

		cpu->acc -= cpu->dr;
		break;
	case BUMPUP:
		if (!IS_NUMBER(cpu->dr)) {
			Exception(cpu, E_EMPTY_ACC);
		}

		if (cpu->ir & 0x10) {
			cpu->acc = ++cpu->data[cpu->data[cpu->ir & 0x0f]];
		} else {
			cpu->acc = ++cpu->data[cpu->ir & 0x0f];
		}
		break;
	case BUMPDN:
		if (!IS_NUMBER(cpu->dr)) {
			Exception(cpu, E_EMPTY_ACC);
		}

		if (cpu->ir & 0x10) {
			cpu->acc = --cpu->data[cpu->data[cpu->ir & 0x0f]];
		} else {
			cpu->acc = --cpu->data[cpu->ir & 0x0f];
		}
		break;
	case COPYTO:
		if (IS_EMPTY(cpu->acc)) {
			Exception(cpu, E_EMPTY_ACC);
		}

		if (cpu->ir & 0x10) {
			cpu->data[cpu->data[cpu->ir & 0x0f]] = cpu->acc;
		} else {
			cpu->data[cpu->ir & 0x0f] = cpu->acc;
		}
		break;
	case COPYFROM:
		cpu->acc = cpu->dr;
		break;
	}
}

static void cpu_cycle(struct cpu *cpu)
{
	read_instruction(cpu);
	read_data(cpu);
	execute(cpu);
	cpu->clock++;
}

/* Public calls */

CPU new_cpu(int datasize)
{
	struct cpu *cpu;

	if ((cpu = malloc(sizeof (struct cpu))) == NULL) {
		goto err;
	}

	cpu->datasize = datasize;

	reset_cpu(cpu);

	return (CPU)cpu;

    err:
	return NULL;
}


void reset_cpu(CPU c)
{
	struct cpu *cpu = (struct cpu *)c;
	int i;

	memset(cpu, 0, sizeof (struct cpu));

	cpu->acc = EMPTY;

	for (i = 0; i < cpu->datasize; i++) {
		cpu->data[i] = EMPTY;
	}
}

void run_cpu(CPU c)
{
	struct cpu *cpu = (struct cpu *)c;

	if (setjmp(cpu->jmpbuf) < 0) {
		return;
	}

	while (42) {
		cpu_cycle(cpu);
	}
}
