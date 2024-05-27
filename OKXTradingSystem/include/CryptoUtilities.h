// CryptoUtilities.h
#pragma once

#include <string>

std::string encodeBase64(const unsigned char *buffer, size_t length);
std::string hmac_sha256(const std::string &data, const std::string &key);