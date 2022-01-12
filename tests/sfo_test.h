// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef DUMPER_TESTS_SFO_TEST_H_
#define DUMPER_TESTS_SFO_TEST_H_

#include "sfo.h"

#include <gtest/gtest.h>

#include "testing.h"

TEST(sfoTests, isSfo) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo(""), "^Error: Empty path argument! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo(" "), "^Error: Empty path argument! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo("  "), "^Error: Empty path argument! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo("\t"), "^Error: Empty path argument! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo("\t\t"), "^Error: Empty path argument! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                        // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo("./tests/files/sfo/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo("./tests/files/sfo/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(sfo::is_sfo("./tests/files/sfo/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/sfo/noPermission\\.ext at \"sfo\\.cpp\":\\d*:\\(is_sfo\\)$", "Could \"open\" file without permissions");

  /* TODO
  // False
  EXPECT_FALSE(sfo::is_sfo("./tests/files/sfo/brokenSfoSize.sfo"));
  EXPECT_FALSE(sfo::is_sfo("./tests/files/sfo/brokenSfoMagic.sfo"));

  // True
  EXPECT_TRUE(sfo::is_sfo("./tests/files/sfo/valid.sfo"));
  */
}

TEST(sfoTests, read) {
  // TODO
}

TEST(sfoTests, getKeys) {
  // TODO
}

TEST(sfoTests, getFormat) {
  // TODO
}

TEST(sfoTests, getLength) {
  // TODO
}

TEST(sfoTests, getMaxLength) {
  // TODO
}

TEST(sfoTests, getValue) {
  // TODO
}

TEST(sfoTests, readPubtoolData) {
  // TODO
}

TEST(sfoTests, getPubtoolKeys) {
  // TODO
}

TEST(sfoTests, getPubtoolValue) {
  // TODO
}

TEST(sfoTests, buildData) {
  // TODO
}

TEST(sfoTests, buildPubtoolData) {
  // TODO
}

TEST(sfoTests, addData) {
  // TODO
}

TEST(sfoTests, addPubtoolData) {
  // TODO
}

TEST(sfoTests, removeKey) {
  // TODO
}

TEST(sfoTests, removePubtoolKey) {
  // TODO
}

TEST(sfoTests, compareSfoData) {
  // TODO
}

TEST(sfoTests, write) {
  // TODO
}

#endif // DUMPER_TESTS_SFO_TEST_H_
