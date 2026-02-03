CC=gcc
CFLAGS=-Wall -Wextra -std=gnu99 -O2 -g
SRC=src/main.c src/util.c src/parser.c src/cpu.c src/memory.c src/syscall.c src/cache.c
OUT=mipsim

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)