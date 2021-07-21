// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef NPBIND_TEST_HPP_
#define NPBIND_TEST_HPP_

#include <gtest/gtest.h>

#include "testing.hpp"

#include "npbind.hpp"

TEST(npbindTest, read) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(npbind::read(""), std::runtime_error, "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(npbind::read(" "), std::runtime_error, "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(npbind::read("  "), std::runtime_error, "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(npbind::read("\t"), std::runtime_error, "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(npbind::read("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(npbind::read(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                           // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/npbind/noPermission\\.ext at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Could \"open\" file without permissions");

  // File is too small for a npbind.dat
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindHeaderSize.dat"), std::runtime_error, "^Error: Error reading header! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Could not read header");

  // File is not a npbind.dat
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindMagic.dat"), std::runtime_error, "^Error: Input path is not a npbind.dat! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Passed file that wasn't a npbind.dat");

  // Could not read entries
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindEntriesSize.dat"), std::runtime_error, "^Error: Error reading entries! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Could not read entries");

  // Non-matching digests
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindDigest.dat"), std::runtime_error, "^Error: Digests do not match! Aborting... at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Passed non matching digests");

  // Success
  if (npbind::read("./tests/files/npbind/validNoEntry.dat").size() != 0) {
    FAIL() << "No entry file failed";
  }

  if (npbind::read("./tests/files/npbind/validNoEntry_alt.dat").size() != 0) { // Includes entry size in file header
    FAIL() << "No entry (alternative) file failed";
  }

  if (npbind::read("./tests/files/npbind/validOneEntry.dat").size() != 1) {
    FAIL() << "One entry entry file failed";
  }

  if (npbind::read("./tests/files/npbind/validTwoEntries.dat").size() != 2) {
    FAIL() << "Two entry entry file failed";
  }

  if (npbind::read("./tests/files/npbind/validThreeEntries.dat").size() != 3) {
    FAIL() << "Three entry entry file failed";
  }
}

#endif // NPBIND_TEST_HPP_
