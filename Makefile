CC = gcc
CFLAGS = -lasound -lcurl -Wall -lX11 -lmpdclient
BUILD_DIR = build
BIN_DIR = .
SRC_DIR = src

# create build dir
$(shell mkdir -p $(BUILD_DIR))

# target: dependencies
# 	  action

dface: main.o blocks.o utils.o
	$(CC) $(BUILD_DIR)/main.o $(BUILD_DIR)/blocks.o $(BUILD_DIR)/utils.o $(CFLAGS) -o $(BIN_DIR)/dface

main.o: $(SRC_DIR)/main.c 
	$(CC) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

blocks.o: $(SRC_DIR)/blocks.c $(SRC_DIR)/blocks.h
	$(CC) -c $(SRC_DIR)/blocks.c -o $(BUILD_DIR)/blocks.o

utils.o: $(SRC_DIR)/utils.c $(SRC_DIR)/utils.h
	$(CC) -c $(SRC_DIR)/utils.c -o $(BUILD_DIR)/utils.o

clean:
	rm $(BUILD_DIR)/*.o

install:
	cp -f dface ~/bin/apps
	chmod +x ~/bin/apps/dface
