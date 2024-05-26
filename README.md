# OKXTradingSystem

## Description
This document explains how to compile and run the OKXTradingSystem application, which requires the cURL library for HTTP communications.

## Prerequisites
Before compiling and running the OKXTradingSystem, ensure you have `g++` and `libcurl4-openssl-dev` installed on your system. If not, you can install them using the following commands:

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
