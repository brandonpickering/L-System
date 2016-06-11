
SOURCES=$(wildcard *.cpp)
HEADERS=$(wildcard *.hpp)


build: $(SOURCES) $(HEADERS)
	g++ -m64 -std=c++11 -Wall -Wextra -Wpedantic $(SOURCES) -o out.exe

run: build
	./out.exe
