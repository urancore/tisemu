#ifndef CLI_H
#define CLI_H

#include "emulator.h"

void print_banner(Emulator *emu);
char *cli(int argc, char *argv[], Emulator *emu);

#endif
