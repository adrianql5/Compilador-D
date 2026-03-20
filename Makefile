CC = gcc
CFLAGS = -Wall
CPPFLAGS = -I.
SRC = $(shell find . -name "*.c")

compilador: $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC) -o compilador

clean:
	rm -f compilador
