// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <filesystem>
#include <sstream>
#include <stdexcept>

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x4000
#endif

#define FATAL_ERROR(error_message)                                                                                                                        \
  {                                                                                                                                                       \
    std::stringstream compiled_msg;                                                                                                                       \
    compiled_msg << "Error: " << error_message << " at " << std::filesystem::path(__FILE__).filename() << ":" << __LINE__ << ":(" << __FUNCTION__ << ")"; \
    throw std::runtime_error(compiled_msg.str());                                                                                                         \
  };

#define DEBUG_LOG(log_message)                                                                                                                          \
  {                                                                                                                                                     \
    std::stringstream compiled_msg;                                                                                                                     \
    compiled_msg << "Debug: " << log_message << " at " << std::filesystem::path(__FILE__).filename() << ":" << __LINE__ << ":(" << __FUNCTION__ << ")"; \
    std::cout << compiled_msg.str() << std::endl;                                                                                                       \
  };

#endif // COMMON_HPP_
