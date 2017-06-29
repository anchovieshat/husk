extern crate byteorder;
extern crate husk_lib;

use std::env;
use std::fs::File;
use std::io::{Read, Write};
use std::collections::HashMap;

use byteorder::{LittleEndian, WriteBytesExt};
use husk_lib::{Op, OpType, Reg};
use husk_lib::get_op_type;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("Incorrect number of arguments!");
        return;
    }

    let mut ops = HashMap::new();
    ops.insert("NOP", Op::NOP);
    ops.insert("SET", Op::SET);
    ops.insert("DEREF", Op::DEREF);
    ops.insert("LOAD", Op::LOAD);
    ops.insert("STORE", Op::STORE);
    ops.insert("LOADI", Op::LOADI);
    ops.insert("STOREI", Op::STOREI);
    ops.insert("ADD", Op::ADD);
    ops.insert("SUB", Op::SUB);
    ops.insert("MUL", Op::MUL);
    ops.insert("DIV", Op::DIV);
    ops.insert("MOD", Op::MOD);
    ops.insert("ADDI", Op::ADDI);
    ops.insert("SUBI", Op::SUBI);
    ops.insert("MULI", Op::MULI);
    ops.insert("DIVI", Op::DIVI);
    ops.insert("MODI", Op::MODI);
    ops.insert("INC", Op::INC);
    ops.insert("DEC", Op::DEC);
    ops.insert("SWAP", Op::SWAP);
    ops.insert("OUT", Op::OUT);
    ops.insert("IN", Op::IN);

    let mut reg = HashMap::new();
    reg.insert("A", Reg::A);
    reg.insert("B", Reg::B);

    let mut asm_file = File::open(&args[1]).unwrap();

    let mut tmp_bin = Vec::new();

    let mut asm_str = String::new();
    asm_file.read_to_string(&mut asm_str).unwrap();

    for line in asm_str.lines() {
        let tokens: Vec<&str> = line.split(' ').collect();

        let inst_str = tokens[0];
        if !ops.contains_key(inst_str) {
            println!("Unsupported instruction {:?}", inst_str);
            break;
        }

        let inst_hex = *ops.get(inst_str).unwrap() as u64;

        let op: Op = unsafe { std::mem::transmute(inst_hex) };
        let op_type = get_op_type(op);

        match op_type {
            OpType::N => {
                let emit = inst_hex;
                tmp_bin.write_u64::<LittleEndian>(emit).unwrap();
                println!("{}; EMIT 0x{:x}", tokens[0], emit);
            },
            _ => {
                if !reg.contains_key(tokens[1]) {
                    println!("Unsupported register: {:?}", tokens[1]);
                    break;
                }

                let reg1_hex = *reg.get(tokens[1]).unwrap() as u64;

                match op_type {
                    OpType::R => {
                        let reg2_lookup = reg.get(tokens[2]);
                        if reg2_lookup.is_some() {
                            let reg2_hex = *reg2_lookup.unwrap() as u64;
                            let emit = inst_hex | reg1_hex << 8 | reg2_hex << 16;
                            tmp_bin.write_u64::<LittleEndian>(emit).unwrap();
                            println!("{} {} {}; EMIT 0x{:x}", tokens[0], tokens[1], tokens[2], emit);
                        } else {
                            println!("{} | Invalid argument 2 '{}', should be a register!", line, tokens[2]);
                            break;
                        }
                    },
                    OpType::I => {
                        let data_hex;

                        // Attempt to use as immediate?
                        let arg2_r = tokens[2].parse::<u32>();
                        if arg2_r.is_ok() {
                            data_hex = arg2_r.unwrap() as u64;
                        } else {

                            // Attempt to parse char?
                            let possible_char = tokens[2].trim_matches('\'');
                            if possible_char.len() == 1 {
                                data_hex = possible_char.chars().nth(0).unwrap() as u64;
                            } else {
                                println!("Error with argument 2; {}", tokens[2]);
                                break;
                            }
                        }

                        let emit = inst_hex | reg1_hex << 8 | data_hex << 32;
                        tmp_bin.write_u64::<LittleEndian>(emit).unwrap();
                        println!("{} {} {}; EMIT 0x{:x}", tokens[0], tokens[1], tokens[2], emit);
                    },
                    _ => {
                        println!("{} {:?} unhandled type!", inst_str, op_type);
                        break;
                    }
                }
            }
        }
    }

    let mut bin_file = File::create(&args[2]).unwrap();
    bin_file.write_all(&tmp_bin).unwrap();
}
