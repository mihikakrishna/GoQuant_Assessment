#include "OKXTradingSystem.h"
#include <curl/curl.h>
#include <iostream>

OKXTradingSystem::OKXTradingSystem(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
: api_key(api_key), secret_key(secret_key), passphrase(passphrase) {}

size_t OKXTradingSystem::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string OKXTradingSystem::sendRequest(const std::string& url, const std::string& postData, const std::string& method) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        }

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "Failed to initialize curl" << std::endl;
    }

    return readBuffer;
}


void OKXTradingSystem::placeOrder(const std::string& instId, const std::string& tdMode, const std::string& clOrdId,
                                  const std::string& side, const std::string& ordType, const std::string& px,
                                  const std::string& sz) {
    std::string url = "https://www.okx.com/api/v5/trade/order";
    std::string postData = "{\"instId\":\"" + instId + "\","
                           "\"tdMode\":\"" + tdMode + "\","
                           "\"clOrdId\":\"" + clOrdId + "\","
                           "\"side\":\"" + side + "\","
                           "\"ordType\":\"" + ordType + "\","
                           "\"px\":\"" + px + "\","
                           "\"sz\":\"" + sz + "\"}";
    std::string response = this->sendRequest(url, postData, "POST");
    // Add more code 
}


void OKXTradingSystem::cancelOrder(const std::string& orderID) {
    std::string url = "https://www.okx.com/api/v5/trade/cancel-order";
    std::string postData = "order_id=" + orderID;
    std::string response = this->sendRequest(url, postData, "POST");
    // Add more code 
}

void OKXTradingSystem::modifyOrder(const std::string& orderID, double newPrice) {
    std::string url = "https://www.okx.com/api/v5/trade/amend-order";
    std::string postData = "order_id=" + orderID + "&new_price=" + std::to_string(newPrice);
    std::string response = this->sendRequest(url, postData, "POST");
    // Add more code 
}

void OKXTradingSystem::getOrderBook(const std::string& symbol) {
    std::string url = "https://www.okx.com/api/v5/market/books?symbol=" + symbol;
    std::string response = this->sendRequest(url);
    // Add more code 
}

void OKXTradingSystem::getCurrentPositions(const std::string& symbol) {
    std::string url = "https://www.okx.com/api/v5/account/positions?symbol=" + symbol;
    std::string response = this->sendRequest(url);
    // Add more code 
}

int main() {
    OKXTradingSystem system("api_key", "secret_key", "passphrase");
    // Add more code 
    return 0;
}