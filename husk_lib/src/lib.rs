#[repr(u64)]
#[derive(Debug, Copy, Clone)]
pub enum Op {
	NOP = 0, LOAD, LOADI, STORE, STOREI, SET, DEREF,
	ADD, SUB, MUL, DIV, MOD, INC, DEC,
	GE, LE, EQ, ISZERO,
	JMP, JZ, JG, JL, JE, JNE, JERR, RET,
	SWAP, NOT, AND, OR, XOR, SHL, SHR,
	IN, OUT,
	HALT, ERROR
}

#[repr(u64)]
#[derive(Debug, Copy, Clone)]
pub enum Reg {
    A = 0, B, SP
}

#[derive(Debug, PartialEq)]
pub enum OpType {
    I, R, J, N, E
}

pub fn get_op_type(op: Op) -> OpType {
    match op {
        Op::NOP => { return OpType::N; },
        Op::SET => { return OpType::I; },
        Op::STORE => { return OpType::I; },
        Op::LOAD => { return OpType::I; },
        Op::OUT => { return OpType::I; },
        _ => { return OpType::E; }
    }
}
