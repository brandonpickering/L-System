
LIB=lib/liblsystem.a

SOURCES=$(wildcard source/*.cpp)
HEADERS=$(wildcard include/L-System/*)

CC=g++
CFLAGS=-m64 -std=c++11 -Wall -Wextra -Wpedantic -Iinclude

OBJECTS=$(SOURCES:source/%.cpp=build/%.o)


LDFLAGS=-static-libgcc -static-libstdc++


all: lib

clean:
	rm -rf $(OBJECTS)
	rm -rf lib/*.a
	rm -rf test/*.exe

build: lib

lib: $(LIB)

test: test/test.exe

$(LIB): $(OBJECTS)
	ar rvs $@ $^

build/%.o: source/%.cpp $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

test/test.exe: test/test.cpp $(LIB) $(HEADERS)
	$(CC) $(CFLAGS) $< $(LDFLAGS) -L$(dir $(LIB)) -llsystem -o test/test.exe
