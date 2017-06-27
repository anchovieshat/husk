#ifndef CPU_H
#define CPU_H

typedef enum Op {
	NOP, LOAD, LOADI, STORE, STOREI, SET, DEREF,
	ADD, SUB, MUL, DIV, MOD, INC, DEC,
	GE, LE, EQ, ISZERO,
	JMP, JZ, JG, JL, JE, JNE, JERR, RET,
	SWAP, NOT, AND, OR, XOR, SHL, SHR,
	IN, OUT,
	HALT, ERROR
} Op;

typedef enum Reg {
	RegA, RegB
} Reg;

typedef enum OPType {
	I, R, E, N, H
} OPType;

#endif
