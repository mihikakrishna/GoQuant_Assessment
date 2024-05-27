// NetworkUtilities.h
#pragma once

#include <curl/curl.h>
#include <string>

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
std::string cleanHeader(const std::string &header);
std::string get_iso8601_timestamp();
std::string sendRequest(const std::string &url, const std::string &postData = "", const std::string &method = "GET", const std::string &api_key = "", const std::string &secret_key = "", const std::string &passphrase = "");