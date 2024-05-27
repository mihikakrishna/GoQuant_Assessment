CC=g++
CFLAGS=-std=c++17 -Wall
LIBS=-lcurl -lcrypto++ -lssl -lcrypto

# Define the source files and object files
SOURCES=OKXTradingSystem.cpp CryptoUtilities.cpp
# SOURCES=main.cpp OKXTradingSystem.cpp CryptoUtilities.cpp NetworkUtilities.cpp TimeUtilities.cpp JsonUtilities.cpp
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
