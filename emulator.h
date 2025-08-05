#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>

#define MAX_LABELS 100
#define MAX_LINES 1024
#define MAX_LABEL_LEN 32


typedef struct {
	char label[MAX_LABEL_LEN];
	int line;
} Label;


typedef struct {
	int acc;
	int bak;
	int inpt;
	int pc;
	unsigned int line;

	unsigned char banner;
	unsigned char debug;
	unsigned char color;

	char *filename;
	FILE *fp;

	char *lines[MAX_LINES];
	int total_lines;

	Label labels[MAX_LABELS];
	int label_count;
} Emulator;


typedef struct {
	const char *name;
	int *ptr;
} Register;


/*
 * values here are sequences of chars in little endian
 * for example mov is \0vom
 */
enum {
	ACC = 0x00636361,
	MOV = 0x00766f6d,
	SAV = 0x00766173,
	SWP = 0x00707773,
	INPT = 0x74706e69,
	NIL = 0x006c696e,
	NOP = 0x00706F6E,
	ADD = 0x00646461,
	SUB = 0x00627573,
	NEG = 0x0067656E,
	JMP = 0x00706D6A,
	JEZ = 0x007A656A,
	JNZ = 0x007A6E6A,
	JGZ = 0x007A676A,
	JLZ = 0x007A6C6A,
	JRO = 0x006F726A,
	OUT = 0x0074756F,
	COMMENT = 0x00000023,
};

Register *get_keywords(Emulator *emu);
int *get_register(Emulator *emu, const char *name);

int str_to_token(const char *str);
void parse_labels(Emulator *emu);
int find_label_line(Emulator *emu, const char *label_name);
void jmp_func(Emulator *emu, int matched, char arg1[16]);
int exec_line(Emulator *emu, const char *linebuf_input);
void load_program(Emulator *emu);
void debug_message(Emulator *emu);

#endif
