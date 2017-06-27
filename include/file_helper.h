#ifndef FILE_HELPER_H
#define FILE_HELPER_H

#include "common.h"
#include "dynarr.h"

typedef struct File {
	char *filename;
	char *string;
	u64 size;
} File;

typedef struct BinFile {
	char *filename;
	void *data;
	u64 size;
} BinFile;

File *read_file(char *filename) {
	FILE *file = fopen(filename, "r");

	if (file == NULL) {
		printf("%s not found!\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	u64 length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *file_string = (char *)malloc(length + 1);
	length = fread(file_string, 1, length, file);
	file_string[length] = 0;

	fclose(file);

	File *f = (File *)malloc(sizeof(File));
	f->filename = filename;
	f->string = file_string;
	f->size = length;

	return f;
}

BinFile *read_bin_file(char *filename) {
	FILE *file = fopen(filename, "rb");

	if (file == NULL) {
		printf("%s not found!\n", filename);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	u64 length = ftell(file);
	fseek(file, 0, SEEK_SET);

	void *file_bin = (void *)malloc(length);
	length = fread(file_bin, 1, length, file);

	fclose(file);

	BinFile *f = (BinFile *)malloc(sizeof(BinFile));
	f->filename = filename;
	f->data = file_bin;
	f->size = length;

	return f;
}

DynArr *file_into_lines(char *file) {
	char *tok_ptr = NULL;

	DynArr *lines = da_init();
	char *line = strtok_r(file, "\n", &tok_ptr);

	while (line != NULL) {
		da_insert(lines, line);
		line = strtok_r(NULL, "\n", &tok_ptr);
	}

	return lines;
}

#endif
