#ifndef OKX_TRADING_SYSTEM_H
#define OKX_TRADING_SYSTEM_H

#include <string>

class OKXTradingSystem {
public:
    OKXTradingSystem(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);
    void placeOrder(const std::string& symbol, const std::string& type, double price, double amount);
    void cancelOrder(const std::string& orderID);
    void modifyOrder(const std::string& orderID, double newPrice);
    void getOrderBook(const std::string& symbol);
    void getCurrentPositions(const std::string& symbol);

private:
    std::string api_key;
    std::string secret_key;
    std::string passphrase;
    std::string sendRequest(const std::string& url, const std::string& postData = "", const std::string& method = "GET");
};

#endif // OKX_TRADING_SYSTEM_H
