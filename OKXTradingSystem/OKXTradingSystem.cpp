#include <iostream>
#include <string>
#include <stdio.h>
#include <./curl/curl.h>

#include "OKXTradingSystem.h"

OKXTradingSystem::OKXTradingSystem(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
: api_key(api_key), secret_key(secret_key), passphrase(passphrase) {}

std::string sendRequest(const std::string& url, const std::string& postData = "", const std::string& method = "GET"){
    CURL* curl = curl_easy_init();
    //curl_global_init(CURL_GLOBAL_ALL);
    std::string res = "";
    return res;
}

void OKXTradingSystem::placeOrder(const std::string& symbol, const std::string& type, double price, double amount) {
    std::string url = "https://www.okx.com/api/v5/trade/order";
    std::string postData = "symbol=" + symbol + "&type=" + type + "&price=" + std::to_string(price) + "&amount=" + std::to_string(amount);
    std::string response = this->sendRequest(url, postData, "POST");
    // Additional code to handle the response
}

void OKXTradingSystem::cancelOrder(const std::string& orderID) {
    std::string url = "https://www.okx.com/api/v5/trade/cancel-order";
    std::string postData = "order_id=" + orderID;
    std::string response = this->sendRequest(url, postData, "POST");
    // Additional code to handle the response
}

void OKXTradingSystem::modifyOrder(const std::string& orderID, double newPrice) {
    std::string url = "https://www.okx.com/api/v5/trade/amend-order";
    std::string postData = "order_id=" + orderID + "&new_price=" + std::to_string(newPrice);
    std::string response = this->sendRequest(url, postData, "POST");
    // Additional code to handle the response
}

void OKXTradingSystem::getOrderBook(const std::string& symbol) {
    std::string url = "https://www.okx.com/api/v5/market/books?symbol=" + symbol;
    std::string response = this->sendRequest(url);
    // Additional code to handle the response
}

void OKXTradingSystem::getCurrentPositions(const std::string& symbol) {
    std::string url = "https://www.okx.com/api/v5/account/positions?symbol=" + symbol;
    std::string response = this->sendRequest(url);
    // Additional code to handle the response
}

// Definition of sendRequest method should be added here, correctly handling all requests
std::string OKXTradingSystem::sendRequest(const std::string& url, const std::string& postData, const std::string& method) {
    return 0;
}


int main() {
    OKXTradingSystem system("api_key", "secret_key", "passphrase");
    // Add more code to use your system as needed
    return 0;
}