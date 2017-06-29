#[repr(u64)]
#[derive(Debug, Copy, Clone)]
pub enum Op {
	NOP = 0,
    SET, DEREF,
    LOAD, STORE,
    LOADI, STOREI,
	ADD, SUB, MUL, DIV, MOD,
	ADDI, SUBI, MULI, DIVI, MODI,
    INC, DEC, SWAP,
	GE, LE, EQ, ISZERO,
	JMP, JZ, JG, JL, JE, JNE, JERR, RET,
    NOT, AND, OR, XOR, SHL, SHR,
	NOTI, ANDI, ORI, XORI, SHLI, SHRI,
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
    I, R, J, N, S, E
}

pub fn get_op_type(op: Op) -> OpType {
    match op {
        Op::NOP => { return OpType::N; },
        Op::SET => { return OpType::I; },
        Op::DEREF => { return OpType::R; },
        Op::LOAD => { return OpType::R; },
        Op::STORE => { return OpType::R; },
        Op::LOADI => { return OpType::I; },
        Op::STOREI => { return OpType::I; },
        Op::ADD => { return OpType::R; },
        Op::SUB => { return OpType::R; },
        Op::MUL => { return OpType::R; },
        Op::DIV => { return OpType::R; },
        Op::MOD => { return OpType::R; },
        Op::ADDI => { return OpType::I; },
        Op::SUBI => { return OpType::I; },
        Op::MULI => { return OpType::I; },
        Op::DIVI => { return OpType::I; },
        Op::MODI => { return OpType::I; },
        Op::INC => { return OpType::S; },
        Op::DEC => { return OpType::S; },
        Op::SWAP => { return OpType::R; },
        Op::IN => { return OpType::I; },
        Op::OUT => { return OpType::I; },
        Op::HALT => { return OpType::N; },
        _ => { return OpType::E; }
    }
}
