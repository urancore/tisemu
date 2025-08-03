#include <stdio.h>

FILE *fp = 0;
int acc = 0;
int bak = 0;
int inpt = 0;
unsigned int line = 1;

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
};

unsigned int parse_symbol(void)
{
	char buf[5] = {0};
	fscanf(fp, "%s ", (char *)(&buf));
	return *(int *)(buf);
}

int parse_num(void)
{
	int num;
	fscanf(fp, "%d", &num);
	return num;
}

int exec(void)
{
	int instruction = parse_symbol();
	if (!instruction)
		return 0;
	switch (instruction) {
	case MOV:
		int val = parse_num();
		if (parse_symbol() == ACC)
			acc = val;
		break;
	case SAV:
		bak = acc;
		break;
	case SWP:
		int tmp = acc;
		acc = bak;
		bak = tmp;
		break;
	case INPT:
		printf("inpt detected\n");
		break;
	default:
		printf("t.asm:%d: illegal opcode\n", line);
		return 0;
	}
	line++;
	return 1;
}

int main(void)
{
	fp = fopen("test.ets", "rb");
	while (exec()) {}
	printf("acc %d\n", acc);
	printf("bak %d\n", bak);
	return 0;
}
