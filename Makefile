CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -g
LDFLAGS = -static-libgcc -Wl,--gc-sections

O = out
SRC = $(wildcard *.c)
OBJ = $(SRC:%.c=$(O)/%.o)

BIN = tisemu

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	@echo " LD    $@"
	$(CC) $(OBJ) $(LDFLAGS) -o $@

$(O)/%.o: %.c | $(O)
	@echo " CC    $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(O):
	mkdir -p $(O)

clean:
	@echo " CLEAN $(O) and $(BIN)"
	rm -rf $(O) $(BIN)
