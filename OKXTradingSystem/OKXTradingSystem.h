#ifndef OKX_TRADING_SYSTEM_H
#define OKX_TRADING_SYSTEM_H

#include <string>

class OKXTradingSystem {
public:
    OKXTradingSystem(const std::string& api_key, const std::string& secret_key, const std::string& passphrase);
    void placeOrder(const std::string& instId, const std::string& tdMode, const std::string& clOrdId,
                    const std::string& side, const std::string& ordType, const std::string& px,
                    const std::string& sz);
    void cancelOrder(const std::string& ordId, const std::string& instId);
    void modifyOrder(const std::string& orderID, double newSz, const std::string& instId);
    void getOrderBook(const std::string& symbol);
    void getCurrentPositions(const std::string& symbol);

private:
    std::string api_key;
    std::string secret_key;
    std::string passphrase;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    std::string sendRequest(const std::string& url, const std::string& postData = "", const std::string& method = "GET");
    std::string generateTimestamp();
    std::string sign(const std::string &timestamp, const std::string &method, const std::string &requestPath, const std::string &body);
};

#endif // OKX_TRADING_SYSTEM_H
