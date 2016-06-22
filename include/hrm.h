#ifndef HRM_UTILS_HRM_H_
#define HRM_UTILS_HRM_H_

#include <stdint.h>

/* Opcodes */

#define JUMP		0x0000		/* so that 0000 is nop */
#define JUMPZ		0x0100
#define JUMPN		0x0200
#define OUTBOX		0x1000
#define INBOX		0x1100
#define ADD		0x2000
#define ADDI		0x3000
#define SUB		0x4000
#define SUBI		0x5000
#define	BUMPUP		0x6000
#define	BUMPUPI		0x7000
#define BUMPDN		0x8000
#define BUMPDNI		0x9000
#define COPYTO		0xa000
#define COPYTOI		0xb000
#define COPYFROM	0xc000
#define COPYFROMI	0xd000

#define INDIRECT_BIT	0x1000

/* Limits and tests */

#define HRM_DATASIZE	256		/* data memory size (in words) */
#define HRM_TEXTSIZE	256		/* text memory size (in words) */

#define MIN_NUMBER	-999
#define MAX_NUMBER	999
#define MIN_LETTER	(MAX_NUMBER + 1)
#define MAX_LETTER	(MIN_LETTER + 'Z' - 'A')
#define EMPTY		(MIN_NUMBER - 1)
#define LETTER(x)	((x) - 'A' + MIN_LETTER)
#define IS_EMPTY(x)	((x) == EMPTY)
#define IS_NUMBER(x)	((x) >= MIN_NUMBER && (x) <= MAX_NUMBER)
#define IS_LETTER(x)	((x) >= MIN_LETTER && (x) <= MAX_LETTER)

/* Types */

typedef int16_t		Word;

#endif

