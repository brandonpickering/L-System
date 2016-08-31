
LIB=lib/liblsystem.so

SOURCES=$(wildcard source/*.cpp)
HEADERS=$(wildcard include/L-System/*)

CC=g++
CFLAGS=-m64 -std=c++11 -Wall -Wextra -Wpedantic -Iinclude

OBJECTS=$(SOURCES:source/%.cpp=build/%.o)


LDFLAGS=-static-libgcc -static-libstdc++

TEST_EXEC=test/test


all: lib

clean:
	rm -rf $(OBJECTS)
	rm -rf lib/*
	rm -f test/test test/test.exe

build: lib

lib: $(LIB)

test: $(TEST_EXEC)

$(LIB): $(OBJECTS)
	ar rvs $@ $^

build/%.o: source/%.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

$(TEST_EXEC): test/test.cpp $(LIB) $(HEADERS)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -L$(dir $(LIB)) -llsystem -o $(TEST_EXEC)
