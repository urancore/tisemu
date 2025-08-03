CC = gcc
CFLAGS = -Wall -g
NAME = tsm.exe
SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: $(NAME) run

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	.\$(NAME)

.PHONY: clean
clean:
	rm -f $(OBJS) $(NAME)
