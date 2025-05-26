# Makefile for Donna Contact Manager

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -g
TARGET  := donna
SRCS    := main.c contact.c
OBJS    := $(SRCS:.c=.o)
DEPS    := contact.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	rm -f $(OBJS)

# Compile .c to .o, rebuild if header changes
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
