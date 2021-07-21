// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef TESTING_HPP_
#define TESTING_HPP_

#include <regex>

// Checks the expression to throw a specific exception message matching a regex string
// If the test doesn't thow an exception the test fails and "pass" is printed
#define EXPECT_EXCEPTION_REGEX(expression, exception_type, exception_string, pass) \
  {                                                                                \
    try {                                                                          \
      expression;                                                                  \
      FAIL() << pass;                                                              \
    } catch (exception_type const &err) {                                          \
      if (!std::regex_match(err.what(), std::regex(exception_string))) {           \
        FAIL() << "Unexpected exception";                                          \
      }                                                                            \
    }                                                                              \
  }

#endif // TESTING_HPP_
