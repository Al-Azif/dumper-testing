#include "../include/elf.hpp"

#include <regex>

#include <gtest/gtest.h>

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

TEST(elfTests, sceHeaderOffset) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                         // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Passed a file that was not a SELF");

  // This code will not pass because elf::is_self() within the function will fail before it can get to this check
  // EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenSelfSize.self"), std::runtime_error, "^Error: Error reading SELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken SELF header (Size)");

  // Check ELF header size
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenElfSize.self"), std::runtime_error, "^Error: Error reading ELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken ELF header (Size)");

  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenElfMagic.self"), std::runtime_error, "^Error: Error reading ELF magic! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken ELF magic");

  // Valid tests
  EXPECT_EQ(0xC0, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffset_0xC0.self"));
  EXPECT_EQ(0x410, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffset_0x410.self"));
}

TEST(elfTests, getSceHeader) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                  // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Passed a file that was not a SELF");

  // Check SCE header size
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/brokenSceHeader.self"), std::runtime_error, "^Error: Error reading SCE header! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted broken SCE Header (Size)");
}

TEST(elfTests, getSceHeaderNpdrm) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                        // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Passed a file that was not a SELF");

  // Check SCE header size
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/brokenSceHeaderNpdrm.self"), std::runtime_error, "^Error: Error reading SCE header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted broken SCE Header (Size)");
}

TEST(elfTests, isElf) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_elf(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_elf(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_elf("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_elf("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_elf("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_elf(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                          // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_elf("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_elf("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_elf("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Could \"open\" file without permissions");

  // False
  EXPECT_FALSE(elf::is_elf("./tests/files/elf/brokenElfSize.elf"));
  EXPECT_FALSE(elf::is_elf("./tests/files/elf/brokenElfMagic.elf"));

  // True
  EXPECT_TRUE(elf::is_elf("./tests/files/elf/isValidElf.elf"));
}

TEST(elfTests, isSelf) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_self(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_self(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_self("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_self("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_self("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_self(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                           // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_self("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_self("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_self("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Could \"open\" file without permissions");

  // False
  EXPECT_FALSE(elf::is_self("./tests/files/elf/brokenSelfSize.self"));
  EXPECT_FALSE(elf::is_self("./tests/files/elf/brokenSelfMagic.self"));

  // True
  EXPECT_TRUE(elf::is_self("./tests/files/elf/isValidSelf.self"));
}

TEST(elfTests, isNpdrm) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                            // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Passed a file that was not a SELF");

  // False
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_fake.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_npdrm_dynlib.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_system_exec.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_system_dynlib.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_host_kernel.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_secure_module.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_secure_kernel.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_unknown.self"));

  // True
  // TODO: Is it just npdrm_exec that are considered NPDRM or is npdrm_dynlib as well? What about fake?
  EXPECT_TRUE(elf::is_npdrm("./tests/files/elf/ptype_npdrm_exec.self"));
}

TEST(elfTests, getPtype) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                             // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Passed a file that was not a SELF");

  // Checks
  // TODO: Is it just npdrm_exec that are considered NPDRM or is npdrm_dynlib as well? What about fake?
  EXPECT_EQ("fake", elf::get_ptype("./tests/files/elf/ptype_fake.self"));
  EXPECT_EQ("npdrm_exec", elf::get_ptype("./tests/files/elf/ptype_npdrm_exec.self"));
  EXPECT_EQ("npdrm_dynlib", elf::get_ptype("./tests/files/elf/ptype_npdrm_dynlib.self"));
  EXPECT_EQ("system_exec", elf::get_ptype("./tests/files/elf/ptype_system_exec.self"));
  EXPECT_EQ("system_dynlib", elf::get_ptype("./tests/files/elf/ptype_system_dynlib.self"));
  EXPECT_EQ("host_kernel", elf::get_ptype("./tests/files/elf/ptype_host_kernel.self"));
  EXPECT_EQ("secure_module", elf::get_ptype("./tests/files/elf/ptype_secure_module.self"));
  EXPECT_EQ("secure_kernel", elf::get_ptype("./tests/files/elf/ptype_secure_kernel.self"));

  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/ptype_unknown.self"), std::runtime_error, "^Error: Unknown ptype! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Passed an unknown ptype");
}

TEST(elfTests, getPaid) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_paid(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_paid(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_paid("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_paid("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_paid("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_paid(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                            // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid_0s.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid_Fs.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid_0s_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid_Fs_(npdrm_header).self"));
}

TEST(elfTests, getAppVersion) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                   // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_app_version("./tests/files/elf/getAppVersion_0s.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_app_version("./tests/files/elf/getAppVersion_Fs.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_app_version("./tests/files/elf/getAppVersion_0s_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_app_version("./tests/files/elf/getAppVersion_Fs_(npdrm_header).self"));
}

TEST(elfTests, getFwVersion) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                  // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/getFwVersion_0s.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/getFwVersion_Fs.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/getFwVersion_0s_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/getFwVersion_Fs_(npdrm_header).self"));
}

TEST(elfTests, getDigest) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_digest(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_digest(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_digest("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_digest("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_digest("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_digest(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                              // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Passed a file that was not a SELF");

  // Digests to test against
  std::vector<unsigned char> digest_0 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  std::vector<unsigned char> digest_f = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Non-NPDRM header
  EXPECT_EQ(digest_0, elf::get_digest("./tests/files/elf/getDigest_0s.self"));
  EXPECT_EQ(digest_f, elf::get_digest("./tests/files/elf/getDigest_Fs.self"));

  // NPDRM header
  EXPECT_EQ(digest_0, elf::get_digest("./tests/files/elf/getDigest_0s_(npdrm_header).self"));
  EXPECT_EQ(digest_f, elf::get_digest("./tests/files/elf/getDigest_Fs_(npdrm_header).self"));
}

TEST(elfTests, getAuthInfo) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                 // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Passed a file that was not a SELF");

  // Unable to test this further
}

TEST(elfTests, isValidDecrypt) {
  // Original, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt(" ", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("  ", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("\t", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("\t\t", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt(nullptr, "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                             // nullptr

  // Original, Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/doesNotExist.ext", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Input original path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-existant file");

  // Original, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/notAFile.ext", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Input original path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-file object as file");

  // Original, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/noPermission.ext", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Cannot open original file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Could \"open\" file without permissions");

  // Original, File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/brokenSelfMagic.self", "./tests/files/elf/isValidDecryptDecrypted_1.elf"), std::runtime_error, "^Error: Input original path is not a SELF! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Passed a file that was not a SELF");

  // Decrypted, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", ""), std::runtime_error, "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", " "), std::runtime_error, "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "  "), std::runtime_error, "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "\t"), std::runtime_error, "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "\t\t"), std::runtime_error, "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                              // nullptr

  // Decrypted, Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input decrypted path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-existant file");

  // Decrypted, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input decrypted path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-file object as file");

  // Decrypted, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open decrypted file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Could \"open\" file without permissions");

  // Decrypted, File is not an ELF
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "./tests/files/elf/brokenElfMagic.elf"), std::runtime_error, "^Error: Input decrypted path is not an ELF! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Passed a file that was not a SELF");

  // Digests to test against
  // isValidDecryptDecrypted_1: Minimum ELF Header
  // isValidDecryptDecrypted_2: Page size - 1
  // isValidDecryptDecrypted_3: Page size
  // isValidDecryptDecrypted_4: Page size + 1

  // False, Non-NPDRM Header
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "./tests/files/elf/isValidDecryptDecrypted_4.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_2.self", "./tests/files/elf/isValidDecryptDecrypted_3.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_3.self", "./tests/files/elf/isValidDecryptDecrypted_2.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_4.self", "./tests/files/elf/isValidDecryptDecrypted_1.elf"));

  // False, NPDRM Header
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_4.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_2_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_3.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_3_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_2.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_4_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_1.elf"));

  // True, Non-NPDRM Header
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1.self", "./tests/files/elf/isValidDecryptDecrypted_1.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_2.self", "./tests/files/elf/isValidDecryptDecrypted_2.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_3.self", "./tests/files/elf/isValidDecryptDecrypted_3.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_4.self", "./tests/files/elf/isValidDecryptDecrypted_4.elf"));

  // True, NPDRM Header
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_1_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_1.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_2_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_2.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_3_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_3.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecryptEncrypted_4_(npdrm_header).self", "./tests/files/elf/isValidDecryptDecrypted_4.elf"));
}

TEST(elfTests, zeroSectionHeader) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("\t\t"), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                             // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Could \"open\" file without permissions");

  // File is not an ELF
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/brokenElfMagic.elf"), std::runtime_error, "^Error: Input path is not an ELF! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Passed a file that was not an ELF");

  EXPECT_NO_THROW(elf::zero_section_header("./tests/files/elf/sectionHeaderZero.elf"));
  // TODO: Validate
}

TEST(elfTests, decrypt) {
  // Input, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::decrypt("", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::decrypt(" ", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("  ", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("\t", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt("\t\t", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt(nullptr, "./tests/files/elf/decryptedOutput.elf"), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                              // nullptr

  // Input, Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/doesNotExist.ext", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Opened non-existant file");

  // Input, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/notAFile.ext", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Opened non-file object as file");

  // Input, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/noPermission.ext", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Cannot open input file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Could \"open\" file without permissions");

  // Input, File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/brokenSelfMagic.self", "./tests/files/elf/decryptedOutput.elf"), std::runtime_error, "^Error: Input path is not a \\(S\\)ELF! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Passed a file that was not a (S)ELF");

  // Output, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", ""), std::runtime_error, "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", " "), std::runtime_error, "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", "  "), std::runtime_error, "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", "\t"), std::runtime_error, "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", "\t\t"), std::runtime_error, "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                              // nullptr

  // Output, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", "./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Output path exists, but is not a file! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Opened non-file object as file");

  // Output, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/isValidSelf.self", "./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open output file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Could \"open\" file without permissions");

  // Success, already ELF (Run twice to see if it works when the file already exists)
  EXPECT_NO_THROW(elf::decrypt("./tests/files/elf/isValidElf.elf", "./tests/files/elf/decryptedOutput.elf"));
  // TODO: Check digest // CC9D6FAC1F314CCBD3003C6A0E788A426958609B1312422D8FED40A08EE0BB51
  EXPECT_NO_THROW(elf::decrypt("./tests/files/elf/isValidElf.elf", "./tests/files/elf/decryptedOutput.elf"));
  // TODO: Check digest // CC9D6FAC1F314CCBD3003C6A0E788A426958609B1312422D8FED40A08EE0BB51
  // TODO: Delete `./tests/files/elf/decryptedOutput.elf`

  // TODO: `Error reading prog header!`

  // TODO: `Error reading SELF data!`

  // Unable to test this further
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
