#include <errno.h>
#include <assert.h>

#include "../include/common.h"
#include "../include/file_helper.h"
#include "../include/hashmap.h"
#include "../include/cpu.h"

int main(int arg_len, char **args) {
    if (arg_len != 3) {
		puts("incorrect number of arguments!");
		return 1;
	}

	HashMap *ops = hm_init();
	hm_insert(&ops, "LOAD", (void *)(LOAD + 1));
	hm_insert(&ops, "ADD", (void *)(ADD + 1));
	hm_insert(&ops, "OUT", (void *)(OUT + 1));

	HashMap *reg = hm_init();
	hm_insert(&reg, "A", (void *)(RegA + 1));
	hm_insert(&reg, "B", (void *)(RegB + 1));

	File *asm_file = read_file(args[1]);
	FILE *bin_file = fopen(args[2], "wb");

	DynArr *lines = file_into_lines(asm_file->string);

	for (u64 i = 0; i < lines->size; i++) {
		char *save_ptr = NULL;

		char *inst = strtok_r(lines->buffer[i], " ", &save_ptr);

		u64 *inst_hex_ptr = hm_get(ops, inst);
		if (inst_hex_ptr == NULL) {
			printf("Unsupported instruction: %s\n", inst);
			break;
		}
		u64 inst_hex = ((u64)inst_hex_ptr) - 1;

		char *arg1 = strtok_r(NULL, " ", &save_ptr);

		u64 *reg1_hex_ptr = hm_get(reg, arg1);
		if (reg1_hex_ptr == NULL) {
			printf("Unsupported register: %s\n", arg1);
			break;
		}

		u64 reg1_hex = ((u64)reg1_hex_ptr) - 1;

		char *arg2 = strtok_r(NULL, " ", &save_ptr);
		char *arg3 = strtok_r(NULL, " ", &save_ptr);
		if (arg3 != NULL) {
			printf("Invalid line [%llu]: %s\n", i, lines->buffer[i]);
			break;
		}

		u64 emit;
		u64 *reg2_hex_ptr = hm_get(reg, arg2);
		if (reg2_hex_ptr == NULL) {
			u64 arg2_val = strtol(arg2, (char **)NULL, 10);
			if (errno == EINVAL) {
				if (arg2[0] == '\'' && arg2[2] == '\'') {
					arg2_val = arg2[1];
					errno = 0;
				} else {
					printf("Invalid argument %s\n", arg2);
					break;
				}
			}
			emit = inst_hex | reg1_hex << 8 | arg2_val << 32;
		} else {
			u64 reg2_hex = ((u64)reg2_hex_ptr) - 1;
			emit = inst_hex | reg1_hex << 8 | reg2_hex << 16;
		}

		printf("%s %s %s; EMIT 0x%llx\n", inst, arg1, arg2, emit);
		fwrite(&emit, sizeof(u64), 1, bin_file);
	}

	fclose(bin_file);

	return 0;
}
