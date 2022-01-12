// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#if defined(__TEST__)

#include <gtest/gtest.h>

#include "dump_test.h"
#include "elf_test.h"
#include "fself_test.h"
#include "gp4_test.h"
#include "npbind_test.h"
#include "pfs_test.h"
#include "pkg_test.h"
#include "sfo_test.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#else

#include <string>

#include "common.h"
#include "dump.h"

int main() {
  std::string dump_dir = "/mnt/usb0";
  dump::dump_base(dump_dir, "CUSA05258");
  dump::dump_patch(dump_dir, "CUSA05258");
  dump::dump_theme_unlock(dump_dir, "UP0700-CUSA05258_00-PS4TOBTHM0000001");

  return 0;
}

#endif // __TEST__
