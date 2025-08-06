#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "cli.h"
#include "emulator.h"

int main(int argc, char *argv[]) {

	Emulator emu = {0};
	emu.line = 1;
	emu.filename = cli(argc, argv, &emu);

	if (!emu.filename) return 1;

	emu.fp = fopen(emu.filename, "r");
	if (!emu.fp) {
		perror("fopen");
		return 1;
	}

	clock_t start, end;

	parse_labels(&emu);
	load_program(&emu);
	fclose(emu.fp);

	start = clock();

	while (emu.pc < emu.total_lines) {
		if (!exec_line(&emu, emu.lines[emu.pc])) break;
		if (emu.debug) debug_message(&emu);
		emu.pc++;
	}
	end = clock();
	double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("work time %.6fs\n", cpu_time_used);

	for (int i = 0; i < emu.total_lines; i++) {
		free(emu.lines[i]);
	}

	return 0;
}
