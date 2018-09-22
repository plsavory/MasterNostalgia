CC = g++
CFLAGS = -std=c++17 -g -Wall -O3
GITVER = $(shell git rev-parse --abbrev-ref HEAD)
REVISION = $(shell git rev-parse HEAD)

all:

	$(CC) $(CFLAGS) -DCURRENT_BRANCH=$(GITVER) -DCURRENT_COMMIT=$(REVISION) src\entryPoint.cpp -o build\Mastalgia -O3 -D_hypot=hypot
