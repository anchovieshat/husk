extern crate byteorder;
extern crate husk_lib;

use std::fs::File;
use std::io::Read;
use std::io::Cursor;

use byteorder::{LittleEndian, ReadBytesExt};
use husk_lib::{Op, OpType, Reg};
use husk_lib::get_op_type;

struct CPUState {
    mem: Vec<u64>,
    pc: usize,
    reg: [u64; 3],
    running: bool
}

impl CPUState {
    fn new() -> CPUState {
        CPUState {
            mem: Vec::new(),
            pc: 0,
            reg: [0, 0, 0],
            running: true,
        }
    }

    fn tick(&mut self) {
        let bytecode = self.mem[self.pc];

        let opcode = bytecode << 56 >> 56;
        let reg1_hex = bytecode << 48 >> 56;
        let reg2_hex = bytecode << 40 >> 56;
        let data = bytecode >> 32;


        let op: Op = unsafe { std::mem::transmute(opcode) };

        let op_type = get_op_type(op);
        if op_type == OpType::E {
            println!("0x{:x}: {:?} doesn't have an op type!", bytecode, op);
            self.running = false;
            return;
        }

        let reg1: Reg = unsafe { std::mem::transmute(reg1_hex) };
        let reg2: Reg = unsafe { std::mem::transmute(reg2_hex) };

        match op_type {
            OpType::I => {
                println!("0x{:x}: {:?} {:?} {}", bytecode, op, reg1, data);
            },
            OpType::R => {
                println!("0x{:x}: {:?} {:?} {:?}", bytecode, op, reg1, reg2);
            },
            OpType::N => {
                println!("0x{:x}: {:?}", bytecode, op);
            },
            _ => {
                println!("0x{:x}: {:?} {:?} unhandled type!", bytecode, op, op_type);
                self.running = false;
                return;
            },
        }

        match op {
            Op::NOP => {},
            Op::SET => { self.reg[reg1_hex as usize] = data; },
            Op::LOAD => { self.reg[reg2_hex as usize] = self.mem[reg1_hex as usize]; },
            Op::STORE => { self.mem[reg2_hex as usize] = self.reg[reg1_hex as usize]; },
            Op::LOADI => { self.reg[reg1_hex as usize] = self.mem[data as usize]; },
            Op::STOREI => { self.mem[data as usize] = self.reg[reg1_hex as usize]; },
            Op::ADD => { self.reg[reg1_hex as usize] += self.reg[reg2_hex as usize]; },
            Op::SUB => { self.reg[reg1_hex as usize] -= self.reg[reg2_hex as usize]; },
            Op::OUT => { println!("{}", self.reg[reg1_hex as usize] as u8 as char); },
            Op::HALT => { self.running = false; },
            _ => { println!("Op not implemented!"); self.running = false; },
        }

        self.pc += 1;
    }
}

fn main() {
    let mut cpu = CPUState::new();

    let mut bin_data = Vec::new();
    let mut bin_file = File::open("test.bin").unwrap();
    bin_file.read_to_end(&mut bin_data).unwrap();

    let mut cur = Cursor::new(bin_data);

    loop {
        let inst = cur.read_u64::<LittleEndian>();
        match inst {
            Ok(n) => { cpu.mem.push(n); },
            Err(_) => { break; }
        }
    }

    while cpu.running && cpu.pc < cpu.mem.len() {
        cpu.tick();
    }
}
