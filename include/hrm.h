#ifndef HRM_UTILS_HRM_H_
#define HRM_UTILS_HRM_H_

#include <stdint.h>

/* Opcodes */

#define OUTBOX		0x4000
#define INBOX		0x4100
#define JUMP		0x2000
#define JUMPZ		0x2100
#define JUMPN		0x2200
#define ADD		0x0100
#define ADDI		0x1100
#define SUB		0x0200
#define SUBI		0x1200
#define	BUMPUP		0x0300
#define	BUMPUPI		0x1300
#define BUMPDN		0x0400
#define BUMPDNI		0x1400
#define COPYTO		0x0500
#define COPYTOI		0x1500
#define COPYFROM	0x0600
#define COPYFROMI	0x1600

#define IO_MASK		0x4000
#define JUMP_MASK	0x2000
#define INDIRECT_MASK	0x1000

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

