// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef ELF_TEST_HPP_
#define ELF_TEST_HPP_

#include "elf.hpp"

#include "testing.hpp"

#include <gtest/gtest.h>

TEST(elfTests, sceHeaderOffset) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                       // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Passed a file that was not a SELF");

  // This code will not pass because elf::is_self() within the function will fail before it can get to this check
  // EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenSelfSize.self"), "^Error: Error reading SELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken SELF header (Size)");

  // Check ELF header size
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenElfSize.self"), "^Error: Error reading ELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken ELF header (Size)");

  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_offset("./tests/files/elf/brokenElfMagic.self"), "^Error: Error reading ELF magic! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$", "Accepted broken ELF magic");

  // Valid tests
  EXPECT_EQ(0xC0, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffset_0xC0.self"));
  EXPECT_EQ(0x410, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffset_0x410.self"));
}

TEST(elfTests, getSceHeader) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Passed a file that was not a SELF");

  // Check SCE header size
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header("./tests/files/elf/brokenSceHeader.self"), "^Error: Error reading SCE header! at \"elf\\.cpp\":\\d*:\\(get_sce_header\\)$", "Accepted broken SCE Header (Size)");
}

TEST(elfTests, getSceHeaderNpdrm) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                      // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Passed a file that was not a SELF");

  // Check SCE header size
  EXPECT_EXCEPTION_REGEX(elf::get_sce_header_npdrm("./tests/files/elf/brokenSceHeaderNpdrm.self"), "^Error: Error reading SCE header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_npdrm\\)$", "Accepted broken SCE Header (Size)");
}

TEST(elfTests, isElf) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_elf(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_elf(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_elf("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_elf("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_elf("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_elf(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                        // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_elf("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_elf("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_elf("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_elf\\)$", "Could \"open\" file without permissions");

  // False
  EXPECT_FALSE(elf::is_elf("./tests/files/elf/brokenElfSize.elf"));
  EXPECT_FALSE(elf::is_elf("./tests/files/elf/brokenElfMagic.elf"));

  // True
  EXPECT_TRUE(elf::is_elf("./tests/files/elf/valid.elf"));
}

TEST(elfTests, isSelf) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_self(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_self(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_self("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_self("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_self("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_self(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                         // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_self("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_self("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_self("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_self\\)$", "Could \"open\" file without permissions");

  // False
  EXPECT_FALSE(elf::is_self("./tests/files/elf/brokenSelfSize.self"));
  EXPECT_FALSE(elf::is_self("./tests/files/elf/brokenSelfMagic.self"));

  // True
  EXPECT_TRUE(elf::is_self("./tests/files/elf/valid.self"));
}

TEST(elfTests, isNpdrm) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                          // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::is_npdrm("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(is_npdrm\\)$", "Passed a file that was not a SELF");

  // False
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_Fake.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_NpdrmDynlib.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_SystemExec.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_SystemDynlib.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_HostKernel.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_SecureModule.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_SecureKernel.self"));
  EXPECT_FALSE(elf::is_npdrm("./tests/files/elf/ptype_Unknown.self"));

  // True
  // TODO: Is it just npdrm_exec that are considered NPDRM or is npdrm_dynlib as well? What about fake?
  EXPECT_TRUE(elf::is_npdrm("./tests/files/elf/ptype_NpdrmExec.self"));
}

TEST(elfTests, getPtype) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_ptype(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                           // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Passed a file that was not a SELF");

  // Checks
  // TODO: Is it just npdrm_exec that are considered NPDRM or is npdrm_dynlib as well? What about fake?
  EXPECT_EQ("fake", elf::get_ptype("./tests/files/elf/ptype_Fake.self"));
  EXPECT_EQ("npdrm_exec", elf::get_ptype("./tests/files/elf/ptype_NpdrmExec.self"));
  EXPECT_EQ("npdrm_dynlib", elf::get_ptype("./tests/files/elf/ptype_NpdrmDynlib.self"));
  EXPECT_EQ("system_exec", elf::get_ptype("./tests/files/elf/ptype_SystemExec.self"));
  EXPECT_EQ("system_dynlib", elf::get_ptype("./tests/files/elf/ptype_SystemDynlib.self"));
  EXPECT_EQ("host_kernel", elf::get_ptype("./tests/files/elf/ptype_HostKernel.self"));
  EXPECT_EQ("secure_module", elf::get_ptype("./tests/files/elf/ptype_SecureModule.self"));
  EXPECT_EQ("secure_kernel", elf::get_ptype("./tests/files/elf/ptype_SecureKernel.self"));

  EXPECT_EXCEPTION_REGEX(elf::get_ptype("./tests/files/elf/ptype_Unknown.self"), "^Error: Unknown ptype! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$", "Passed an unknown ptype");
}

TEST(elfTests, getPaid) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_paid(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_paid(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_paid("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_paid("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_paid("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_paid(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                          // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_paid("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid_0s.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid_Fs.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid_0s_(NPDRM_Header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid_Fs_(NPDRM_Header).self"));
}

TEST(elfTests, getAppVersion) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_app_version(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                 // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_app_version("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_app_version\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_app_version("./tests/files/elf/getAppVersion_0s.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_app_version("./tests/files/elf/getAppVersion_Fs.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_app_version("./tests/files/elf/getAppVersion_0s_(NPDRM_Header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_app_version("./tests/files/elf/getAppVersion_Fs_(NPDRM_Header).self"));
}

TEST(elfTests, getFwVersion) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_fw_version("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$", "Passed a file that was not a SELF");

  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/getFwVersion_0s.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/getFwVersion_Fs.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/getFwVersion_0s_(NPDRM_Header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/getFwVersion_Fs_(NPDRM_Header).self"));
}

TEST(elfTests, getDigest) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_digest(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_digest(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_digest("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_digest("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_digest("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_digest(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                            // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_digest("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$", "Passed a file that was not a SELF");

  // Digests to test against
  std::vector<unsigned char> digest_0 = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  std::vector<unsigned char> digest_f = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  // Non-NPDRM header
  EXPECT_EQ(digest_0, elf::get_digest("./tests/files/elf/getDigest_0s.self"));
  EXPECT_EQ(digest_f, elf::get_digest("./tests/files/elf/getDigest_Fs.self"));

  // NPDRM header
  EXPECT_EQ(digest_0, elf::get_digest("./tests/files/elf/getDigest_0s_(NPDRM_Header).self"));
  EXPECT_EQ(digest_f, elf::get_digest("./tests/files/elf/getDigest_Fs_(NPDRM_Header).self"));
}

TEST(elfTests, getAuthInfo) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                               // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Could \"open\" file without permissions");

  // File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::get_auth_info("./tests/files/elf/brokenSelfMagic.self"), "^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_auth_info\\)$", "Passed a file that was not a SELF");

  // Unable to test this further
}

TEST(elfTests, isValidDecrypt) {
  // Original, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt(" ", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("  ", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("\t", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("\t\t", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Empty original path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt(nullptr, "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                           // nullptr

  // Original, Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/doesNotExist.ext", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Input original path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-existant file");

  // Original, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/notAFile.ext", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Input original path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-file object as file");

  // Original, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/noPermission.ext", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Cannot open original file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Could \"open\" file without permissions");

  // Original, File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/brokenSelfMagic.self", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"), "^Error: Input original path is not a SELF! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Passed a file that was not a SELF");

  // Decrypted, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", ""), "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", " "), "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "  "), "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "\t"), "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "\t\t"), "^Error: Empty decrypted path argument! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                            // nullptr

  // Decrypted, Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "./tests/files/elf/doesNotExist.ext"), "^Error: Input decrypted path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-existant file");

  // Decrypted, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "./tests/files/elf/notAFile.ext"), "^Error: Input decrypted path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Opened non-file object as file");

  // Decrypted, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "./tests/files/elf/noPermission.ext"), "^Error: Cannot open decrypted file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Could \"open\" file without permissions");

  // Decrypted, File is not an ELF
  EXPECT_EXCEPTION_REGEX(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "./tests/files/elf/brokenElfMagic.elf"), "^Error: Input decrypted path is not an ELF! at \"elf\\.cpp\":\\d*:\\(is_valid_decrypt\\)$", "Passed a file that was not a SELF");

  // Digests to test against
  // isValidDecrypt_Decrypted_1: Minimum ELF Header
  // isValidDecrypt_Decrypted_2: Page size - 1
  // isValidDecrypt_Decrypted_3: Page size
  // isValidDecrypt_Decrypted_4: Page size + 1

  // False, Non-NPDRM Header
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "./tests/files/elf/isValidDecrypt_Decrypted_4.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_2.self", "./tests/files/elf/isValidDecrypt_Decrypted_3.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_3.self", "./tests/files/elf/isValidDecrypt_Decrypted_2.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_4.self", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"));

  // False, NPDRM Header
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_4.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_2_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_3.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_3_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_2.elf"));
  EXPECT_FALSE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_4_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"));

  // True, Non-NPDRM Header
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1.self", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_2.self", "./tests/files/elf/isValidDecrypt_Decrypted_2.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_3.self", "./tests/files/elf/isValidDecrypt_Decrypted_3.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_4.self", "./tests/files/elf/isValidDecrypt_Decrypted_4.elf"));

  // True, NPDRM Header
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_1_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_1.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_2_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_2.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_3_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_3.elf"));
  EXPECT_TRUE(elf::is_valid_decrypt("./tests/files/elf/isValidDecrypt_Encrypted_4_(NPDRM_Header).self", "./tests/files/elf/isValidDecrypt_Decrypted_4.elf"));
}

TEST(elfTests, zeroSectionHeader) {
  // Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header(""), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header(" "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("  "), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("\t\t"), "^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header(nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                     // nullptr

  // Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/doesNotExist.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Opened non-existant file");

  // Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/notAFile.ext"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Opened non-file object as file");

  // Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/noPermission.ext"), "^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Could \"open\" file without permissions");

  // File is not an ELF
  EXPECT_EXCEPTION_REGEX(elf::zero_section_header("./tests/files/elf/brokenElfMagic.elf"), "^Error: Input path is not an ELF! at \"elf\\.cpp\":\\d*:\\(zero_section_header\\)$", "Passed a file that was not an ELF");

  std::filesystem::copy_file("./tests/files/elf/sectionHeaderZero.elf", "./tests/files/elf/sectionHeaderZero.elf.bak");
  SHA256SUM("./tests/files/elf/sectionHeaderZero.elf", "3FFCBC334138F0FCF5841262AF8D79F1861726D1E5893BCF3EE521C98474BF3B");
  EXPECT_NO_THROW(elf::zero_section_header("./tests/files/elf/sectionHeaderZero.elf"));
  SHA256SUM("./tests/files/elf/sectionHeaderZero.elf", "CC9D6FAC1F314CCBD3003C6A0E788A426958609B1312422D8FED40A08EE0BB51");
  if (std::filesystem::exists("./tests/files/elf/sectionHeaderZero.elf")) {
    std::filesystem::remove_all("./tests/files/elf/sectionHeaderZero.elf");
  }
  std::filesystem::rename("./tests/files/elf/sectionHeaderZero.elf.bak", "./tests/files/elf/sectionHeaderZero.elf");
}

TEST(elfTests, decrypt) {
  // Input, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::decrypt("", "./tests/files/elf/decryptedOutput.elf"), "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::decrypt(" ", "./tests/files/elf/decryptedOutput.elf"), "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("  ", "./tests/files/elf/decryptedOutput.elf"), "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("\t", "./tests/files/elf/decryptedOutput.elf"), "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt("\t\t", "./tests/files/elf/decryptedOutput.elf"), "^Error: Empty input path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt(nullptr, "./tests/files/elf/decryptedOutput.elf"), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                               // nullptr

  // Input, Non-existant file
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/doesNotExist.ext", "./tests/files/elf/decryptedOutput.elf"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Opened non-existant file");

  // Input, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/notAFile.ext", "./tests/files/elf/decryptedOutput.elf"), "^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Opened non-file object as file");

  // Input, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/noPermission.ext", "./tests/files/elf/decryptedOutput.elf"), "^Error: Cannot open input file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Could \"open\" file without permissions");

  // Input, File is not a SELF
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/brokenSelfMagic.self", "./tests/files/elf/decryptedOutput.elf"), "^Error: Input path is not a \\(S\\)ELF! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Passed a file that was not a (S)ELF");

  // Output, Empty input arguments
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", ""), "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted empty argument");          // Empty
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", " "), "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");    // Single space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", "  "), "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Double space
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", "\t"), "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument");   // Single tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", "\t\t"), "^Error: Empty output path argument! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Accepted whitespace argument"); // Double tab
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", nullptr), "^basic_string::_M_construct null not valid$", "Accepted nullptr argument");                                // nullptr

  // Output, Open non-file object
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", "./tests/files/elf/notAFile.ext"), "^Error: Output path exists, but is not a file! at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Opened non-file object as file");

  // Output, Open file without permission to access
  EXPECT_EXCEPTION_REGEX(elf::decrypt("./tests/files/elf/valid.self", "./tests/files/elf/noPermission.ext"), "^Error: Cannot open output file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(decrypt\\)$", "Could \"open\" file without permissions");

  // Success, already ELF (Run twice to see if it works when the file already exists)
  if (std::filesystem::exists("./tests/files/elf/decryptedOutput.elf")) {
    std::filesystem::remove("./tests/files/elf/decryptedOutput.elf");
  }
  EXPECT_NO_THROW(elf::decrypt("./tests/files/elf/valid.elf", "./tests/files/elf/decryptedOutput.elf"));
  SHA256SUM("./tests/files/elf/decryptedOutput.elf", "CC9D6FAC1F314CCBD3003C6A0E788A426958609B1312422D8FED40A08EE0BB51");
  EXPECT_NO_THROW(elf::decrypt("./tests/files/elf/valid.elf", "./tests/files/elf/decryptedOutput.elf"));
  SHA256SUM("./tests/files/elf/decryptedOutput.elf", "CC9D6FAC1F314CCBD3003C6A0E788A426958609B1312422D8FED40A08EE0BB51");
  if (std::filesystem::exists("./tests/files/elf/decryptedOutput.elf")) {
    std::filesystem::remove("./tests/files/elf/decryptedOutput.elf");
  }

  // TODO: `Error reading prog header!`

  // TODO: `Error reading SELF data!`

  // Unable to test this further
}

#endif // ELF_TEST_HPP_
