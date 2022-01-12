// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#ifndef DUMPER_INCLUDE_COMMON_H_
#define DUMPER_INCLUDE_COMMON_H_

#include <filesystem>
#include <sstream>
#include <stdexcept>

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x4000
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
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

#endif // DUMPER_INCLUDE_COMMON_H_
