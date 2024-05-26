# OKXTradingSystem

## Description
This document explains how to compile and run the OKXTradingSystem application, which requires the cURL library for HTTP communications.

## Prerequisites
Before compiling and running the OKXTradingSystem, ensure the following packages are installed:

### Dependencies
- **g++**: The GNU C++ Compiler
- **libcurl4-openssl-dev**: Provides development resources for libcurl.
- **nlohmann-json3-dev**: Provides the nlohmann JSON library for C++.

```bash
sudo apt update
sudo apt install g++ libcurl4-openssl-dev
```

## Compiling
To compile the OKXTradingSystem, navigate to the directory containing `OKXTradingSystem.cpp` and run the following command:

```bash
g++ -o OKXTradingSystem OKXTradingSystem.cpp -lcurl
```

## Running the Program
Once compiled, you can run the program by executing:

```bash
./OKXTradingSystem
```
