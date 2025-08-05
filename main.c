#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cli.h"
#include "emulator.h"

// TODO: create func get_version
// TODO: delete global vars
// TODO: delete out instruction / add logic: mov acc out - mov inpt out -e etc...
// TODO: create error buffer
// TODO: rewrite mov logic

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


	parse_labels(&emu);
	load_program(&emu);
	fclose(emu.fp);

	while (emu.pc < emu.total_lines) {
		if (!exec_line(&emu, emu.lines[emu.pc])) break;
		if (emu.debug) debug_message(&emu);
		emu.pc++;
	}

	for (int i = 0; i < emu.total_lines; i++) {
		free(emu.lines[i]);
	}

	return 0;
}
