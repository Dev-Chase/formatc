CC=clang
CFLAGS=-g -Wall -Iinclude
RELEASE_FLAGS=-Wall -o2 -Iinclude
EXE=formatc
MAIN=main
OBJ=obj
BIN=bin
SRC=src
INCLUDE=include
FILE=src/test.c
OPTS=space2
OBJS=$(OBJ)/$(MAIN).o $(OBJ)/format.o

all: $(BIN)/$(EXE)

run: all
	$(BIN)/$(EXE) $(OPTS) $(FILE)

release:
	$(CC) $(RELEASE_FLAGS) -o $(BIN)/$(EXE) $(SRC)/*.c

$(OBJ)/$(MAIN).o: $(SRC)/$(MAIN).c
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDE)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf *.dSYM $(OBJ)/* $(BIN)/*
