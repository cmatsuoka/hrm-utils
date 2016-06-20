#ifndef HRM_UTILS_cpu_H_
#define HRM_UTILS_cpu_H_

#include <stdint.h>
#include <setjmp.h>
#include "hrm.h"

#define DATASIZE 16

struct cpu {
	uint32_t clock;
	uint8_t datasize;
	uint8_t ip;
	uint8_t flags;
	uint8_t ir;			/* current instruction */
	Word dr;			/* data */
	Word acc;
	Word data[DATASIZE];
	uint8_t dest;			/* jump destination */
	uint8_t *text;
	int (*inbox)(void);
	int (*outbox)(void);
	int (*exception)(struct cpu *, int);
	jmp_buf jmpbuf;
};

/* Exception codes */

#define E_DATA_OVERFLOW		(-1)	/* Integer overflow */
#define E_ILLEGAL_INS		(-2)	/* Illegal instruction */
#define E_END_OF_EXECUTION	(-3)	/* End of execution */
#define E_EMPTY_DATA		(-4)	/* Empty data register */
#define E_EMPTY_ACC		(-5)	/* Empty accumulator */
#define E_LETTER_ARITH		(-6)	/* Letter in arithmetic operation */
#define E_OUT_OF_BOUNDS		(-7)	/* Out of bounds indirection */

void	reset		(struct cpu *);

#endif
