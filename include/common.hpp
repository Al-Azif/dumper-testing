// Copyright (c) 2021 Al Azif
// License: GPLv3

#include <sstream>
#include <stdexcept>

#ifndef PAGE_SIZE
#define PAGE_SIZE 0x4000
#endif

#define fatal_error(error_message)                                                                                                \
  std::stringstream compiled_error;                                                                                               \
  compiled_error << "Error in \"" << __FILE__ << "\" on line " << __LINE__ << ", in " << __FUNCTION__ << "(): " << error_message; \
  throw std::runtime_error(compiled_error.str());
