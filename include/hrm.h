#ifndef HRM_UTILS_HRM_H_
#define HRM_UTILS_HRM_H_

#include <stdint.h>

/*
     0        1        2        3 
0    -      ADD    BUMPUP    COPYTO
1 (JUMP)    ADD[]  BUMPUP[]  COPYTO[]
2 (MULTI)   SUB    BUMPDN    COPYFROM
3    -      SUB[]  BUMPDN[]  COPYFROM[]

          
	  7 .. 4   3 .. 0
	+--------+--------+ 
	| instr  |argument| 
	+--------+--------+

*/

/* Opcodes */

#define RSVD1		0x00
#define JUMP		0x10
#define JUMPZ		0x11
#define JUMPN		0x12
#define CALL		0x18		/* not implemented */
#define IO		0x20
#define OUTBOX		0x20
#define INBOX		0x21
#define RET		0x28		/* not implemented */
#define RSVD2		0x30
#define ADD		0x40
#define ADDI		0x50
#define SUB		0x60
#define SUBI		0x70
#define	BUMPUP		0x80
#define	BUMPUPI		0x90
#define BUMPDN		0xa0
#define BUMPDNI		0xb0
#define COPYTO		0xc0
#define COPYTOI		0xd0
#define COPYFROM	0xe0
#define COPYFROMI	0xf0

/* Limits and tests */

#define HRM_DATASIZE	16		/* data memory size (in words) */
#define HRM_TEXTSIZE	256		/* text memory size (in bytes) */

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
typedef void *		CPU;

#endif

