#include "OKXTradingSystem.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <crypto++/sha.h>
#include <crypto++/hmac.h>
#include <crypto++/base64.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

OKXTradingSystem::OKXTradingSystem(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
: api_key(api_key), secret_key(secret_key), passphrase(passphrase) {}

size_t OKXTradingSystem::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string encodeBase64(const unsigned char* buffer, size_t length) {
    BIO* bio, * b64;
    BUF_MEM* bufferPtr;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string output(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);
    return output;
}

std::string hmac_sha256(const std::string &data, const std::string &key) {
    unsigned char* hash = HMAC(EVP_sha256(), key.c_str(), key.length(), (const unsigned char*)data.c_str(), data.length(), NULL, NULL);
    return encodeBase64(hash, 32); // 32 bytes = 256 bits
}

std::string get_iso8601_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
    auto msPart = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream dateTimeStream;
    dateTimeStream << std::put_time(gmtime(&nowAsTimeT), "%Y-%m-%dT%H:%M:%S");
    dateTimeStream << '.' << std::setfill('0') << std::setw(3) << msPart.count() << 'Z';
    return dateTimeStream.str();
}

std::string cleanHeader(const std::string& header) {
    std::string cleaned = header;
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\n'), cleaned.end());
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\r'), cleaned.end());
    return cleaned;
}

std::string OKXTradingSystem::sendRequest(const std::string& url, const std::string& postData, const std::string& method) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    std::string isoString = get_iso8601_timestamp();
    std::string requestPath = url.substr(url.find("/api/v5")); // Extract path from URL
    std::string preHash = isoString + method + requestPath + postData;
    std::string signature = hmac_sha256(preHash, secret_key);

    std::cout << "Signature: " << signature << std::endl;
    std::cout << "Generated Timestamp: " << isoString << std::endl;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        } else if (method == "GET") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        }

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "x-simulated-trading: 1");
        headers = curl_slist_append(headers, ("OK-ACCESS-KEY: " + cleanHeader(api_key)).c_str());
        headers = curl_slist_append(headers, ("OK-ACCESS-SIGN: " + cleanHeader(signature)).c_str());
        headers = curl_slist_append(headers, ("OK-ACCESS-TIMESTAMP: " + cleanHeader(isoString)).c_str());
        headers = curl_slist_append(headers, ("OK-ACCESS-PASSPHRASE: " + cleanHeader(passphrase)).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);

        struct curl_slist *temp = headers;
        while (temp != NULL) {
            std::cout << "Final Header: " << temp->data << std::endl;
            temp = temp->next;
        }


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
                           "\"side\":\"" + side + "\","
                           "\"ordType\":\"" + ordType + "\","
                           "\"px\":\"" + px + "\","
                           "\"sz\":\"" + sz + "\"}";

    std::string response = this->sendRequest(url, postData, "POST");
    // std::cout << "Response received: " << response << std::endl;

    if (!response.empty()) {
        try {
            auto jsonResponse = nlohmann::json::parse(response);
            // Access the first element of the data array
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty()) {
                auto firstElement = jsonResponse["data"][0];
                std::string sCode = firstElement.value("sCode", "");
                std::cout << "Response sCode: " << sCode << std::endl;
                if (sCode != "0") {
                    std::string sMsg = firstElement.value("sMsg", "Unknown error");
                    std::cerr << "Error placing order: " << sMsg << std::endl;
                    throw std::runtime_error("Error placing order: " + sMsg);
                } else {
                    std::string ordId = firstElement.value("ordId", "No Order ID");
                    std::cout << "Order placed successfully, Order ID: " << ordId << std::endl;
                }
            } else {
                std::cerr << "Response JSON does not contain expected 'data' array or is empty." << std::endl;
                throw std::runtime_error("Invalid JSON response structure.");
            }
        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
            std::cerr << "Raw response: " << response << '\n';
            throw;
        }
    } else {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}



void OKXTradingSystem::cancelOrder(const std::string& ordId, const std::string& instId) {
    std::string url = "https://www.okx.com/api/v5/trade/cancel-order";
    std::string postData = "{\"ordId\":\"" + ordId + "\","
                            "\"instId\":\"" + instId + "\"}";
    std::string response = this->sendRequest(url, postData, "POST");

    if (!response.empty()) {
        try {
            auto jsonResponse = nlohmann::json::parse(response);

            if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty()) {
                auto firstElement = jsonResponse["data"][0];
                std::string sCode = firstElement.value("sCode", "");

                if (sCode != "0") {
                    std::string sMsg = firstElement.value("sMsg", "Unknown error");
                    std::cerr << "Error cancelling order: " << sMsg << std::endl;
                    throw std::runtime_error("Error cancelling order: " + sMsg);
                } else {
                    std::cout << "Order cancelled successfully." << std::endl;
                }
            } else {
                std::string errorMsg = jsonResponse.value("msg", "");
                if (!errorMsg.empty()) {
                    std::cerr << "Error cancelling order: " << errorMsg << std::endl;
                    throw std::runtime_error("Error cancelling order: " + errorMsg);
                } else {
                    std::cerr << "Unexpected response structure." << std::endl;
                    throw std::runtime_error("Unexpected response structure.");
                }
            }
        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
            std::cerr << "Raw response: " << response << '\n';
            throw;
        }
    } else {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::modifyOrder(const std::string& ordId, double newSz, const std::string& instId) {
    std::string url = "https://www.okx.com/api/v5/trade/amend-order";
    std::string postData = "{\"ordId\":\"" + ordId + "\","
                            "\"newSz\":\"" + std::to_string(newSz) + "\","
                            "\"instId\":\"" + instId + "\"}";
    std::string response = this->sendRequest(url, postData, "POST");

    if (!response.empty()) {
        try {
            auto jsonResponse = nlohmann::json::parse(response);

            // Check for the existence of a 'data' array and that it is not empty
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty()) {
                auto firstElement = jsonResponse["data"][0];
                std::string sCode = firstElement.value("sCode", "");

                if (sCode != "0") {
                    std::string sMsg = firstElement.value("sMsg", "Unknown error");
                    std::cerr << "Error modifying order: " << sMsg << std::endl;
                    throw std::runtime_error("Error modifying order: " + sMsg);
                } else {
                    std::cout << "Order modified successfully." << std::endl;
                }
            } else {
                // Check for a global error message if no 'data' array is present
                std::string errorMsg = jsonResponse.value("msg", "");
                if (!errorMsg.empty()) {
                    std::cerr << "Error modifying order: " << errorMsg << std::endl;
                    throw std::runtime_error("Error modifying order: " + errorMsg);
                } else {
                    std::cerr << "Unexpected response structure." << std::endl;
                    throw std::runtime_error("Unexpected response structure.");
                }
            }
        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
            std::cerr << "Raw response: " << response << '\n';
            throw;
        }
    } else {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::getOrderBook(const std::string& sprdId) {
    std::string url = "https://www.okx.com/api/v5/market/books?symbol=" + sprdId;
    std::string response = this->sendRequest(url);
    // Add more code 
}

void OKXTradingSystem::getCurrentPositions(const std::string& symbol) {
    std::string url = "https://www.okx.com/api/v5/account/positions?symbol=" + symbol;
    std::string response = this->sendRequest(url);
    // Add more code 
}

int main() {
    OKXTradingSystem system("be7ab88e-443b-43d0-a0c7-016d9a98c9cc", "E085D4AB76DF1F43833D328B52AEEDB3", "GoQuantIsAwesome100!");
    
    std::string input, instId, ordId, tdMode, side, ordType, px, sz, sprdId, symbol;
    double newPrice;
    int choice;

    while (true) {
        std::cout << "\nChoose an operation:\n"
                  << "1. Place Order\n"
                  << "2. Cancel Order\n"
                  << "3. Modify Order\n"
                  << "4. Get Order Book\n"
                  << "5. Get Current Positions\n"
                  << "6. Exit\n"
                  << "Enter choice (1-6): ";
        std::getline(std::cin, input);
        choice = std::stoi(input);

        switch (choice) {
            case 1:
                std::cout << "Enter Instrument ID (e.g., BTC-USDT): ";
                std::getline(std::cin, instId);
                std::cout << "Enter Trade Mode (e.g., cash): ";
                std::getline(std::cin, tdMode);
                std::cout << "Enter Client Order ID (unique): ";
                std::getline(std::cin, ordId);
                std::cout << "Enter Side (buy/sell): ";
                std::getline(std::cin, side);
                std::cout << "Enter Order Type (e.g., limit): ";
                std::getline(std::cin, ordType);
                std::cout << "Enter Price: ";
                std::getline(std::cin, px);
                std::cout << "Enter Size: ";
                std::getline(std::cin, sz);
                system.placeOrder(instId, tdMode, ordId, side, ordType, px, sz);
                break;
            case 2:
                std::cout << "Enter Instrument ID: ";
                std::getline(std::cin, instId);
                std::cout << "Enter Order ID: ";
                std::getline(std::cin, ordId);
                system.cancelOrder(ordId, instId);
                break;
            case 3:
                std::cout << "Enter Order ID: ";
                std::getline(std::cin, ordId);
                std::cout << "Enter New Price: ";
                std::getline(std::cin, input);
                newPrice = std::stod(input);
                std::cout << "Enter Instrument ID (e.g., BTC-USDT): ";
                std::getline(std::cin, instId);
                system.modifyOrder(ordId, newPrice, instId);
                break;
            case 4:
                std::cout << "Enter Spread ID: ";
                std::getline(std::cin, sprdId);
                system.getOrderBook(sprdId);
                break;
            case 5:
                std::cout << "Enter Symbol: ";
                std::getline(std::cin, symbol);
                system.getCurrentPositions(symbol);
                break;
            case 6:
                std::cout << "Exiting program.\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please enter a number between 1 and 6.\n";
        }
    }

    return 0;
}