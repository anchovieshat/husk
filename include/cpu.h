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

const char *get_op_str(u8 op) {
	switch (op) {
		case NOP: { return "NOP"; } break;
		case LOAD: { return "LOAD"; } break;
		case LOADI: { return "LOADI"; } break;
		case STORE: { return "STORE"; } break;
		case STOREI: { return "STOREI"; } break;
		case SET: { return "SET"; } break;
		case DEREF: { return "DEREF"; } break;
		case ADD: { return "ADD"; } break;
		case SUB: { return "SUB"; } break;
		case MUL: { return "MUL"; } break;
		case DIV: { return "DIV"; } break;
		case MOD: { return "MOD"; } break;
		case INC: { return "INC"; } break;
		case DEC: { return "DEC"; } break;
		case GE: { return "GE"; } break;
		case LE: { return "LE"; } break;
		case EQ: { return "EQ"; } break;
		case ISZERO: { return "ISZERO"; } break;
		case JMP: { return "JMP"; } break;
		case JZ: { return "JZ"; } break;
		case JG: { return "JG"; } break;
		case JL: { return "JL"; } break;
		case JE: { return "JE"; } break;
		case JNE: { return "JNE"; } break;
		case JERR: { return "JERR"; } break;
		case RET: { return "RET"; } break;
		case SWAP: { return "SWAP"; } break;
		case NOT: { return "NOT"; } break;
		case AND: { return "AND"; } break;
		case OR: { return "OR"; } break;
		case XOR: { return "XOR"; } break;
		case SHL: { return "SHL"; } break;
		case SHR: { return "SHR"; } break;
		case IN: { return "IN"; } break;
		case OUT: { return "OUT"; } break;
		case HALT: { return "HALT"; } break;
		case ERROR: { return "ERROR"; } break;
		default: { return "Invalid OP"; } break;
	}
}

const char *get_reg_str(u8 reg) {
	switch (reg) {
		case RegA: { return "A"; } break;
		case RegB: { return "B"; } break;
		default: { return "Invalid Reg"; } break;
	}
}

OPType get_op_type(u8 op) {
    switch (op) {
		case NOP: { return N; } break;
		case SET: { return I; } break;
		case LOAD: { return I; } break;
		case LOADI: { return I; } break;
		case STORE: { return I; } break;
		case STOREI: { return I; } break;
		case OUT: { return I; } break;
		case IN: { return I; } break;
		case ADD: { return R; } break;
		case HALT: { return H; } break;
		case ERROR: { return E; } break;
		default: { return E; } break;
	}
}

#endif
