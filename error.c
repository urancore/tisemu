#include <stdarg.h>

#include "emulator.h"

void errorf(Emulator *emu, const char *fmt, ...)
{
	const char *red = emu->color ? "\033[1;31m" : ""; // bold red
	const char *reset = emu->color ? "\033[0m" : "";

	fprintf(stderr, "%s%s:%d: error: ", red, emu->filename, emu->line);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);

	fprintf(stderr, "%s\n", reset);
}
