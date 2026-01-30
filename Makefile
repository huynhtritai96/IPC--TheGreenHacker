CC = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build
DEPSRC = DLL/dll.c Mac-List/mac-list.c Routing-Table/routing-table.c Sync/sync.c
DEPOBJ = $(BUILD_DIR)/dll.o $(BUILD_DIR)/mac-list.o $(BUILD_DIR)/routing-table.o $(BUILD_DIR)/sync.o $(BUILD_DIR)/shm_ip.o

.PHONY: default client server dll mac-list routing-table sync shm_ip dll_test test clean

default: client server

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

client: $(BUILD_DIR) $(DEPOBJ)
	$(CC) $(CFLAGS) client.c $(DEPOBJ) -o $(BUILD_DIR)/client -lrt

server: $(BUILD_DIR) $(DEPOBJ)
	$(CC) $(CFLAGS) server.c $(DEPOBJ) -o $(BUILD_DIR)/server -lrt

dll_test: $(BUILD_DIR)/dll_test

test: dll_test
	./$(BUILD_DIR)/dll_test

dll: $(BUILD_DIR)
	$(CC) $(CFLAGS) -c DLL/dll.c -o $(BUILD_DIR)/dll.o
	# $(CC) $(CFLAGS) DLL/main.c DLL/dll.c -o $(BUILD_DIR)/DLL/main

mac-list: $(BUILD_DIR) dll shm_ip
	$(CC) $(CFLAGS) -c Mac-List/mac-list.c -o $(BUILD_DIR)/mac-list.o

routing-table: $(BUILD_DIR) dll
	$(CC) $(CFLAGS) -c Routing-Table/routing-table.c -o $(BUILD_DIR)/routing-table.o

sync: $(BUILD_DIR) dll mac-list routing-table shm_ip
	$(CC) $(CFLAGS) -c Sync/sync.c -o $(BUILD_DIR)/sync.o

shm_ip: $(BUILD_DIR)
	$(CC) $(CFLAGS) -c shm_ip.c -o $(BUILD_DIR)/shm_ip.o

$(BUILD_DIR)/dll.o: DLL/dll.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c DLL/dll.c -o $(BUILD_DIR)/dll.o

$(BUILD_DIR)/mac-list.o: Mac-List/mac-list.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c Mac-List/mac-list.c -o $(BUILD_DIR)/mac-list.o

$(BUILD_DIR)/routing-table.o: Routing-Table/routing-table.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c Routing-Table/routing-table.c -o $(BUILD_DIR)/routing-table.o

$(BUILD_DIR)/sync.o: Sync/sync.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c Sync/sync.c -o $(BUILD_DIR)/sync.o

$(BUILD_DIR)/shm_ip.o: shm_ip.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c shm_ip.c -o $(BUILD_DIR)/shm_ip.o

$(BUILD_DIR)/dll_test: DLL/main.c $(BUILD_DIR)/dll.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) DLL/main.c $(BUILD_DIR)/dll.o -o $(BUILD_DIR)/dll_test

clean:
	rm -rf $(BUILD_DIR)
