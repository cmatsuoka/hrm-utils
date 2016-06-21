#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "hrm.h"

#define LINE_SIZE 80

struct symbol {
	char *name;
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

static struct symbol symtable[HRM_TEXTSIZE];
static int st_index = 0;
static int addr;
static uint8_t bin[HRM_TEXTSIZE];


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
	symtable[st_index].name = strdup(name);
	symtable[st_index].addr = addr;
	st_index++;
}

static void pass1(FILE *f)
{
	char line[LINE_SIZE];
	char *l, *label, *instr, *arg;

	printf("PASS 1: ");

	while (42) {
		char *t;

		printf(".");
		fflush(stdout);

		if (fgets(line, LINE_SIZE, f) == NULL) {
			break;
		}

		/* remove trailing newline */
		int len = strlen(line);
		while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
			line[len - 1] = 0;
			len--;
		}

		/* remove comments */
		if (t = strstr(line, "--")) {
			*t = 0;
		}

		/* remove spaces */
		for (l = line; *l == ' ' || *l == '\t'; l++);

		/* remove empty lines */
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
	printf("PASS 2:\n");
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
	pass2(f);

	/* save object code */

	exit(EXIT_SUCCESS);
}
