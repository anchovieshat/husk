extern crate byteorder;
extern crate husk_lib;

use std::env;
use std::fs::File;
use std::io::{Read, Write};
use std::collections::HashMap;

use byteorder::{LittleEndian, WriteBytesExt};
use husk_lib::{Op, OpType, Reg};

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("Incorrect number of arguments!");
        return;
    }

    let mut ops = HashMap::new();
    ops.insert("NOP", Op::NOP);
    ops.insert("SET", Op::SET);
    ops.insert("STORE", Op::STORE);
    ops.insert("LOAD", Op::LOAD);
    ops.insert("OUT", Op::OUT);

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

        if !reg.contains_key(tokens[1]) {
            println!("Unsupported register: {:?}", tokens[1]);
            break;
        }

        let reg1_hex = *reg.get(tokens[1]).unwrap() as u64;

        if !reg.contains_key(tokens[2]) {
            let mut arg2_hex;

            // Attempt to use as immediate?
            let arg2_r = tokens[2].parse::<u32>();
            if arg2_r.is_ok() {
                arg2_hex = arg2_r.unwrap() as u64;
            } else {

                // Attempt to parse char?
                let possible_char = tokens[2].trim_matches('\'');
                if possible_char.len() == 1 {
                    arg2_hex = possible_char.chars().nth(0).unwrap() as u64;
                } else {
                    println!("Error with argument 2; {}", tokens[2]);
                    break;
                }
            }

            let emit = inst_hex | reg1_hex << 8 | arg2_hex << 32;
            tmp_bin.write_u64::<LittleEndian>(emit).unwrap();
            println!("{} {} {}; EMIT 0x{:x}", tokens[0], tokens[1], tokens[2], emit);
        } else {
            // Lookup register hex_val
            let reg2_hex = *reg.get(tokens[2]).unwrap() as u64;
            let emit = inst_hex | reg1_hex << 8 | reg2_hex << 16;
            tmp_bin.write_u64::<LittleEndian>(emit).unwrap();
            println!("{} {} {}; EMIT 0x{:x}", tokens[0], tokens[1], tokens[2], emit);
        }
    }

    let mut bin_file = File::create(&args[2]).unwrap();
    bin_file.write_all(&tmp_bin).unwrap();
}
