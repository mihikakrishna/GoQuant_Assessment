# OKXTradingSystem

## Description
This document explains how to compile and run the OKXTradingSystem application, which interacts with the OKX trading platform. The system uses the cURL library for HTTP communications, the nlohmann JSON library for JSON parsing, and Crypto++ and OpenSSL libraries for cryptographic operations.

## Prerequisites
Before compiling and running the OKXTradingSystem, ensure the following packages are installed:

### Dependencies
- **g++**: The GNU C++ Compiler
- **libcurl4-openssl-dev**: Provides development resources for libcurl.
- **nlohmann-json3-dev**: Provides the nlohmann JSON library for C++.
- **libcrypto++-dev**: Provides the Crypto++ library.
- **libssl-dev**: Provides the OpenSSL library.
```bash
sudo apt update
sudo apt install g++ libcurl4-openssl-dev nlohmann-json3-dev libcrypto++-dev libssl-dev
```

## Compiling

### Using g++
To compile the OKXTradingSystem manually, navigate to the directory containing the source files and run the following command:

```bash
g++ -std=c++17 -Wall -Iinclude -o bin/OKXTradingSystem $(find src -name '*.cpp') -Llib -lcurl -lcrypto++ -lssl -lcrypto
```

### Using Makefile
To compile the project using the Makefile, simply run:

```bash
make
```

## Running the Program
Once compiled, you can run the program by executing:

```bash
./bin/OKXTradingSystem
```
