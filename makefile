CC = g++
CFLAGS = -std=c++17 -g -Wall -O3
GITVER = $(shell git rev-parse --abbrev-ref HEAD)
REVISION = $(shell git rev-parse HEAD)
FILES_TO_COMPILE = src\EntryPoint.cpp src\CPUZ80.cpp src\CPUInstructionHandlers.cpp src\Memory.cpp src\Cartridge.cpp src\MasterSystem.cpp src\VDP.cpp src\Utils.cpp

all:

	$(CC) $(CFLAGS) -I src\include -DCURRENT_BRANCH=$(GITVER) -DCURRENT_COMMIT=$(REVISION) $(FILES_TO_COMPILE) -o build\Mastalgia -O3 -D_hypot=hypot


verbose:
	$(CC) $(CFLAGS) -I src\include -DCURRENT_BRANCH=$(GITVER) -DCURRENT_COMMIT=$(REVISION) -DVERBOSE_MODE $(FILES_TO_COMPILE) -o build\Mastalgia -O3 -D_hypot=hypot
