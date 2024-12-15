PROJECT := arfts
BINARY := arfts
CC := gcc
CFLAGS := -Wall -pedantic -g -O0
LDFLAGS := 

SRC := $(wildcard *.c)
HDR := $(wildcard *.h)
OBJ := $(patsubst %.c,%.o,$(SRC))

all: $(BINARY)

$(BINARY): $(OBJ)
	$(CC) -o $(BINARY) $(OBJ) $(LDFLAGS)

%.o: %.c $(HDR)
	$(CC) -c $(CCFLAGS) $<

.PHONY: clean
clean:
	rm $(BINARY) *.o

