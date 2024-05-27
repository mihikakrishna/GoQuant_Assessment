#include "NetworkUtilities.h"
#include "CryptoUtilities.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string cleanHeader(const std::string& header) {
    std::string cleaned = header;
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\n'), cleaned.end());
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '\r'), cleaned.end());
    return cleaned;
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

std::string sendRequest(const std::string& url, const std::string& postData, const std::string& method, const std::string& api_key, const std::string& secret_key, const std::string& passphrase) {
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
        } else {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        }

        struct curl_slist *headers = NULL;
        if (!api_key.empty() && !secret_key.empty()) {
            std::string isoString = get_iso8601_timestamp();
            std::string requestPath = url.substr(url.find("/api/v5")); // Extract path from URL
            std::string preHash = isoString + method + requestPath + postData;
            std::string signature = hmac_sha256(preHash, secret_key);

            headers = curl_slist_append(headers, "accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "x-simulated-trading: 1");
            headers = curl_slist_append(headers, ("OK-ACCESS-KEY: " + cleanHeader(api_key)).c_str());
            headers = curl_slist_append(headers, ("OK-ACCESS-SIGN: " + cleanHeader(signature)).c_str());
            headers = curl_slist_append(headers, ("OK-ACCESS-TIMESTAMP: " + cleanHeader(isoString)).c_str());
            headers = curl_slist_append(headers, ("OK-ACCESS-PASSPHRASE: " + cleanHeader(passphrase)).c_str());
        }
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