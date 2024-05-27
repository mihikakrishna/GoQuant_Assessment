#include "OKXTradingSystem.h"
#include <iostream>
#include <string>
#include <optional>

void placeOrder(OKXTradingSystem &system);
void cancelOrder(OKXTradingSystem &system);
void modifyOrder(OKXTradingSystem &system);
void getOrderBook(OKXTradingSystem &system);
void getCurrentPositions(OKXTradingSystem &system);
int getUserChoice();
std::string getUserInput(const std::string &prompt);

int main()
{
    OKXTradingSystem system("be7ab88e-443b-43d0-a0c7-016d9a98c9cc", "E085D4AB76DF1F43833D328B52AEEDB3", "GoQuantIsAwesome100!");

    while (true)
    {
        std::cout << "\nChoose an operation:\n"
                  << "1. Place Order\n"
                  << "2. Cancel Order\n"
                  << "3. Modify Order\n"
                  << "4. Get Order Book\n"
                  << "5. Get Current Positions\n"
                  << "6. Exit\n"
                  << "Enter choice (1-6): ";
        int choice = getUserChoice();

        switch (choice)
        {
        case 1:
            placeOrder(system);
            break;
        case 2:
            cancelOrder(system);
            break;
        case 3:
            modifyOrder(system);
            break;
        case 4:
            getOrderBook(system);
            break;
        case 5:
            getCurrentPositions(system);
            break;
        case 6:
            std::cout << "Exiting program.\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please enter a number between 1 and 6.\n";
            break;
        }
    }
}

void placeOrder(OKXTradingSystem &system)
{
    std::string instId = getUserInput("Enter Instrument ID (e.g., BTC-USDT): ");
    std::string tdMode = getUserInput("Enter Trade Mode (e.g., cash): ");
    std::string ordId = getUserInput("Enter Client Order ID (unique): ");
    std::string side = getUserInput("Enter Side (buy/sell): ");
    std::string ordType = getUserInput("Enter Order Type (e.g., limit): ");
    std::string px = getUserInput("Enter Price: ");
    std::string sz = getUserInput("Enter Size: ");
    system.placeOrder(instId, tdMode, ordId, side, ordType, px, sz);
}

void cancelOrder(OKXTradingSystem &system)
{
    std::string instId = getUserInput("Enter Instrument ID: ");
    std::string ordId = getUserInput("Enter Order ID: ");
    system.cancelOrder(ordId, instId);
}

void modifyOrder(OKXTradingSystem &system)
{
    std::string ordId = getUserInput("Enter Order ID: ");
    std::string newPrice = getUserInput("Enter New Price: ");
    std::string instId = getUserInput("Enter Instrument ID (e.g., BTC-USDT): ");
    system.modifyOrder(ordId, std::stod(newPrice), instId);
}

void getOrderBook(OKXTradingSystem &system)
{
    std::string instId = getUserInput("Enter Instrument ID (e.g., BTC-USDT): ");
    system.getOrderBook(instId);
}

void getCurrentPositions(OKXTradingSystem &system)
{
    std::string instId = getUserInput("Enter Instrument ID (optional, e.g., BTC-USDT): ");
    std::string instType = getUserInput("Enter Instrument Type (optional, e.g., MARGIN, SWAP): ");
    system.getCurrentPositions(instId.empty() ? std::nullopt : std::make_optional(instId),
                               instType.empty() ? std::nullopt : std::make_optional(instType));
}

int getUserChoice()
{
    std::string input;
    std::getline(std::cin, input);
    try
    {
        return std::stoi(input);
    }
    catch (...)
    {
        return -1;
    }
}

std::string getUserInput(const std::string &prompt)
{
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}
