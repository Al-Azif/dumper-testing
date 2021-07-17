// Copyright (c) 2021 Al Azif
// License: GPLv3

#include "common.hpp"
#include "dump.hpp"

#include <iostream>

int main() {
  std::string dump_dir = "/mnt/usb0";
  dump::dump_base(dump_dir, "CUSA05258");
  dump::dump_patch(dump_dir, "CUSA05258");
  dump::dump_theme_unlock(dump_dir, "UP0700-CUSA05258_00-PS4TOBTHM0000001");

  return 0;
}
