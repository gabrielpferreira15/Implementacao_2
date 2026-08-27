CC = gcc
CFLAGS = -Wall -Wextra -std=c17

TARGET = madelbrot
OBJ = main.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: run clean