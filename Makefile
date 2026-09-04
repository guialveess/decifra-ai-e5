CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude
TARGET = jogo
SOURCES = src/main.c src/game.c src/ui.c src/input.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilacao concluida!"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f src/*.o $(TARGET)
	@echo "Limpeza concluida!"

.PHONY: all run clean
