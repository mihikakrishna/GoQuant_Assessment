#ifndef OKX_TRADING_SYSTEM_H
#define OKX_TRADING_SYSTEM_H

#include <string>
#include <vector>
#include <optional>

class OKXTradingSystem
{
public:
    OKXTradingSystem(const std::string &api_key, const std::string &secret_key, const std::string &passphrase);
    void placeOrder(const std::string &instId, const std::string &tdMode, const std::string &clOrdId,
                    const std::string &side, const std::string &ordType, const std::string &px,
                    const std::string &sz);
    void cancelOrder(const std::string &ordId, const std::string &instId);
    void modifyOrder(const std::string &orderID, double newSz, const std::string &instId);
    void getOrderBook(const std::string &instId);
    void getCurrentPositions(const std::optional<std::string> &instId, const std::optional<std::string> &instType = std::nullopt);

private:
    std::string api_key;
    std::string secret_key;
    std::string passphrase;
    std::string sign(const std::string &timestamp, const std::string &method, const std::string &requestPath, const std::string &body);
};

#endif // OKX_TRADING_SYSTEM_H
