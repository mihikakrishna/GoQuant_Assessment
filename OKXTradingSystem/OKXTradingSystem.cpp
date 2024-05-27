#include "OKXTradingSystem.h"
#include "CryptoUtilities.h"
#include "NetworkUtilities.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

OKXTradingSystem::OKXTradingSystem(const std::string& api_key, const std::string& secret_key, const std::string& passphrase)
: api_key(api_key), secret_key(secret_key), passphrase(passphrase) {}


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

    std::string response = sendRequest(url, postData, "POST", api_key, secret_key, passphrase);
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
    std::string response = sendRequest(url, postData, "POST", api_key, secret_key, passphrase);

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
    std::string response = sendRequest(url, postData, "POST", api_key, secret_key, passphrase);

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

void OKXTradingSystem::getOrderBook(const std::string& instId) {
    std::string url = "https://www.okx.com/api/v5/market/books?instId=" + instId;
    std::string response = sendRequest(url, "", "GET", api_key, secret_key, passphrase);

    if (!response.empty()) {
        try {
            auto jsonResponse = nlohmann::json::parse(response);
            if (jsonResponse.contains("code") && jsonResponse["code"] == "0") {
                auto orderBook = jsonResponse["data"];  // Assume 'data' holds the order book
                std::cout << "Order Book: " << orderBook.dump(4) << std::endl;
            } else {
                std::string errorMsg = jsonResponse.value("msg", "Unknown error");
                std::cerr << "Error retrieving order book: " << errorMsg << std::endl;
                throw std::runtime_error("Error retrieving order book: " + errorMsg);
            }
        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
            throw;
        }
    } else {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::getCurrentPositions(const std::optional<std::string>& instId, const std::optional<std::string>& instType) {    
    std::string url = "https://www.okx.com/api/v5/account/positions";
    
    bool queryStarted = false;

    // Add instId to the URL if it is provided
    if (instId.has_value()) {
        url += (queryStarted ? "&" : "?") + std::string("instId=") + instId.value();
        queryStarted = true;
    }
    
    // Add instType to the URL if it is provided
    if (instType.has_value()) {
        url += (queryStarted ? "&" : "?") + std::string("instType=") + instType.value();
        queryStarted = true;
    }

    std::string response = sendRequest(url, "", "GET", api_key, secret_key, passphrase);

    if (!response.empty()) {
        try {
            auto jsonResponse = nlohmann::json::parse(response);
            if (jsonResponse.contains("code") && jsonResponse["code"] == "0") {
                auto positions = jsonResponse["data"];  // Assume 'data' holds the positions
                std::cout << "Current Positions: " << positions.dump(4) << std::endl;
            } else {
                std::string errorMsg = jsonResponse.value("msg", "Unknown error");
                std::cerr << "Error retrieving positions: " << errorMsg << std::endl;
                throw std::runtime_error("Error retrieving positions: " + errorMsg);
            }
        } catch (nlohmann::json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
            throw;
        }
    } else {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

int main() {
    OKXTradingSystem system("be7ab88e-443b-43d0-a0c7-016d9a98c9cc", "E085D4AB76DF1F43833D328B52AEEDB3", "GoQuantIsAwesome100!");
    
    std::string input, instId, ordId, tdMode, side, ordType, px, sz, symbol, instType;
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
            case 1: {
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
            }
            case 2: {
                std::cout << "Enter Instrument ID: ";
                std::getline(std::cin, instId);
                std::cout << "Enter Order ID: ";
                std::getline(std::cin, ordId);
                system.cancelOrder(ordId, instId);
                break;
            }
            case 3: {
                std::cout << "Enter Order ID: ";
                std::getline(std::cin, ordId);
                std::cout << "Enter New Price: ";
                std::getline(std::cin, input);
                newPrice = std::stod(input);
                std::cout << "Enter Instrument ID (e.g., BTC-USDT): ";
                std::getline(std::cin, instId);
                system.modifyOrder(ordId, newPrice, instId);
                break;
            }
            case 4: {
                std::cout << "Enter Instrument ID (e.g., BTC-USDT): ";
                std::getline(std::cin, instId);
                system.getOrderBook(instId);
                break;
            }
            case 5: {
                std::cout << "Enter Instrument ID (optional, e.g., BTC-USDT): ";
                std::getline(std::cin, instId);
                std::optional<std::string> optInstId = instId.empty() ? std::nullopt : std::optional<std::string>(instId);
                
                std::cout << "Enter Instrument Type (optional, e.g., MARGIN, SWAP): ";
                std::getline(std::cin, instType);
                std::optional<std::string> optInstType = instType.empty() ? std::nullopt : std::optional<std::string>(instType);
                
                system.getCurrentPositions(optInstId, optInstType);
                break;
            }
            case 6: {
                std::cout << "Exiting program.\n";
                return 0;
            }
            default: {
                std::cout << "Invalid choice. Please enter a number between 1 and 6.\n";
                break;
            }
        }
    }
}