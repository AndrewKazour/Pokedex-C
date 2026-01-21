# Makefile for Pokedex Server
# Works on Windows (with MinGW) and Linux/Mac

CC = gcc
CFLAGS = -Wall -Wextra -I./include

# Source files
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/file_io.c \
          $(SRC_DIR)/search.c \
          $(SRC_DIR)/progress.c \
          $(SRC_DIR)/json.c \
          $(SRC_DIR)/http_server.c

# Output
TARGET = pokedex_server

# Detect OS for linking
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    LDFLAGS = -lws2_32
    RM = del /Q
else
    LDFLAGS =
    RM = rm -f
endif

# Default target
all: $(TARGET)

# Link all object files
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Run the server
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	$(RM) $(TARGET)

# Rebuild
rebuild: clean all

.PHONY: all run clean rebuild
