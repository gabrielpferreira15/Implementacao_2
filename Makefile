CC = gcc-16
CFLAGS = -Wall -Wextra -std=c17 -fopenmp -pthread

TARGET = mandelbrot
OBJ = main.o mandelbrot.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

main.o: main.c mandelbrot.h
	$(CC) $(CFLAGS) -c main.c

mandelbrot.o: mandelbrot.c mandelbrot.h
	$(CC) $(CFLAGS) -c mandelbrot.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: run clean