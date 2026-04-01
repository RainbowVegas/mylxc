CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = mylxc
SRC_DIR = src
OBJS = $(SRC_DIR)/main.o $(SRC_DIR)/container.o $(SRC_DIR)/util.o $(SRC_DIR)/image.o $(SRC_DIR)/network.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

rebuild: clean all

.PHONY: all clean install rebuild
