#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LABELS 100
#define MAX_LINES 1024

char *lines[MAX_LINES];
int total_lines = 0;

FILE *fp = 0;
char *filename = NULL;
int acc = 0;
int bak = 0;
int inpt = 0;
unsigned int line = 1;


typedef struct {
	const char *name;
	int *ptr;
} Register;

typedef struct
{
	char label[32];
	int line;
} Label;



Register keyword[] = {
	{"acc", &acc},
	{"inpt", &inpt},
	{"nil", NULL},
	{NULL, NULL}, // end
};

Label labels[MAX_LABELS];
int label_count = 0;


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
	COMMENT = 0x00000023,
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
};

int *get_register(const char *name)
{
	for (int i = 0; keyword[i].name != NULL; i++) {
		if (strcmp(keyword[i].name, name) == 0) {
			return keyword[i].ptr;
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
	if (strcmp(str, "add") == 0) return ADD;
	if (strcmp(str, "sub") == 0) return SUB;
	if (strcmp(str, "neg") == 0) return NEG;
	if (strcmp(str, "jmp") == 0) return JMP;
	if (strcmp(str, "jez") == 0) return JEZ;
	if (strcmp(str, "jnz") == 0) return JNZ;
	if (strcmp(str, "jgz") == 0) return JGZ;
	if (strcmp(str, "jlz") == 0) return JLZ;
	if (strcmp(str, "jro") == 0) return JRO;
	if (strcmp(str, "#") == 0) return COMMENT;
	return 0;
}

void parse_labels(FILE *fp)
{
	char linebuf[128];
	int current_line = 0;

	while (fgets(linebuf, sizeof(linebuf), fp)) {
		char *colon = strchr(linebuf, ':');
		current_line++;
		if (!colon)
			continue;

		*colon = '\0';
		char *label_name = strtok(linebuf, " \t\n");
		if (label_name && label_count < MAX_LABELS) {
			strncpy(labels[label_count].label, label_name, 31);
			labels[label_count].line = current_line;
			label_count++;
		}
	}
	rewind(fp);
}

int find_label_line(const char *label_name)
{
	for (int i = 0; i < label_count; i++) {
		if (strcmp(labels[i].label, label_name) == 0)
			return labels[i].line;
	}
	return -1;
}

void jmp_func(int matched, char arg1[16], int *pc) {
	if (matched >= 2) {
		int target = find_label_line(arg1);
		if (target >= 0) {
			*pc = target - 1;
		} else {
			printf("%s:%d: unknown label '%s'\n", filename, line, arg1);
			return;
		}
	} else {
		printf("%s:%d: jmp requires a label\n", filename, line);
		return;
	}
}

int exec_line(const char *linebuf, int *pc)
{
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

	switch (instruction) {
	case MOV:
		if (matched == 3) {
			src_ptr = get_register(arg1);

			if (strcmp(arg2, "nil") == 0) {
				printf("%s:%d: invalid mov: cannot write to 'nil'\n", filename, line);
				return 0;
			} else if (strcmp(arg1, "nil") == 0 && strcmp(arg2, "nil") == 0) {
				printf("%s:%d: invalid mov: cannot use 'nil' as both source and destination\n", filename, line);
				return 0;
			} else if (strcmp(arg2, "acc") != 0) {
				printf("%s:%d: invalid mov: destination must be 'acc'\n", filename, line);
				return 0;
			} else if (strcmp(arg1, "nil") == 0) {
				acc = 0;
			} else if (src_ptr) {
				acc = *src_ptr;
			} else {
				int val = atoi(arg1);
				acc = val;
			}
		} else {
			printf("%s:%d: mov requires two arguments\n", filename, line);
			return 0;
		}
		break;

	case SAV:
		bak = acc;
		break;

	case SWP:
		int tmp = acc;
		acc = bak;
		bak = tmp;
		break;

	case COMMENT:
		break;

	case NOP:
		acc = 0;
		break;

	case ADD:
		src_ptr = get_register(arg1);
		if (src_ptr != NULL) {
			acc += *src_ptr;
		} else {
			acc += atoi(arg1);
		}
		break;

	case SUB:
		src_ptr = get_register(arg1);
		if (src_ptr != NULL) {
			acc -= *src_ptr;
		} else {
			acc -= atoi(arg1);
		}
		break;

	case NEG:
		acc *= -1;
		break;

	case JMP:
		jmp_func(matched, arg1, pc);
		break;

	case JEZ:
		if (acc == 0)
			jmp_func(matched, arg1, pc);
		break;

	case JNZ:
		if (acc != 0)
			jmp_func(matched, arg1, pc);
		break;

	case JGZ:
		if (acc > 0)
			jmp_func(matched, arg1, pc);
		break;

	case JLZ:
		if (acc < 0)
			jmp_func(matched, arg1, pc);
		break;

	case JRO:
		src_ptr = get_register(arg1);
		if (src_ptr) {
			*pc += *src_ptr;
		} else {
			*pc += atoi(arg1);
		}
		(*pc)--; // корректировка (pc++ в цикле)
		break;

	default:
		printf("%s:%d: unknown instruction '%s'\n", filename, line, op);
		return 0;
	}

	line++;
	return 1;
}

char *cli(int argc, char *argv[])
{
	char *input_file = NULL;
	int input_data = 0;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
			input_data = atoi(argv[++i]);
			inpt = input_data;
		} else {
			input_file = argv[i];
		}
	}
	if (!input_file) {
		return NULL;
	}
	return input_file;
}


void load_program(FILE *fp) {
	char buffer[128];
	while (fgets(buffer, sizeof(buffer), fp) && total_lines < MAX_LINES) {
		lines[total_lines++] = strdup(buffer);
	}
}

int main(int argc, char *argv[])
{
	filename = cli(argc, argv);
	if (filename == NULL) return 1;

	fp = fopen(filename, "r");
	if (!fp) {
		perror("fopen");
		return 1;
	}

	parse_labels(fp);
	load_program(fp);
	fclose(fp);

	int pc = 0;
	while (pc < total_lines) {
		if (!exec_line(lines[pc], &pc)) break;
		printf("acc %d\n", acc);
		printf("pc %d\n", pc);
		printf("bak %d\n", bak);
		printf("inpt %d\n", inpt);
		pc++;
	}
	return 0;
}
