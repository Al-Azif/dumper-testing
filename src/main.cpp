// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#if defined(__TEST__)

#include <gtest/gtest.h>

#include "dump_test.hpp"
#include "elf_test.hpp"
#include "fself_test.hpp"
#include "gp4_test.hpp"
#include "npbind_test.hpp"
#include "pfs_test.hpp"
#include "pkg_test.hpp"
#include "sfo_test.hpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#else

#include "common.hpp"
#include "dump.hpp"

#include <string>

int main() {
  std::string dump_dir = "/mnt/usb0";
  dump::dump_base(dump_dir, "CUSA05258");
  dump::dump_patch(dump_dir, "CUSA05258");
  dump::dump_theme_unlock(dump_dir, "UP0700-CUSA05258_00-PS4TOBTHM0000001");

  return 0;
}

#endif // __TEST__
