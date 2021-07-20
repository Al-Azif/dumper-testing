#include "../include/elf.hpp"

#include <regex>

#include <gtest/gtest.h>

TEST(elfTests, sceHeaderOffset) {
  try {
    elf::get_sce_header_offset("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::get_sce_header_offset(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::get_sce_header_offset("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::get_sce_header_offset(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::get_sce_header_offset("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::get_sce_header_offset(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/noPermission.ext");
    FAIL() << "Could \"open\" file without permissions";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Cannot open file";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/brokenSelfMagic.self");
    FAIL() << "Accepted bad SELF magic";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input file is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Input file is not a SELF (Magic)";
    }
  }

  // This code will not pass because elf::is_self() will fail before it can get to this check
  // try {
  //   elf::get_sce_header_offset("./tests/files/elf/brokenSelfSize.self");
  //   FAIL() << "Accepted broken SELF header (Size)";
  // } catch (std::runtime_error const &err) {
  //   if (!std::regex_match(err.what(), std::regex("^Error: Error reading SELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
  //     FAIL() << "Error reading SELF header (Size)";
  //   }
  // }

  // TODO: Must add ELF magic check to elf::get_sce_header_offset() ctrl + f "TODO" in src/elf.cpp
  // try {
  //   elf::get_sce_header_offset("./tests/files/elf/brokenElfMagic.self");
  //   FAIL() << "Accepted bad ELF magic";
  // } catch (std::runtime_error const &err) {
  //   if (!std::regex_match(err.what(), std::regex("^Error: Error reading ELF magic! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
  //     FAIL() << "Error reading ELF magic";
  //   }
  // }

  try {
    elf::get_sce_header_offset("./tests/files/elf/brokenElfSize.self");
    FAIL() << "Accepted broken ELF header (Size)";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Error reading ELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Error reading ELF header (Size)";
    }
  }

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
  try {
    elf::is_elf("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::is_elf(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::is_elf("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::is_elf(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::is_elf("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::is_elf(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::is_elf("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::is_elf("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::is_elf("./tests/files/elf/noPermission.ext");
    FAIL() << "Could \"open\" file without permissions";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_elf\\)$"))) {
      FAIL() << "Cannot open file";
    }
  }

  EXPECT_FALSE(elf::is_elf("./tests/files/elf/brokenElfSize.elf"));
  EXPECT_FALSE(elf::is_elf("./tests/files/elf/brokenElfMagic.elf"));

  EXPECT_TRUE(elf::is_elf("./tests/files/elf/isValidElf.elf"));
}

TEST(elfTests, isSelf) {
  try {
    elf::is_self("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::is_self(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::is_self("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::is_self(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::is_self("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::is_self(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::is_self("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::is_self("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::is_self("./tests/files/elf/noPermission.ext");
    FAIL() << "Could \"open\" file without permissions";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Cannot open file: \\./tests/files/elf/noPermission\\.ext at \"elf\\.cpp\":\\d*:\\(is_self\\)$"))) {
      FAIL() << "Cannot open file";
    }
  }

  EXPECT_FALSE(elf::is_self("./tests/files/elf/brokenSelfSize.self"));
  EXPECT_FALSE(elf::is_self("./tests/files/elf/brokenSelfMagic.self"));

  EXPECT_TRUE(elf::is_self("./tests/files/elf/isValidSelf.self"));
}

TEST(elfTests, isNpdrm) {
  // TODO
}

TEST(elfTests, getPtype) {
  try {
    elf::get_ptype("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::get_ptype(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::get_ptype("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::get_ptype(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::get_ptype("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::get_ptype(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::get_ptype("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_ptype("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_ptype("./tests/files/elf/brokenSelfMagic.self");
    FAIL() << "Passed a file that was not a SELF";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << err.what();
    }
  }

  /*
  // Non-NPDRM header
  EXPECT_EQ("fake", elf::get_ptype("./tests/files/elf/getPtype_fake.self"));
  EXPECT_EQ("npdrm_exec", elf::get_ptype("./tests/files/elf/getPtype_npdrm_exec.self"));
  EXPECT_EQ("npdrm_dynlib", elf::get_ptype("./tests/files/elf/getPtype_npdrm_dynlib.self"));
  EXPECT_EQ("system_exec", elf::get_ptype("./tests/files/elf/getPtype_system_exec.self"));
  EXPECT_EQ("system_dynlib", elf::get_ptype("./tests/files/elf/getPtype_system_dynlib.self"));
  EXPECT_EQ("host_kernel", elf::get_ptype("./tests/files/elf/getPtype_host_kernel.self"));
  EXPECT_EQ("secure_module", elf::get_ptype("./tests/files/elf/getPtype_secure_module.self"));
  EXPECT_EQ("secure_kernel", elf::get_ptype("./tests/files/elf/getPtype_secure_kernel.self"));

  try {
    elf::get_ptype("./tests/files/elf/getPtype_unknown.self");
    FAIL() << "Returned an unknown ptype";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Unknown ptype! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Unknown ptype";
    }
  }

  // NPDRM Header
  EXPECT_EQ("fake", elf::get_ptype("./tests/files/elf/getPtype_fake_(npdrm_header).self"));
  EXPECT_EQ("npdrm_exec", elf::get_ptype("./tests/files/elf/getPtype_npdrm_exec_(npdrm_header).self"));
  EXPECT_EQ("npdrm_dynlib", elf::get_ptype("./tests/files/elf/getPtype_npdrm_dynlib_(npdrm_header).self"));
  EXPECT_EQ("system_exec", elf::get_ptype("./tests/files/elf/getPtype_system_exec_(npdrm_header).self"));
  EXPECT_EQ("system_dynlib", elf::get_ptype("./tests/files/elf/getPtype_system_dynlib_(npdrm_header).self"));
  EXPECT_EQ("host_kernel", elf::get_ptype("./tests/files/elf/getPtype_host_kernel_(npdrm_header).self"));
  EXPECT_EQ("secure_module", elf::get_ptype("./tests/files/elf/getPtype_secure_module_(npdrm_header).self"));
  EXPECT_EQ("secure_kernel", elf::get_ptype("./tests/files/elf/getPtype_secure_kernel_(npdrm_header).self"));

  try {
    elf::get_ptype("./tests/files/elf/getPtype_unknown_(npdrm_header).self");
    FAIL() << "Returned an unknown ptype";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Unknown ptype! at \"elf\\.cpp\":\\d*:\\(get_ptype\\)$"))) {
      FAIL() << "Unknown ptype";
    }
  }
  */
}

TEST(elfTests, getPaid) {
  try {
    elf::get_paid("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::get_paid(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::get_paid("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::get_paid(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::get_paid("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::get_paid(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::get_paid("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_paid("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_paid("./tests/files/elf/brokenSelfMagic.self");
    FAIL() << "Passed a file that was not a SELF";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Input path is not a SELF";
    }
  }

  /*
  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid/paid_0000000000000000.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid/paid_FFFFFFFFFFFFFFFF.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid/paid_0000000000000000_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid/paid_FFFFFFFFFFFFFFFF_(npdrm_header).self"));
  */
}

TEST(elfTests, getAppVersion) {
  try {
    elf::get_paid("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::get_paid(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::get_paid("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::get_paid(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::get_paid("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::get_paid(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::get_paid("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_paid("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_paid("./tests/files/elf/brokenSelfMagic.self");
    FAIL() << "Passed a file that was not a SELF";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_paid\\)$"))) {
      FAIL() << "Input path is not a SELF";
    }
  }

  /*
  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid/paid_0000000000000000.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid/paid_FFFFFFFFFFFFFFFF.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_paid("./tests/files/elf/getPaid/paid_0000000000000000_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_paid("./tests/files/elf/getPaid/paid_FFFFFFFFFFFFFFFF_(npdrm_header).self"));
  */
}

TEST(elfTests, getFwVersion) {
  try {
    elf::get_fw_version("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::get_fw_version(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::get_fw_version("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::get_fw_version(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::get_fw_version("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::get_fw_version(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::get_fw_version("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_fw_version("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_fw_version("./tests/files/elf/brokenSelfMagic.self");
    FAIL() << "Passed a file that was not a SELF";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_fw_version\\)$"))) {
      FAIL() << "Input path is not a SELF";
    }
  }

  /*
  // Non-NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/getFwVersion/fw_version_0000000000000000.self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/getFwVersion/fw_version_FFFFFFFFFFFFFFFF.self"));

  // NPDRM header
  EXPECT_EQ(0x0000000000000000, elf::get_fw_version("./tests/files/elf/getFwVersion/fw_version_0000000000000000_(npdrm_header).self"));
  EXPECT_EQ(0xFFFFFFFFFFFFFFFF, elf::get_fw_version("./tests/files/elf/getFwVersion/fw_version_FFFFFFFFFFFFFFFF_(npdrm_header).self"));
  */
}

TEST(elfTests, getDigest) {
  try {
    elf::get_digest("");
    FAIL() << "Accepted empty argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Empty path argument (Empty)";
    }
  }

  try {
    elf::get_digest(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Empty path argument (Single space)";
    }
  }

  try {
    elf::get_digest("  ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Empty path argument (Double space)";
    }
  }

  try {
    elf::get_digest(" ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Empty path argument (Single tab)";
    }
  }

  try {
    elf::get_digest("   ");
    FAIL() << "Accepted whitespace argument";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Empty path argument! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Empty path argument (Double tab)";
    }
  }

  try {
    elf::get_digest(nullptr);
    FAIL() << "Accepted nullptr argument";
  } catch (std::logic_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^basic_string::_M_construct null not valid$"))) {
      FAIL() << "Empty path argument (nullptr)";
    }
  }

  try {
    elf::get_digest("./tests/files/elf/doesNotExist.ext");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_digest("./tests/files/elf/notAFile.ext");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_digest("./tests/files/elf/brokenSelfMagic.self");
    FAIL() << "Passed a file that was not a SELF";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_digest\\)$"))) {
      FAIL() << "Input path is not a SELF";
    }
  }

  /*
    TODO
  */
}

TEST(elfTests, getAuthInfo) {
  // TODO
  // Unlikely to be able to test this properly
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
