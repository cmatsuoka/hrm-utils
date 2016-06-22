#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "cpu.h"


static void Exception(struct cpu *cpu, int num)
{
	cpu->exception(cpu, num);
	longjmp(cpu->jmpbuf, num);
}

static void format_opcode(struct cpu *cpu, char *buf, size_t n)
{
	Word ins = cpu->ir & 0xff00;

	if (ins & JUMP_MASK) {
		snprintf(buf, n, "%02X %02X    ", cpu->ir >> 8, cpu->dr);
		switch (ins) {
		case JUMP:
			strncat(buf, "JUMP     ", n);
			break;
		case JUMPZ:
			strncat(buf, "JUMPZ    ", n);
			break;
		case JUMPN:
			strncat(buf, "JUMPN    ", n);
			break;
		}
		char s[10];
		snprintf(s, 10, "%d", cpu->ip + cpu->dr);
		strncat(buf, s, n); 
	} else if (ins & IO_MASK) {
		snprintf(buf, n, "%02X 00    ", cpu->ir >> 8);
		switch (ins) {
		case OUTBOX:
			strncat(buf, "OUTBOX", n);
			break;
		case INBOX:
			strncat(buf, "INBOX", n);
			break;
		}
	} else {
		snprintf(buf, n, "%02X %02X    ", cpu->ir >> 8, cpu->ir & 0xff);
		switch (ins & ~INDIRECT_MASK) {
		case ADD:
			strncat(buf, "ADD     ", n);
			break;
		case SUB:
			strncat(buf, "SUB     ", n);
			break;
		case BUMPUP:
			strncat(buf, "BUMPUP  ", n);
			break;
		case BUMPDN:
			strncat(buf, "BUMPDN  ", n);
			break;
		case COPYTO:
			strncat(buf, "COPYTO  ", n);
			break;
		case COPYFROM:
			strncat(buf, "COPYFROM", n);
			break;
		}

		char s[10];
		if (ins & INDIRECT_MASK) {
			snprintf(s, 10, " [%d]", cpu->dr);
		} else {
			snprintf(s, 10, " %d", cpu->dr);
		}
		strncat(buf, s, n);
	}


}

static void read_instruction(struct cpu *cpu)
{
	cpu->last_ip = cpu->ip;
	cpu->ir = cpu->text[cpu->ip++];
}

static void read_data_address(struct cpu *cpu)
{
	if (~cpu->ir & IO_MASK) {
		uint8_t addr = cpu->ir & 0xff;
		
		if (cpu->ir & INDIRECT_MASK) {
			if (addr >= cpu->datasize) {
				Exception(cpu, E_OUT_OF_BOUNDS);
			}
			cpu->dr = cpu->data[addr];
		} else {
			cpu->dr = addr;
		}
	}
}

static void format_word(Word val, char *buf, size_t n)
{
	if (IS_EMPTY(val)) {
		snprintf(buf, n, "-");
	} else if (IS_NUMBER(val)) {
		snprintf(buf, n, "%d", val);
	} else if (IS_LETTER(val)) {
		snprintf(buf, n, "%c", LETTER(val));
	} else {
		snprintf(buf, n, "?");	
	}
}

static void execute(struct cpu *cpu)
{
	Word data;

	if (cpu->debug) {
		char buf[50], s[10];
		format_opcode(cpu, buf, 50);
		format_word(cpu->acc, s, 10);

		printf("%3d  %-20.20s %4.4s  | ", cpu->last_ip, buf, s);

		for (int i = 0; i < cpu->datasize; i++) {
			format_word(cpu->data[i], s, 10);
			printf("%4.4s", s);
		}
		printf("\n");
	}

	Word ins = cpu->ir & 0xff00;

	switch (ins &= ~INDIRECT_MASK) {
	case JUMP:
		cpu->ip += cpu->dr;
		break;
	case JUMPZ:
		if (ins == JUMPZ && cpu->acc == 0) {
			cpu->ip += cpu->dr;
		}
		break;
	case JUMPN:
		if (!IS_NUMBER(cpu->acc)) {
			Exception(cpu, E_LETTER_ARITH);
		}
		if (ins == JUMPN && cpu->acc < 0) {
			cpu->ip += cpu->dr;
		}
		break;
	case OUTBOX:
		cpu->outbox(cpu->acc);
		cpu->acc = EMPTY;
		break;
	case INBOX: {
		Word val;
		if (!cpu->inbox(&val)) {
			Exception(cpu, E_END_OF_EXECUTION);
		}
		cpu->acc = val;
		break; }
	case ADD:
	case SUB:
		data = cpu->data[cpu->dr];

		if (IS_EMPTY(cpu->acc)) {
			Exception(cpu, E_EMPTY_ACC);
		}
		if (!IS_NUMBER(cpu->acc) || !IS_NUMBER(data)) {
			Exception(cpu, E_LETTER_ARITH);
		}

		if (ins == ADD) {
			cpu->acc += data;
		} else {
			cpu->acc -= data;
		}
		break;
	case BUMPUP:
	case BUMPDN:
		if (!IS_NUMBER(cpu->data[cpu->dr])) {
			Exception(cpu, E_EMPTY_DATA);
		}

		if (ins == BUMPUP) {
			cpu->acc = ++cpu->data[cpu->dr];
		} else {
			cpu->acc = --cpu->data[cpu->dr];
		}
		break;
	case COPYTO:
		if (IS_EMPTY(cpu->acc)) {
			Exception(cpu, E_EMPTY_ACC);
		}

		cpu->data[cpu->dr] = cpu->acc;
		break;
	case COPYFROM:
		cpu->acc = cpu->data[cpu->dr];
		break;
	default:
		Exception(cpu, E_ILLEGAL_INSTRUCTION);
	}
}

static void cpu_cycle(struct cpu *cpu)
{
	cpu->clock++;
	read_instruction(cpu);
	read_data_address(cpu);
	execute(cpu);
}

/* Public calls */

struct cpu *new_cpu(int datasize)
{
	struct cpu *cpu;

	if ((cpu = malloc(sizeof (struct cpu))) == NULL) {
		goto err;
	}

	cpu->datasize = datasize;

	reset_cpu(cpu);

	return cpu;

    err:
	return NULL;
}


void reset_cpu(struct cpu *cpu)
{
	int i;

	cpu->acc = EMPTY;
	cpu->clock = 0;
	cpu->debug = 0;
	cpu->ip = 0;
	cpu->ir = 0;
	cpu->dr = 0;
	cpu->last_ip = 0;

	for (i = 0; i < cpu->datasize; i++) {
		cpu->data[i] = EMPTY;
	}
}

void run_cpu(struct cpu *cpu)
{
	if (setjmp(cpu->jmpbuf) != 0) {
		return;
	}

	while (42) {
		cpu_cycle(cpu);
	}
}

int load_code(struct cpu *cpu, unsigned char *code, size_t n)
{
	if (n > HRM_TEXTSIZE) {
		return -1;
	}

	for (int i = 0; i < n; i++) {
		/* Load code with endian conversion */
		cpu->text[i] = ((Word)code[i * 2]) << 8 | code[i * 2 + 1];
	}

	return 0;
}
