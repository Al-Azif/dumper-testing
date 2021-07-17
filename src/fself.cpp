// Copyright (c) 2021 Al Azif
// License: GPLv3

#include "fself.hpp"
#include "common.hpp"
#include "elf.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fself {
void make_fself(const std::string &input, const std::string &output, uint64_t paid, const std::string &ptype, uint64_t app_version, uint64_t fw_version, std::vector<unsigned char> auth_info) {
  // Check for empty or pure whitespace path
  if (input.empty() || std::all_of(input.begin(), input.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty input path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(input)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream self_input(input, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(input));
  }

  // Check to make sure file is a SELF
  if (!elf::is_self(input)) {
    self_input.close();
    FATAL_ERROR("Input file is not a SELF!");
  }

  // Check for empty or pure whitespace path
  if (output.empty() || std::all_of(output.begin(), output.end(), [](char c) { return std::isspace(c); })) {
    self_input.close();
    FATAL_ERROR("Empty output path argument!");
  }

  std::filesystem::path output_path(output);

  // Exists, but is not a file
  if (std::filesystem::exists(output_path) && !std::filesystem::is_regular_file(output_path)) {
    self_input.close();
    FATAL_ERROR("Oputput object exists but is not a file!");
  }

  if (paid > 0xFFFFFFFFFFFFFFFF) { // Unsigned, so no need to check < 0
    self_input.close();
    FATAL_ERROR("Invalid program authority ID!");
  }

  if (ptype != "fake" && ptype != "npdrm_exec" && ptype != "npdrm_dynlib" && ptype != "system_exec" && ptype != "system_dynlib" && ptype != "host_kernel" && ptype != "secure_module" && ptype != "secure_kernel") {
    self_input.close();
    FATAL_ERROR("Invalid ptype!");
  }

  if (app_version > 0xFFFFFFFFFFFFFFFF) { // Unsigned, so no need to check < 0
    self_input.close();
    FATAL_ERROR("Invalid application version!");
  }

  if (fw_version > 0xFFFFFFFFFFFFFFFF) { // Unsigned, so no need to check < 0
    self_input.close();
    FATAL_ERROR("Invalid firmware version!");
  }

  // Should be 0x110 in size and 0-9a-fA-F
  if (auth_info.size() != 0x110) {
    self_input.close();
    FATAL_ERROR("Auth info is invalid length!");
  }
  if (std::all_of(output.begin(), output.end(), [](char c) { return std::isxdigit(c); })) {
    self_input.close();
    FATAL_ERROR("Auth info is not hex!");
  }

  // Input may not be "correct" but it's valid at this point

  // TODO
  self_input.close();
}
} // namespace fself
