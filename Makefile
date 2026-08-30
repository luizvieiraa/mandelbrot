CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
THREAD_FLAGS = -fopenmp -pthread

TARGET = mandelbrot

SOURCES = src/main.c src/mandelbrot.c
HEADERS = src/mandelbrot.h

OUTPUTS = mandelbrot_lhcv_serial.pgm \
          mandelbrot_lhcv_openmp.pgm \
          mandelbrot_lhcv_pthreads1.pgm \
          mandelbrot_lhcv_pthreads2.pgm \
          times.txt \
          stdout.txt \
          stderr.txt

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(THREAD_FLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) 800 600 1000 4

clean:
	rm -f $(TARGET) $(OUTPUTS)

.PHONY: all run clean