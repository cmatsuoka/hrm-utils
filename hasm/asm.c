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

static struct symbol symtable[256];
static int addr;

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

static void pass1(FILE *f)
{
	char line[LINE_SIZE];
	char *l, *label, *instr, *arg;

	while (42) {
		char *t;

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

		//printf("LINE: \"%s\"\n", l);
		parse_line(l, &label, &instr, &arg);
		//printf("[%s] [%s] [%s]\n\n", label, instr, arg);
	}
}

static void pass2(FILE *f)
{
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

	if ((f = fopen(argv[optind], "r")) == NULL) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	addr = 0;
	pass1(f);


	exit(EXIT_SUCCESS);
}
