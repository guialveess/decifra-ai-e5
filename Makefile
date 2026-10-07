CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude -D_GNU_SOURCE -MMD -MP
TARGET  = jogo
SOURCES = src/main.c src/game.c src/ui.c src/input.c src/ai_client.c
OBJECTS = $(SOURCES:.c=.o)
DEPS    = $(OBJECTS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Compilacao concluida!"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

run: all
	./$(TARGET)

clean:
	rm -f src/*.o src/*.d $(TARGET)
	@echo "Limpeza concluida!"