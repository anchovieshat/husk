#include "../include/common.h"
#include "../include/file_helper.h"
#include "../include/cpu.h"

typedef struct Instruction {
	u8 op;
	u8 reg1;
	u8 reg2;
	u8 type;
	u32 data;
} Instruction;

#define MEM_SIZE 2048
#define MMIO_SIZE 100
typedef struct CPUState {
	u64 *mem;
	u64 *mmio;
	u64 pc;
	u32 reg[2];
	u64 sp;
	u64 ticks;
	bool running;
} CPUState;

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
		case ADD: { return R; } break;
		case HALT: { return H; } break;
		case ERROR: { return E; } break;
		default: { return E; } break;
	}
}

Instruction load_inst(CPUState *cpu) {
	u64 bytecode = cpu->mem[cpu->pc];
	Instruction i;
	i.op = bytecode << 56 >> 56;
	i.reg1 = bytecode << 48 >> 56;
	i.reg2 = bytecode << 40 >> 56;
	i.data = bytecode >> 32;
	i.type = get_op_type(i.op);
	return i;
}

Instruction parse_inst(CPUState *cpu) {
    Instruction inst = load_inst(cpu);

	const char *op_str = get_op_str(inst.op);
	const char *reg1_str = get_reg_str(inst.reg1);
	switch (inst.type) {
		case I: {
			printf("%s %s %d | 0x%llx\n", op_str, reg1_str, inst.data, cpu->mem[cpu->pc]);
		} break;
		case R: {
			const char *reg2_str = get_reg_str(inst.reg2);
			printf("%s %s %s | 0x%llx\n", op_str, reg1_str, reg2_str, cpu->mem[cpu->pc]);
		} break;
		case H: { } break;
		default: {
			cpu->running = false;
			printf("DECODING ERROR!\n");
		} break;
	}

	return inst;
}

void cpu_set(CPUState *cpu, u8 r_idx, u32 x) {
	cpu->reg[r_idx] = x;
}

void cpu_store(CPUState *cpu, u8 r_idx, u32 dest) {
	u32 val = cpu->reg[r_idx];
	cpu->mem[dest] = val;
}

void cpu_load(CPUState *cpu, u8 r_idx, u32 src) {
	cpu->reg[r_idx] = cpu->mem[src];
}

void cpu_out(CPUState *cpu, u8 r_idx, u32 dest) {
	u32 val = cpu->reg[r_idx];
	cpu->mmio[dest] = val;
	printf("%c\n", val);
}

void cpu_add(CPUState *cpu, u8 r_idx1, u8 r_idx2) {
	cpu->reg[r_idx1] += cpu->reg[r_idx2];
}

void cpu_sub(CPUState *cpu, u8 r_idx1, u8 r_idx2) {
	cpu->reg[r_idx1] -= cpu->reg[r_idx2];
}

void exec_inst(Instruction inst, CPUState *cpu) {
	switch (inst.op) {
		case ADD: {
			cpu_add(cpu, inst.reg1, inst.reg2);
		} break;
		case SUB: {
			cpu_sub(cpu, inst.reg1, inst.reg2);
		} break;
		case SET: {
			cpu_set(cpu, inst.reg1, inst.data);
		} break;
		case LOAD: {
			cpu_load(cpu, inst.reg1, inst.data);
		} break;
		case STORE: {
			cpu_store(cpu, inst.reg1, inst.data);
		} break;
		case OUT: {
			cpu_out(cpu, inst.reg1, inst.data);
		} break;
		case HALT: {
			cpu->running = false;
		} break;
		default: {
			printf("Instruction wasn't run!\n");
			cpu->running = false;
		}
	}

	cpu->pc += 1;
}

CPUState build_cpu() {
	CPUState cpu;
	cpu.mem = (u64 *)malloc(sizeof(u64) * MEM_SIZE);
	for (u64 i = 0; i < MEM_SIZE; i++) {
		cpu.mem[i] = HALT;
	}
	cpu.mmio = (u64 *)calloc(sizeof(u64), MMIO_SIZE);

	for (u64 i = 0; i < 2; i++) {
		cpu.reg[i] = 0;
	}

	cpu.pc = 0;
	cpu.sp = 0;
	cpu.ticks = 0;
	cpu.running = true;

	return cpu;
}

void print_cpu_state(CPUState *cpu) {
	printf("\t[%llu] Reg A: %d, Reg B: %d\n", cpu->pc, cpu->reg[0], cpu->reg[1]);
}

int main() {
    CPUState cpu = build_cpu();

	BinFile *bin_file = read_bin_file("test.bin");
	memcpy(cpu.mem, bin_file->data, bin_file->size);

	/*
	cpu.mem[0] = SET | (u64)RegA << 8 | (u64)'a' << 32;
	cpu.mem[1] = STORE | (u64)RegA << 8 | (u64)1 << 32;
	cpu.mem[2] = LOAD | (u64)RegB << 8 | (u64)1 << 32;
	cpu.mem[3] = OUT | (u64)RegB << 8 | (u64)0 << 32;
	*/

	while (cpu.running && cpu.pc < MEM_SIZE) {
 		Instruction inst = parse_inst(&cpu);
		exec_inst(inst, &cpu);

		cpu.ticks++;
	}

	return 0;
}
