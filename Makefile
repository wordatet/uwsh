# Makefile for sh (Linux AMD64 Hybrid Port)
# Supports GCC for stability and PCC for historical auditing.

CC = gcc
CFLAGS = -g -O0 -I./include -D_GNU_SOURCE -DACCT \
	-fno-stack-protector -fno-builtin-malloc
LDFLAGS = 
LDLIBS = -L. -lshim

SOURCES = args.c blok.c bltin.c cmd.c ctype.c defs.c echo.c error.c expand.c \
	fault.c func.c hash.c hashserv.c io.c jobs.c macro.c main.c msg.c \
	name.c print.c pwd.c service.c setbrk.c stak.c string.c test.c \
	ulimit.c umask.c word.c xec.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean vintage paranoid

# Default: GCC build (Stability & 64-bit safety)
all: sh

sh: $(OBJECTS) libshim.a
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

# Vintage: PCC build (Historical K&R Accuracy)
vintage: clean
	$(MAKE) CC=pcc CFLAGS="-g -O0 -I./include"

# Verification: Run the Super Paranoid suite
paranoid: clean sh
	./super_paranoid.sh

# Re-link libshim
libshim.a: shim.o
	ar rcs libshim.a shim.o

shim.o: shim.c
	$(CC) $(CFLAGS) -c shim.c

clean:
	rm -f $(OBJECTS) sh libshim.a shim.o
