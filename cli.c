#include <stdlib.h>
#include <string.h>

#include "meta.h"
#include "emulator.h"

void print_banner(Emulator *emu)
{
	char *cyan    = "";
	char *green   = "";
	char *yellow  = "";
	char *reset   = "";
	char *bold    = "";

	if (emu->color) {
		cyan    = "\033[36m";
		green   = "\033[32m";
		yellow  = "\033[33m";
		reset   = "\033[0m";
		bold    = "\033[1m";
	}

	printf("%s%s", cyan, bold);
	printf("  ,--------.,--. ,---.                                 \n");
	printf("  '--.  .--'|  |'   .-'   ,---. ,--,--,--.,--.,--.     \n");
	printf("     |  |   |  |`.  `-.  | .-. :|        ||  ||  |     \n");
	printf("     |  |   |  |.-'    | \\   --.|  |  |  |'  ''  '    \n");
	printf("     `--'   `--'`-----'   `----'`--`--`--' `----'      \n");
	printf("                                                       \n");
	printf("-------------------------------------------------------\n");
	printf("%s", reset);

	printf("%s%s", green, bold);
	printf("TISemu - A TIS-100 emulator in C\n");
	printf("%s", reset);

	printf("%s", yellow);
	printf(" - Version : %s\n", TISEMU_VERSION);
	printf(" - Author  : %s\n", TISEMU_AUTHOR);
	printf(" - GitHub  : %s\n", TISEMU_GITHUB);
	printf(" - License : %s\n", TISEMU_LICENSE);
	printf("%s\n", reset);
}


void print_help(void)
{
	FILE *f = fopen("help.txt", "r");
	if (!f) {
		fprintf(stderr, "error: help.txt not found\n");
		exit(1);
	}

	char buf[256];
	while (fgets(buf, sizeof(buf), f)) {
		fputs(buf, stdout);
	}
	fclose(f);
	exit(0);
}

void print_doc(void)
{
	FILE *f = fopen("doc/doc.txt", "r");
	if (!f) {
		fprintf(stderr, "error: doc/doc.txt not found\n");
		exit(1);
	}

	char buf[256];
	while (fgets(buf, sizeof(buf), f)) {
		fputs(buf, stdout);
	}
	fclose(f);
	exit(0);
}

char *cli(int argc, char *argv[], Emulator *emu)
{
	char *input_file = NULL;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
		} else if (strcmp(argv[i], "--doc") == 0) {
			print_doc();
		} else if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) && i + 1 < argc) {
			emu->inpt = atoi(argv[++i]);
		} else if (strcmp(argv[i], "--banner") == 0) {
			print_banner(emu);
		} else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
			emu->debug = 1;
		} else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--color") == 0) {
			emu->color = 1;
		} else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
			printf("tisemu version %s\n", TISEMU_VERSION);
			exit(0);
		} else if (argv[i][0] != '-') {
			input_file = argv[i];
		} else {
			fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
			exit(1);
		}
	}

	if (!input_file) {
		fprintf(stderr, "error: no input file provided\n");
		exit(1);
	}

	return input_file;
}
