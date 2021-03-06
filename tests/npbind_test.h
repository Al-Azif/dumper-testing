// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef DUMPER_TESTS_NPBIND_TEST_H_
#define DUMPER_TESTS_NPBIND_TEST_H_

#include "npbind.h"

#include <gtest/gtest.h>

#include "testing.h"

TEST(npbindTest, read) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(npbind::read(""), "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(npbind::read(" "), "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(npbind::read("  "), "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(npbind::read("\t"), "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(npbind::read("\t\t"), "^Error: Empty path argument! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(npbind::read(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                         // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/npbind/noPermission\\.ext at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Could \"open\" file without permissions");

  // File is too small for a npbind.dat
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindHeaderSize.dat"), "^Error: Error reading header! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Could not read header");

  // File is not a npbind.dat
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindMagic.dat"), "^Error: Input path is not a npbind.dat! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Passed file that wasn't a npbind.dat");

  // Could not read entries
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindEntriesSize.dat"), "^Error: Error reading entries! at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Could not read entries");

  // Non-matching digests
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindDigest.dat"), "^Error: Digests do not match! Aborting... at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Passed non matching digests");

  // Success
  if (npbind::read("./tests/files/npbind/valid_NoEntry.dat").size() != 0) {
    FAIL() << "No entry file failed";
  }

  if (npbind::read("./tests/files/npbind/valid_NoEntry_Alternative.dat").size() != 0) { // Includes entry size in file header
    FAIL() << "No entry (alternative) file failed";
  }

  if (npbind::read("./tests/files/npbind/valid_OneEntry.dat").size() != 1) {
    FAIL() << "One entry entry file failed";
  }

  if (npbind::read("./tests/files/npbind/valid_TwoEntries.dat").size() != 2) {
    FAIL() << "Two entry entry file failed";
  }

  if (npbind::read("./tests/files/npbind/valid_ThreeEntries.dat").size() != 3) {
    FAIL() << "Three entry entry file failed";
  }
}

#endif // DUMPER_TESTS_NPBIND_TEST_H_
