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
    elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/does_not_exist.self");
    FAIL() << "Opened non-existant file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Input path does not exist";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/not_a_file.self");
    FAIL() << "Opened non-file object as file";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input path does not exist or is not a file! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Input path is not a file";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/no_permission.self");
    FAIL() << "Could \"open\" file without permissions";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Cannot open file: ./tests/files/elf/sceHeaderOffsets/no_permission.self at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Cannot open file";
    }
  }

  try {
    elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/broken_self_magic.self");
    FAIL() << "Accepted bad SELF magic";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Input file is not a SELF! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Input file is not a SELF (Magic)";
    }
  }

  // This code will not pass because elf::is_self() will fail before it can get to this check
  // try {
  //   elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/broken_self_size.self");
  //   FAIL() << "Accepted broken SELF header (Size)";
  // } catch (std::runtime_error const &err) {
  //   if (!std::regex_match(err.what(), std::regex("^Error: Error reading SELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
  //     FAIL() << "Error reading SELF header (Size)";
  //   }
  // }

  // TODO: Must add ELF magic check to elf::get_sce_header_offset() ctrl + f "TODO" in src/elf.cpp
  // try {
  //   elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/broken_elf_magic.self");
  //   FAIL() << "Accepted bad ELF magic";
  // } catch (std::runtime_error const &err) {
  //   if (!std::regex_match(err.what(), std::regex("^Error: Error reading ELF magic! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
  //     FAIL() << "Error reading ELF magic";
  //   }
  // }

  try {
    elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/broken_elf_size.self");
    FAIL() << "Accepted broken ELF header (Size)";
  } catch (std::runtime_error const &err) {
    if (!std::regex_match(err.what(), std::regex("^Error: Error reading ELF header! at \"elf\\.cpp\":\\d*:\\(get_sce_header_offset\\)$"))) {
      FAIL() << "Error reading ELF header (Size)";
    }
  }

  // Valid tests
  EXPECT_EQ(0xC0, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/0xC0.self"));
  EXPECT_EQ(0x410, elf::get_sce_header_offset("./tests/files/elf/sceHeaderOffsets/0x410.self"));
}

TEST(elfTests, getSceHeader) {
  // TODO
}

TEST(elfTests, getSceHeaderNpdrm) {
  // TODO
}

TEST(elfTests, isElf) {
  // TODO
}

TEST(elfTests, isSelf) {
  // TODO
}

TEST(elfTests, isNpdrm) {
  // TODO
}

TEST(elfTests, getPtype) {
  // TODO
}

TEST(elfTests, getPaid) {
  // TODO
}

TEST(elfTests, getAppVersion) {
  // TODO
}

TEST(elfTests, getFwVersion) {
  // TODO
}

TEST(elfTests, getDigest) {
  // TODO
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
  // Unlikely to be able to test this properly
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
