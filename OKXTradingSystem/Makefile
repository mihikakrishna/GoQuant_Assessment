CC=g++
CFLAGS=-std=c++17 -Wall -Iinclude
LIBS=-lcurl -lcrypto++ -lssl -lcrypto

# Define the source files and object files
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:src/%.cpp=obj/%.o)
EXECUTABLE=bin/OKXTradingSystem

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f obj/*.o bin/$(EXECUTABLE)

.PHONY: all clean
