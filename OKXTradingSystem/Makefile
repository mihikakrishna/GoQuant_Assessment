CC=g++
CFLAGS=-std=c++17 -Wall
LIBS=-lcurl -lcrypto++ -lssl -lcrypto

# Define the source files and object files
SOURCES=main.cpp OKXTradingSystem.cpp CryptoUtilities.cpp NetworkUtilities.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=OKXTradingSystem

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
