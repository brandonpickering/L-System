
SOURCES=$(wildcard source/*.cpp)
HEADERS=$(wildcard include/L-System/*)

CC=g++
CFLAGS=-m64 -std=c++11 -Wall -Wextra -Wpedantic -Iinclude

OBJECTS=$(SOURCES:source/%.cpp=build/%.o)


LDFLAGS=-static-libgcc -static-libstdc++


all: build

clean:
	rm -rf $(OBJECTS)
	rm -rf lib/*.a
	rm -rf test/*.exe

build: lib test/test.exe

lib: lib/libl_system.a

lib/libl_system.a: $(OBJECTS)
	ar rvs lib/libl_system.a $^

build/%.o: source/%.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

test/test.exe: test/test.cpp lib $(HEADERS)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -L./lib -ll_system -o test/test.exe

test: test/test.exe
	test/test.exe
