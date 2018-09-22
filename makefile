CC = g++
CFLAGS = -std=c++17 -g -Wall -O3
GITVER = $(shell git rev-parse --abbrev-ref HEAD)
REVISION = $(shell git rev-parse HEAD)
FILES_TO_COMPILE = src\entryPoint.cpp src\cpuZ80.cpp src\cpuTester.cpp

all:

	$(CC) $(CFLAGS) -I src\include -DCURRENT_BRANCH=$(GITVER) -DCURRENT_COMMIT=$(REVISION) $(FILES_TO_COMPILE) -o build\Mastalgia -O3 -D_hypot=hypot


verbose:
	$(CC) $(CFLAGS) -I src\include -DCURRENT_BRANCH=$(GITVER) -DCURRENT_COMMIT=$(REVISION) -DVERBOSE_MODE $(FILES_TO_COMPILE) -o build\Mastalgia -O3 -D_hypot=hypot
