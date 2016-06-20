#ifndef HRM_UTILS_CPU_H_
#define HRM_UTILS_CPU_H_

#include <stdint.h>
#include <setjmp.h>
#include "hrm.h"

struct cpu {
	uint32_t clock;
	uint8_t datasize;		/* data memory size */
	uint8_t ip;			/* instruction pointer */
	uint8_t ir;			/* instruction register */
	uint8_t dr;			/* data register */
	int8_t dest;			/* jump destination */
	uint8_t last_ip;
	Word acc;			/* accumulator */
	Word data[HRM_DATASIZE];	/* data memory */
	uint8_t text[HRM_TEXTSIZE];	/* text memory */
	int (*inbox)(Word *);		/* data input callback */
	int (*outbox)(Word);		/* data output callback */
	void (*exception)(struct cpu *, int);
	jmp_buf jmpbuf;
	int debug;
};

/* Exception codes */

#define E_DATA_OVERFLOW		1	/* Integer overflow */
#define E_ILLEGAL_INS		2	/* Illegal instruction */
#define E_END_OF_EXECUTION	3	/* End of execution */
#define E_EMPTY_DATA		4	/* Empty data register */
#define E_EMPTY_ACC		5	/* Empty accumulator */
#define E_LETTER_ARITH		6	/* Letter in arithmetic operation */
#define E_OUT_OF_BOUNDS		7	/* Out of bounds indirection */

struct cpu *	new_cpu		(int);
void		reset_cpu	(struct cpu *);
void		run_cpu		(struct cpu *);
int		load_code		(struct cpu *, unsigned char *, size_t);

#endif
