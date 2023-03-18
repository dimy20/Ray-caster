EXEC = main
CC = gcc

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIRS = include

CFLAGS = -Werror -Wall -g -std=c11 $(foreach D, $(INCLUDE_DIRS), -I$(D))

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

$(EXEC): $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm $(BUILD_DIR)/%.o $(EXEC)
