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
	int size;
	int arg;
	uint8_t opcode;
};

static struct instruction instruction[] = {
	{ "INBOX",	1, 0, INBOX	},
	{ "OUTBOX",	1, 0, OUTBOX	},
	{ "BUMPUP",	1, 1, BUMPUP	},
	{ "BUMPDN",	1, 1, BUMPDN	},
	{ "COPYTO",	1, 1, COPYTO	},
	{ "COPYFROM",	1, 1, COPYFROM	},
	{ "ADD",	1, 1, ADD	},
	{ "SUB",	1, 1, SUB	},
	{ "JUMP",	2, 2, JUMP	},
	{ "JUMPZ",	2, 2, JUMPZ	},
	{ "JUMPN",	2, 2, JUMPN	},
	{ NULL,		0, 0, 0		}
};

static struct symbol symtable[SYMTABLE_SIZE];
static int st_index = 0;
static int addr;
static uint8_t bin[HRM_TEXTSIZE];


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

static void add_symtable(char *name, int addr)
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

static uint8_t get_argument(char *arg)
{
	uint8_t val;

	if (arg[0] == '[') {
		/* indirect addressing */
		return atoi(arg + 1) | 0x10;
	} else {
		return atoi(arg);	
	}
}

static void pass1(FILE *f)
{
	char line[LINE_SIZE];
	char *l, *label, *instr, *arg;

	printf("\nPASS 1: ");

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
			add_symtable(label, addr); 
		}

		struct instruction *ins;
		for (ins = instruction; ins->ins; ins++) {
			if (!strcmp(instr, ins->ins)) {
				addr += ins->size;
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

	printf("\nPASS 2: ");

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
					uint8_t op = ins->opcode;
					printf("%3d  ", addr);
					if (ins->arg == 0) {
						bin[addr] = op;
						printf("%02X         %-8.8s", op, instr);
					} else if (ins->arg == 1) {
						op |= get_argument(arg);
						bin[addr] = op;
						printf("%02X         %-8.8s %s", op, instr, arg);
					} else if (ins->arg == 2) {
						bin[addr] = op;
						bin[addr + 1] = 0x55;
						printf("%02X %02X      %-8.8s %s", op, bin[addr + 1], instr, arg);
					}
					printf("\n");
					addr += ins->size;
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
	FILE *f;
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

	/* save object code */

	exit(EXIT_SUCCESS);
}
