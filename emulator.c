#include <string.h>
#include <stdlib.h>

#include "emulator.h"
#include "error.h"

Register* get_keywords(Emulator *emu) {
	static Register keywords[5];

	keywords[0].name = "acc";
	keywords[0].ptr = &(emu->acc);

	keywords[1].name = "bak";
	keywords[1].ptr = &(emu->bak);

	keywords[2].name = "inpt";
	keywords[2].ptr = &(emu->inpt);

	keywords[3].name = "nil";
	keywords[3].ptr = NULL;

	keywords[4].name = NULL;
	keywords[4].ptr = NULL;

	return keywords;
}


int *get_register(Emulator *emu, const char *name)
{
	Register *keywords = get_keywords(emu);
	for (int i = 0; keywords[i].name != NULL; i++) {
		if (strcmp(keywords[i].name, name) == 0) {
			return keywords[i].ptr;
		}
	}

	return NULL;
}

int str_to_token(const char *str)
{
	if (strcmp(str, "mov") == 0) return MOV;
	if (strcmp(str, "acc") == 0) return ACC;
	if (strcmp(str, "sav") == 0) return SAV;
	if (strcmp(str, "swp") == 0) return SWP;
	if (strcmp(str, "inpt") == 0) return INPT;
	if (strcmp(str, "nil") == 0) return NIL;
	if (strcmp(str, "nop") == 0) return NOP;
	if (strcmp(str, "add") == 0) return ADD;
	if (strcmp(str, "sub") == 0) return SUB;
	if (strcmp(str, "neg") == 0) return NEG;
	if (strcmp(str, "jmp") == 0) return JMP;
	if (strcmp(str, "jez") == 0) return JEZ;
	if (strcmp(str, "jnz") == 0) return JNZ;
	if (strcmp(str, "jgz") == 0) return JGZ;
	if (strcmp(str, "jlz") == 0) return JLZ;
	if (strcmp(str, "jro") == 0) return JRO;
	if (strcmp(str, "out") == 0) return OUT;
	if (strcmp(str, "#") == 0) return COMMENT;
	return 0;
}

void parse_labels(Emulator *emu)
{
	char linebuf[128];
	int current_line = 0;

	rewind(emu->fp);

	while (fgets(linebuf, sizeof(linebuf), emu->fp)) {
		char *colon = strchr(linebuf, ':');
		current_line++;
		if (!colon)
			continue;

		*colon = '\0';
		char *label_name = strtok(linebuf, " \t\n");
		if (label_name && emu->label_count < MAX_LABELS) {
			strncpy(emu->labels[emu->label_count].label, label_name, 31);
			emu->labels[emu->label_count].line = current_line;
			emu->label_count++;
		}
	}
}

int find_label_line(Emulator *emu, const char *label_name)
{
	for (int i = 0; i < emu->label_count; i++) {
		if (strcmp(emu->labels[i].label, label_name) == 0)
			return emu->labels[i].line;
	}
	return -1;
}

void jmp_func(Emulator *emu, int matched, char arg1[16])
{
	if (matched >= 2) {
		int target = find_label_line(emu, arg1);
		if (target >= 0) {
			emu->pc = target - 1;
		} else {
			errorf(emu, "unknown label '%s'", arg1);
			return;
		}
	} else {
		errorf(emu, "jmp requires a label");
		return;
	}
}

int exec_line(Emulator *emu, const char *linebuf_input)
{
	char linebuf[128];
	strncpy(linebuf, linebuf_input, sizeof(linebuf));
	linebuf[sizeof(linebuf) - 1] = '\0';

	char *comment_start = strchr(linebuf, '#');
	if (comment_start) *comment_start = '\0';

	char op[16], arg1[16], arg2[16];
	int matched = sscanf(linebuf, "%15s %15s %15s", op, arg1, arg2);
	if (matched <= 0)
		return 0;

	size_t len = strlen(op);
	if (op[len - 1] == ':') {
		return 1;
	}

	int instruction = str_to_token(op);
	int *src_ptr = NULL;
	// emu->instruction = instruction;

	switch (instruction) {
	case MOV:
		if (matched == 3) {
			src_ptr = get_register(emu, arg1);
			int arg2_token = str_to_token(arg2);
			if (arg2_token == NIL) {
				errorf(emu, "invalid mov: cannot write to 'nil'");
				return 0;
			} else if (arg2_token != ACC && arg2_token != OUT) {
				errorf(emu, "invalid mov: destination must be 'acc'");
				return 0;
			} else if (strcmp(arg1, "nil") == 0 && arg2_token == ACC) {
				emu->acc = 0;
			} else if (strcmp(arg1, "nil") == 0 && arg2_token == OUT) {
				printf("%d\n", 0);
			} else if (src_ptr) {
				if (arg2_token == ACC) {
					emu->acc = *src_ptr;
				} else if (arg2_token == OUT) {
					printf("%d\n", *src_ptr);
				}
			} else {
				int val = atoi(arg1);
				if (arg2_token == ACC) {
					emu->acc = val;
				} else if (arg2_token == OUT) {
					printf("%d\n", val);
				}
			}
		} else {
			errorf(emu, "mov requires two arguments");
			return 0;
		}
		break;
	case SAV:
		emu->bak = emu->acc;
		break;

	case SWP:
		int tmp = emu->acc;
		emu->acc = emu->bak;
		emu->bak = tmp;
		break;

	case COMMENT:
		break;

	case NOP:
		emu->acc = 0;
		break;

	case ADD:
		if (matched != 2) {
			errorf(emu, "sub requires one argument");
			return 0;
		}

		src_ptr = get_register(emu, arg1);
		if (src_ptr != NULL) {
			emu->acc += *src_ptr;
		} else {
			emu->acc += atoi(arg1);
		}
		break;

	case SUB:
		if (matched != 2) {
			errorf(emu, "sub requires one argument");
			return 0;
		}

		src_ptr = get_register(emu, arg1);
		if (src_ptr != NULL) {
			emu->acc -= *src_ptr;
		} else {
			emu->acc -= atoi(arg1);
		}
		break;

	case NEG:
		emu->acc *= -1;
		break;

	case JMP:
		jmp_func(emu, matched, arg1);
		break;

	case JEZ:
		if (emu->acc == 0)
			jmp_func(emu, matched, arg1);
		break;

	case JNZ:
		if (emu->acc != 0)
			jmp_func(emu, matched, arg1);
		break;

	case JGZ:
		if (emu->acc > 0)
			jmp_func(emu, matched, arg1);
		break;

	case JLZ:
		if (emu->acc < 0)
			jmp_func(emu, matched, arg1);
		break;

	case JRO:
		src_ptr = get_register(emu, arg1);
		if (src_ptr) {
			emu->pc += *src_ptr;
		} else {
			emu->pc += atoi(arg1);
		}
		(emu->pc)--; // корректировка (pc++ в цикле)
		break;

	default:
		errorf(emu, "unknown instruction '%s'", op);
		return 0;
	}

	emu->line = emu->pc;
	return 1;
}

void load_program(Emulator *emu)
{
	char buffer[128];
	emu->total_lines = 0;
	rewind(emu->fp);

	while (fgets(buffer, sizeof(buffer), emu->fp) && emu->total_lines < MAX_LINES) {
		char *comment_start = strchr(buffer, '#');
		if (comment_start) *comment_start = '\0';

		emu->lines[emu->total_lines++] = strdup(buffer);
	}
}

void debug_message(Emulator *emu)
{
	const char *label_color = emu->color ? "\033[1;36m" : "";
	const char *value_color = emu->color ? "\033[1;33m" : "";
	const char *reset       = emu->color ? "\033[0m"     : "";

	printf("%s[DEBUG INFO]%s\n", label_color, reset);

	printf("%sline:%s  %s%u%s\n", label_color, reset, value_color, emu->line, reset);
	printf("%spc  :%s  %s%d%s\n", label_color, reset, value_color, emu->pc, reset);
	printf("%sinpt:%s  %s%d%s\n", label_color, reset, value_color, emu->inpt, reset);
	printf("%sacc :%s  %s%d%s\n", label_color, reset, value_color, emu->acc, reset);
	printf("%sbak :%s  %s%d%s\n", label_color, reset, value_color, emu->bak, reset);

	printf("%s-------------------%s\n", label_color, reset);
}
