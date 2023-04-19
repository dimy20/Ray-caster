EXEC = main
CC = g++

SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIRS = include

CFLAGS = -Werror -Wall -g -std=c++17 -O2 $(foreach D, $(INCLUDE_DIRS), -I$(D))
LDFLAGS = -lSDL2 -lSDL2_image -lm

SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

SRCS := $(filter-out $(SRC_DIR)/memory.cpp, $(SRCS))

$(EXEC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm $(BUILD_DIR)/%.o $(EXEC)
