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

void print_cpu_state(CPUState *cpu) {
	printf("\t[%llu] Reg A: %d, Reg B: %d\n", cpu->pc, cpu->reg[0], cpu->reg[1]);
}

void pretty_print_inst(Instruction inst, CPUState *cpu) {
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
			printf("Decoding error!\n");
		} break;
	}
}

Instruction parse_inst(CPUState *cpu) {
    Instruction inst = load_inst(cpu);
    //pretty_print_inst(inst, cpu);

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
}

void cpu_in(CPUState *cpu, u8 r_idx, u32 dest) {
	if (dest == 1) {
		cpu->mmio[dest] = getchar();
	}

	cpu->reg[r_idx] = cpu->mmio[dest];
}

void cpu_add(CPUState *cpu, u8 r_idx1, u8 r_idx2) {
	cpu->reg[r_idx1] += cpu->reg[r_idx2];
}

void cpu_sub(CPUState *cpu, u8 r_idx1, u8 r_idx2) {
	cpu->reg[r_idx1] -= cpu->reg[r_idx2];
}

void handle_mmio(u64 *mmio) {
	if (mmio[0] != 0) {
		printf("%c", (int)mmio[0]);
		mmio[0] = 0;
	}
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
		case IN: {
			cpu_in(cpu, inst.reg1, inst.data);
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

int main() {
    CPUState cpu = build_cpu();

	BinFile *bin_file = read_bin_file("test.bin");
	memcpy(cpu.mem, bin_file->data, bin_file->size);

	while (cpu.running && cpu.pc < MEM_SIZE) {
 		Instruction inst = parse_inst(&cpu);
		exec_inst(inst, &cpu);
		handle_mmio(cpu.mmio);

		cpu.ticks++;
	}

	return 0;
}
