#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include "hrm.h"

#define LINE_SIZE 80
#define SYMBOL_SIZE 16
#define SYMTABLE_SIZE HRM_TEXTSIZE

struct symbol {
	char name[SYMBOL_SIZE];
	uint8_t addr;
};

struct instruction {
	char *ins;
	CodeWord opcode;
};

static struct instruction instruction[] = {
	{ "INBOX",	INBOX    },
	{ "OUTBOX",	OUTBOX   },
	{ "BUMPUP",	BUMPUP   },
	{ "BUMPDN",	BUMPDN   },
	{ "COPYTO",	COPYTO   },
	{ "COPYFROM",	COPYFROM },
	{ "ADD",	ADD      },
	{ "SUB",	SUB      },
	{ "JUMP",	JUMP     },
	{ "JUMPZ",	JUMPZ    },
	{ "JUMPN",	JUMPN    },
	{ NULL,		0        }
};

static struct symbol symtable[SYMTABLE_SIZE];
static int st_index = 0;
static int addr;
static CodeWord bin[HRM_TEXTSIZE];


static void panic(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(EXIT_FAILURE);
}

static void parse_line(char *l, char **label, char **instr, char **arg)
{
	char *t;

	*label = *instr = *arg = "";

	if (strchr(l, ':')) {
		if ((t = strtok(l, ":")) != NULL) {
			*label = t;
		}

		if ((t = strtok(NULL, " \t")) != NULL) {
			*instr = t;
		}
	} else {
		if ((t = strtok(l, " ")) != NULL) {
			*label = "";
			*instr = t;
		}
	}

	if ((t = strtok(NULL, " \t")) != NULL) {
		*arg = t;
	}
}

static void add_symbol(char *name, int addr)
{
	if (st_index >= SYMTABLE_SIZE) {
		panic("too many symbols");
	}

	/* check if symbol already in table */
	for (int i = 0; i < st_index; i++) {
		if (!strcmp(symtable[i].name, name)) {
			panic("duplicate symbol \"%s\"", name);
		} 
	}

	strncpy(symtable[st_index].name, name, SYMBOL_SIZE - 1);
	symtable[st_index].addr = addr;
	st_index++;
}

static int get_symbol(char *name)
{
	for (int i = 0; i < st_index; i++) {
		if (!strcmp(symtable[i].name, name)) {
			return symtable[i].addr;
		} 
	}

	return -1;
}

static char *read_line(char *l, size_t n, FILE *f)
{
	char *t;

	if ((l = fgets(l, LINE_SIZE, f)) == NULL) {
		return NULL;
	}

	/* remove trailing newline */
	int len = strlen(l);
	while (len > 0 && (l[len - 1] == '\n' || l[len - 1] == '\r')) {
		l[len - 1] = 0;
		len--;
	}

	/* remove comments */
	if ((t = strstr(l, "--")) != NULL) {
		*t = 0;
	}

	/* remove spaces */
	for (; *l == ' ' || *l == '\t'; l++);

	return l;
}

static CodeWord get_argument(char *arg)
{
	uint8_t val;

	if (arg[0] == '[') {
		/* indirect addressing */
		return atoi(arg + 1) | INDIRECT_BIT;
	} else {
		return atoi(arg);	
	}
}

static void pass1(FILE *f)
{
	char line[LINE_SIZE];
	char *l, *label, *instr, *arg;

	printf("\nPASS 1:\n");

	while (42) {
		char *t;

		if ((l = read_line(line, LINE_SIZE, f)) == NULL) {
			break;
		}

		/* ignore empty lines */
		if (*l == 0 || *l == '\n') {
			continue;
		}

		parse_line(l, &label, &instr, &arg);

		if (*label) {
			add_symbol(label, addr); 
		}

		struct instruction *ins;
		for (ins = instruction; ins->ins; ins++) {
			if (!strcmp(instr, ins->ins)) {
				addr++;
				break;
			}
		}
	}

	printf("\nSYMBOL TABLE\n");
	for (int i = 0; i < st_index; i++) {
		printf("%4d ... %s\n", symtable[i].addr, symtable[i].name);
	}
}

static void pass2(FILE *f)
{
	char line[LINE_SIZE];
	char *l, *label, *instr, *arg;

	printf("\nPASS 2:\n");

	while (42) {
		char *t;

		if ((l = read_line(line, LINE_SIZE, f)) == NULL) {
			break;
		}

		/* ignore empty lines */
		if (*l == 0 || *l == '\n') {
			continue;
		}

		parse_line(l, &label, &instr, &arg);

		if (*label) {
			printf("            %s:\n", label);

		}
		if (*instr) {
			struct instruction *ins;
			for (ins = instruction; ins->ins; ins++) {
				if (!strcmp(instr, ins->ins)) {
					CodeWord op = ins->opcode;
					printf("%3d  ", addr);
					if ((op & 0xf000) == OUTBOX) {
						bin[addr] = op;
						printf("%02X %02X      %-8.8s", op >> 8, op & 0xff, instr);
					} else if ((op & 0xf000) == JUMP) {
						int dest = get_symbol(arg);
						if (dest < 0) {
							panic("unknown label %s", arg);
						}
						op |= (uint8_t)(dest - (addr + 1));
						bin[addr] = op;
						printf("%02X %02X      %-8.8s %s", op >> 8, op & 0xff, instr, arg);
					} else {
						op |= get_argument(arg);
						bin[addr] = op;
						printf("%02X %02X      %-8.8s %s", op >> 8, op & 0xff, instr, arg);
					}
					printf("\n");
					addr++;
					break;
				}
			}
		}
	}
}

static void usage(char *cmd)
{
	printf("usage: \n");
}

int main(int argc, char **argv)
{
	FILE *f, *fo;
	int debug = 0;
	int o;
	extern int optind;

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

	/* open source file */
	if ((f = fopen(argv[optind], "r")) == NULL) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	/* pass 1: read labels and create symbol table */
	addr = 0;
	pass1(f);

	/* pass 2: assemble code */
	fseek(f, 0, SEEK_SET);
	addr = 0;
	pass2(f);

	fclose(f);

	/* save object code */
	printf("\nassembled %d bytes\n", addr * 2);
	if ((fo = fopen("a.bin", "wb")) == NULL) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < addr; i++) {
		CodeWord x = bin[i];
		fputc(x >> 8, fo);
		fputc(x & 0xff, fo);
	}

	fclose(fo);

	exit(EXIT_SUCCESS);
}
