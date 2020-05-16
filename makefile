 # the compiler: gcc for C program.
CC = gcc

# compiler flags:
CFLAGS = -std=gnu11 -o
LDFLAGS = -lncurses

# the build target executable:
TARGET = /usr/bin/rain
# for development
# TARGET = ./rain

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) $(TARGET) main.c $(LDFLAGS)

clean:
	$(RM) $(TARGET)