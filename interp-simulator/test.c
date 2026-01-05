#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MINT_MOV 0
#define MINT_ADD1 1
#define MINT_ADD 2
#define MINT_RET 3
#define MINT_ADD2 4
#define MINT_ADD3 5
#define MINT_ADD4 6
#define MINT_ADD5 7
#define MINT_ADD6 8
#define MINT_ADD7 9
#define MINT_ADD8 10
#define MINT_ADD9 11
#define MINT_ADD10 12
#define MINT_ADD11 13
#define MINT_ADD12 14
#define MINT_ADD13 15
#define MINT_ADD14 16
#define MINT_ADD15 17

#define USE_COMPUTED_GOTO 1

#ifdef USE_COMPUTED_GOTO
#define MINT_SWITCH(x) goto *mint_labels[*ip];
#define MINT_CASE(x) LABEL_ ## x
#define MINT_BREAK goto *mint_labels[*ip]
#else
#define MINT_SWITCH(x) switch (x)
#define MINT_CASE(x) case x
#define MINT_BREAK break
#endif

__attribute__((noinline))
static void interp_exec_method(const uint8_t *ip, int32_t *stack)
{
#ifdef USE_COMPUTED_GOTO
	static void * const mint_labels[] = { &&LABEL_MINT_MOV, &&LABEL_MINT_ADD1, &&LABEL_MINT_ADD, &&LABEL_MINT_RET,
				&&LABEL_MINT_ADD2,
				&&LABEL_MINT_ADD3,
				&&LABEL_MINT_ADD4,
				&&LABEL_MINT_ADD5,
				&&LABEL_MINT_ADD6,
				&&LABEL_MINT_ADD7,
				&&LABEL_MINT_ADD8,
				&&LABEL_MINT_ADD9,
				&&LABEL_MINT_ADD10,
				&&LABEL_MINT_ADD11,
				&&LABEL_MINT_ADD12,
				&&LABEL_MINT_ADD13,
				&&LABEL_MINT_ADD14,
//				&&LABEL_MINT_ADD15,
	};
#endif

	while (1) {
		MINT_SWITCH(*ip) {
			MINT_CASE(MINT_MOV):
				stack[ip[1]] = stack[ip[2]];
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD1):
				stack[ip[1]] = stack[ip[2]] + 1;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD):
				stack[ip[1]] = stack[ip[2]] + stack[ip[3]];
				ip += 4;
				MINT_BREAK;
			MINT_CASE(MINT_RET):
				goto exit_frame;
			MINT_CASE(MINT_ADD2):
				stack[ip[1]] = stack[ip[2]] + 2;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD3):
				stack[ip[1]] = stack[ip[2]] + 3;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD4):
				stack[ip[1]] = stack[ip[2]] + 4;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD5):
				stack[ip[1]] = stack[ip[2]] + 5;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD6):
				stack[ip[1]] = stack[ip[2]] + 6;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD7):
				stack[ip[1]] = stack[ip[2]] + 7;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD8):
				stack[ip[1]] = stack[ip[2]] + 8;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD9):
				stack[ip[1]] = stack[ip[2]] + 9;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD10):
				stack[ip[1]] = stack[ip[2]] + 10;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD11):
				stack[ip[1]] = stack[ip[2]] + 11;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD12):
				stack[ip[1]] = stack[ip[2]] + 12;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD13):
				stack[ip[1]] = stack[ip[2]] + 13;
				ip += 3;
				MINT_BREAK;
			MINT_CASE(MINT_ADD14):
				stack[ip[1]] = stack[ip[2]] + 14;
				ip += 3;
				MINT_BREAK;
//			MINT_CASE(MINT_ADD15):
//				stack[ip[1]] = stack[ip[2]] + 15;
//				ip += 3;
//				MINT_BREAK;
		}
	}

exit_frame:
	return;
}


int main(int argc, char *argv[])
{
	int32_t *stack = (int32_t*)alloca(128);

	// stack[0] = (1 + 2 x 10) + 2
	const uint8_t ip[] = {
		0, 2, 0,		// mov 8 <- 0
		0, 3, 1,		// mov 12 <- 4
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		4, 2, 2,		// add1 8 <- 8
		2, 2, 2, 3,		// add 8 <- 8 + 12
		0, 0, 2,		// mov 0 <- 8
		3				// ret
	};


	for (int i = 0; i < 200000000; i++) {
		stack[0] = 1;
		stack[1] = 2;
		interp_exec_method (ip, stack);
	}

	printf ("Result is %d\n", stack[0]);
}


