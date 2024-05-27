#include "OKXTradingSystem.h"
#include <string>
#include <iostream>

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