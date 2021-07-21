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
  EXPECT_EXCEPTION_REGEX(npbind::read("./tests/files/npbind/brokenNpbindEntriesSize.dat"), std::runtime_error, "^Error: Digests do not match! Aborting... at \"npbind\\.cpp\":\\d*:\\(read\\)$", "Passed non matching digests");

  // TODO: Success
  // No Entries
  // 1 Entry
  // 2 Entries
  // 3 Entries
}
