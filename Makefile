CC := gcc
CFLAGS := -Wall -g -lSDL2 -lSDL2_ttf -lm
APP_NAME := mrrec

BUILD_DIR := build

SRCS := $(shell find -name '*.c')
OBJS := $(SRCS:%.c=$(BUILD_DIR)/%.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD_DIR)/$(APP_NAME)

$(BUILD_DIR)/%.o : %.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -r $(BUILD_DIR)
