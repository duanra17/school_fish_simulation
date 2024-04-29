CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g
LDFLAGS= fonctions.c graph.c -lSDL2 -lm -lSDL2_image
SRC=main.c
EXECUTABLE=main.x

all: $(EXECUTABLE)

$(EXECUTABLE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

debug: $(EXECUTABLE)
	gdb ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
