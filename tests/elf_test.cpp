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
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                         // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Could \"open\" file without permissions");

  // File is not a self
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Passed a file that was not a SELF");

  // This code will not pass because elf::is_self() within the function will fail before it can get to this check
  // EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenSelfSize.self"), std::runtime_error, "^Error: Error reading SELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken SELF header (Size)");

  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenElfSize.self"), std::runtime_error, "^Error: Error reading ELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken ELF header (Size)");

  // TODO: Must add ELF magic check to elf::get_sce_header_offset() ctrl + f "TODO" in src/elf.cpp
  // EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenElfMagic.self"), std::runtime_error, "^Error: Error reading ELF magic! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken ELF magic");

  // Valid tests
  EXPECT_EQ(0xC0, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffset_0xC0.self"));
  EXPECT_EQ(0x410, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffset_0x410.self"));
}

TEST(elfTests, getSceHeader) {
  // TODO
}

TEST(elfTests, getSceHeaderNpdrm) {
  // TODO
}

TEST(elfTests, isElf) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_elf(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_elf(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_elf("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_elf("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_elf("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument"); // Double tab
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
  EXPECT_EXCEPTION_REGEX(elf::is_self("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_self("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument"); // Double tab
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
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                             // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Could \"open\" file without permissions");

  // File is not a self
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
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                             // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Could \"open\" file without permissions");

  // File is not a self
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
  EXPECT_EXCEPTION_REGEX(elf::get_paid("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_paid("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_paid(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                            // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Could \"open\" file without permissions");

  // File is not a self
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/sceSection_0000000000000000.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/sceSection_FFFFFFFFFFFFFFFF.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/sceSection_0000000000000000_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/sceSection_FFFFFFFFFFFFFFFF_(npdrm_header).self"));
}

TEST(elfTests, getAppVersion) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                   // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Could \"open\" file without permissions");

  // File is not a self
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_app_version("./tests/files/elf/sceSection_0000000000000000.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_app_version("./tests/files/elf/sceSection_FFFFFFFFFFFFFFFF.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_app_version("./tests/files/elf/sceSection_0000000000000000_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_app_version("./tests/files/elf/sceSection_FFFFFFFFFFFFFFFF_(npdrm_header).self"));
}

TEST(elfTests, getFwVersion) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                  // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Could \"open\" file without permissions");

  // File is not a self
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/sceSection_0000000000000000.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/sceSection_FFFFFFFFFFFFFFFF.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/sceSection_0000000000000000_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/sceSection_FFFFFFFFFFFFFFFF_(npdrm_header).self"));
}

TEST(elfTests, getDigest) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_digest(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_digest(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_digest("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_digest("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_digest("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_digest(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                              // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Could \"open\" file without permissions");

  // File is not a self
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Passed a file that was not a SELF");

  // TODO
}

TEST(elfTests, getAuthInfo) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(""), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(" "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("  "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("    "), std::runtime_error, "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(nullptr), std::logic_error, "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                 // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/doesNotExist.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/notAFile.ext"), std::runtime_error, "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/noPermission.ext"), std::runtime_error, "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Could \"open\" file without permissions");

  // File is not a self
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/brokenSelfMagic.self"), std::runtime_error, "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Passed a file that was not a SELF");

  // Unable to test this further
}

TEST(elfTests, isValidDecrypt) {
  // TODO
}

TEST(elfTests, zeroSectionHeader) {
  // TODO
}

TEST(elfTests, decrypt) {
  // TODO
  // Unlikely to be able to test this properly
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
