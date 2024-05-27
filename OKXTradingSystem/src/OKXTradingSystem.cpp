#include "OKXTradingSystem.h"
#include "CryptoUtilities.h"
#include "NetworkUtilities.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

OKXTradingSystem::OKXTradingSystem(const std::string &api_key, const std::string &secret_key, const std::string &passphrase)
    : api_key(api_key), secret_key(secret_key), passphrase(passphrase) {}

nlohmann::json OKXTradingSystem::parseResponse(const std::string &response)
{
    return nlohmann::json::parse(response);
}

void OKXTradingSystem::handlePlaceOrderResponse(const nlohmann::json &jsonResponse)
{
    if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty())
    {
        auto firstElement = jsonResponse["data"][0];
        std::string statusCode = firstElement.value("sCode", "");
        std::cout << "Response StatusCode: " << statusCode << std::endl;
        if (statusCode != "0")
        {
            std::string errorMessage = firstElement.value("sMsg", "Unknown error");
            std::cerr << "Error placing order: " << errorMessage << std::endl;
            throw std::runtime_error("Error placing order: " + errorMessage);
        }
        else
        {
            std::string orderId = firstElement.value("ordId", "No Order ID");
            std::cout << "Order placed successfully, Order ID: " << orderId << std::endl;
        }
    }
    else
    {
        std::cerr << "Response JSON does not contain expected 'data' array or is empty." << std::endl;
        throw std::runtime_error("Invalid JSON response structure.");
    }
}

void OKXTradingSystem::placeOrder(const std::string &instId, const std::string &tdMode, const std::string &clOrdId,
                                  const std::string &side, const std::string &ordType, const std::string &px, const std::string &sz)
{
    std::string url = "https://www.okx.com/api/v5/trade/order";
    nlohmann::json orderDetails = {
        {"instId", instId},
        {"tdMode", tdMode},
        {"side", side},
        {"ordType", ordType},
        {"px", px},
        {"sz", sz}};
    std::string postData = orderDetails.dump();
    std::string response = sendRequest(url, postData, "POST", api_key, secret_key, passphrase);

    if (!response.empty())
    {
        try
        {
            auto jsonResponse = parseResponse(response);
            handlePlaceOrderResponse(jsonResponse);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            throw;
        }
    }
    else
    {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::cancelOrder(const std::string &ordId, const std::string &instId)
{
    std::string url = "https://www.okx.com/api/v5/trade/cancel-order";
    nlohmann::json orderDetails = {
        {"ordId", ordId},
        {"instId", instId}};
    std::string postData = orderDetails.dump();
    std::string response = sendRequest(url, postData, "POST", api_key, secret_key, passphrase);

    if (!response.empty())
    {
        try
        {
            auto jsonResponse = parseResponse(response);
            handleCancellationResponse(jsonResponse);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            throw;
        }
    }
    else
    {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::handleCancellationResponse(const nlohmann::json &jsonResponse)
{
    if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty())
    {
        auto firstElement = jsonResponse["data"][0];
        std::string statusCode = firstElement.value("sCode", "");
        if (statusCode != "0")
        {
            std::string errorMessage = firstElement.value("sMsg", "Unknown error");
            std::cerr << "Error cancelling order: " << errorMessage << std::endl;
            throw std::runtime_error("Error cancelling order: " + errorMessage);
        }
        else
        {
            std::cout << "Order cancelled successfully." << std::endl;
        }
    }
    else
    {
        std::string errorMsg = jsonResponse.value("msg", "Unexpected response structure.");
        std::cerr << "Error cancelling order: " << errorMsg << std::endl;
        throw std::runtime_error("Error cancelling order: " + errorMsg);
    }
}

void OKXTradingSystem::modifyOrder(const std::string &ordId, double newSz, const std::string &instId)
{
    std::string url = "https://www.okx.com/api/v5/trade/amend-order";
    nlohmann::json orderDetails = {
        {"ordId", ordId},
        {"newSz", std::to_string(newSz)},
        {"instId", instId}};
    std::string postData = orderDetails.dump();
    std::string response = sendRequest(url, postData, "POST", api_key, secret_key, passphrase);

    if (!response.empty())
    {
        try
        {
            auto jsonResponse = parseResponse(response);
            handleModificationResponse(jsonResponse);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            throw;
        }
    }
    else
    {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::handleModificationResponse(const nlohmann::json &jsonResponse)
{
    if (jsonResponse.contains("data") && jsonResponse["data"].is_array() && !jsonResponse["data"].empty())
    {
        auto firstElement = jsonResponse["data"][0];
        std::string statusCode = firstElement.value("sCode", "");
        if (statusCode != "0")
        {
            std::string errorMessage = firstElement.value("sMsg", "Unknown error");
            std::cerr << "Error modifying order: " << errorMessage << std::endl;
            throw std::runtime_error("Error modifying order: " + errorMessage);
        }
        else
        {
            std::cout << "Order modified successfully." << std::endl;
        }
    }
    else
    {
        std::string errorMsg = jsonResponse.value("msg", "Unexpected response structure.");
        std::cerr << "Error modifying order: " << errorMsg << std::endl;
        throw std::runtime_error("Error modifying order: " + errorMsg);
    }
}

void OKXTradingSystem::getOrderBook(const std::string &instId)
{
    std::string url = "https://www.okx.com/api/v5/market/books?instId=" + instId;
    std::string response = sendRequest(url, "", "GET", api_key, secret_key, passphrase);

    if (!response.empty())
    {
        try
        {
            auto jsonResponse = parseResponse(response);
            if (jsonResponse["code"] == "0")
            {
                displayOrderBook(jsonResponse);
            }
            else
            {
                std::string errorMsg = jsonResponse.value("msg", "Unknown error");
                std::cerr << "Error retrieving order book: " << errorMsg << std::endl;
                throw std::runtime_error("Error retrieving order book: " + errorMsg);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            throw;
        }
    }
    else
    {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::displayOrderBook(const nlohmann::json &jsonResponse)
{
    auto orderBook = jsonResponse["data"];
    std::cout << "Order Book: " << orderBook.dump(4) << std::endl;
}

void OKXTradingSystem::getCurrentPositions(const std::optional<std::string> &instId, const std::optional<std::string> &instType)
{
    std::string url = "https://www.okx.com/api/v5/account/positions";
    bool isFirstParam = true;

    if (instId)
    {
        url += std::string(isFirstParam ? "?" : "&") + "instId=" + instId.value();
        isFirstParam = false;
    }

    if (instType)
    {
        url += std::string(isFirstParam ? "?" : "&") + "instType=" + instType.value();
    }

    std::string response = sendRequest(url, "", "GET", api_key, secret_key, passphrase);

    if (!response.empty())
    {
        try
        {
            auto jsonResponse = parseResponse(response);
            if (jsonResponse["code"] == "0")
            {
                displayCurrentPositions(jsonResponse);
            }
            else
            {
                std::string errorMsg = jsonResponse.value("msg", "Unknown error");
                std::cerr << "Error retrieving positions: " << errorMsg << std::endl;
                throw std::runtime_error("Error retrieving positions: " + errorMsg);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << '\n';
            throw;
        }
    }
    else
    {
        std::cerr << "Received empty response from server." << std::endl;
        throw std::runtime_error("Empty response from server.");
    }
}

void OKXTradingSystem::displayCurrentPositions(const nlohmann::json &jsonResponse)
{
    auto positions = jsonResponse["data"];
    std::cout << "Current Positions: " << positions.dump(4) << std::endl;
}