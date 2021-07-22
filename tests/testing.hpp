// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef TESTING_HPP_
#define TESTING_HPP_

#include "common.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>

#if defined(__ORBIS__)
#include <libsha256.h>
#else
#include <openssl/sha.h>
#endif // __ORBIS__

// Checks the expression to throw a specific exception message matching a regex string
// If the test doesn't thow an exception the test fails and "pass" is printed
#define EXPECT_EXCEPTION_REGEX(expression, exception_string, pass)       \
  {                                                                      \
    try {                                                                \
      expression;                                                        \
      FAIL() << pass;                                                    \
    } catch (const std::exception &err) {                                \
      if (!std::regex_match(err.what(), std::regex(exception_string))) { \
        FAIL() << "Unexpected exception";                                \
      }                                                                  \
    }                                                                    \
  }

#if defined(__ORBIS__)
#define SHA256SUM(file_path, checksum)                                               \
  {                                                                                  \
    std::ifstream file_to_check(file_path, std::ios::in | std::ios::binary);         \
    if (!file_to_check || !file_to_check.good()) {                                   \
      file_to_check.close();                                                         \
      FAIL() << "Cannot open decrypted file: " << file_path;                         \
    }                                                                                \
    std::string checksum_string(checksum);                                           \
    unsigned char digest[SHA256_DIGEST_LENGTH];                                      \
    size_t count = checksum_string.size() / 2;                                       \
    for (size_t i = 0; count > i; i++) {                                             \
      uint32_t s = 0;                                                                \
      std::stringstream ss;                                                          \
      ss << std::hex << checksum_string.substr(i * 2, 2);                            \
      ss >> s;                                                                       \
      digest[i] = static_cast<unsigned char>(s);                                     \
    }                                                                                \
    unsigned char calculated_digest[SHA256_DIGEST_LENGTH];                           \
    SceSha256Context context;                                                        \
    sceSha256BlockInit(&context);                                                    \
    while (file_to_check.good()) {                                                   \
      unsigned char buffer[PAGE_SIZE];                                               \
      file_to_check.read((char *)buffer, sizeof(buffer)); /* Flawfinder: ignore */   \
      sceSha256BlockUpdate(&context, buffer, decrypted_elf.gcount());                \
    }                                                                                \
    file_to_check.close();                                                           \
    sceSha256BlockResult(calculated_digest, &context));                              \
    if (std::memcmp(calculated_digest, digest, sizeof(SHA256_DIGEST_LENGTH)) != 0) { \
      FAIL() << "Checksum does not match!";                                          \
    }                                                                                \
  }
#else
#define SHA256SUM(file_path, checksum)                                               \
  {                                                                                  \
    std::ifstream file_to_check(file_path, std::ios::in | std::ios::binary);         \
    if (!file_to_check || !file_to_check.good()) {                                   \
      file_to_check.close();                                                         \
      FAIL() << "Cannot open decrypted file: " << file_path;                         \
    }                                                                                \
    std::string checksum_string(checksum);                                           \
    unsigned char digest[SHA256_DIGEST_LENGTH];                                      \
    size_t count = checksum_string.size() / 2;                                       \
    for (size_t i = 0; count > i; i++) {                                             \
      uint32_t s = 0;                                                                \
      std::stringstream ss;                                                          \
      ss << std::hex << checksum_string.substr(i * 2, 2);                            \
      ss >> s;                                                                       \
      digest[i] = static_cast<unsigned char>(s);                                     \
    }                                                                                \
    unsigned char calculated_digest[SHA256_DIGEST_LENGTH];                           \
    SHA256_CTX context;                                                              \
    SHA256_Init(&context);                                                           \
    while (file_to_check.good()) {                                                   \
      unsigned char buffer[PAGE_SIZE];                                               \
      file_to_check.read((char *)buffer, sizeof(buffer)); /* Flawfinder: ignore */   \
      SHA256_Update(&context, buffer, file_to_check.gcount());                       \
    }                                                                                \
    file_to_check.close();                                                           \
    SHA256_Final(calculated_digest, &context);                                       \
    if (std::memcmp(calculated_digest, digest, sizeof(SHA256_DIGEST_LENGTH)) != 0) { \
      FAIL() << "Checksum does not match!";                                          \
    }                                                                                \
  }
#endif // __ORBIS__

#endif // TESTING_HPP_
