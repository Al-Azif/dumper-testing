// Copyright (c) 2021 Al Azif
// License: GPLv3

#include "sfo.hpp"
#include "common.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

sfo::sfo(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    fatal_error("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    fatal_error("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream sfo_input(path, std::ios::in | std::ios::binary);
  if (!sfo_input || !sfo_input.good()) {
    sfo_input.close();
    fatal_error("Cannot open file: " + std::string(path));
  }

  // Check file magic (Read in whole header)
  sfo_input.read((char *)&header_, sizeof(header_)); // Flawfinder: ignore
  if (!sfo_input.good()) {
    sfo_input.close();
    fatal_error("Error reading header!");
  }
  if (__builtin_bswap32(header_.magic) != SFO_MAGIC) {
    sfo_input.close();
    std::stringstream ss;
    ss << "[sfo::open] File magic does not match a SFO! Expected: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << SFO_MAGIC << " | Actual: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << __builtin_bswap32(header_.magic);
    fatal_error(ss.str());
  }

  // Read indices
  for (size_t i = 0; i < header_.num_entries; i++) {
    // Cannot just read sizeof(SfoIndex) as it includes the std::string valude for key_name which is added in the next loop
    SfoIndex temp_index;
    sfo_input.read((char *)&temp_index.key_offset, sizeof(temp_index.key_offset)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      fatal_error("Error reading entry key offset!");
    }
    sfo_input.read((char *)&temp_index.format, sizeof(temp_index.format)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      fatal_error("Error reading entry format!");
    }
    sfo_input.read((char *)&temp_index.length, sizeof(temp_index.length)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      fatal_error("Error reading entry length!");
    }
    sfo_input.read((char *)&temp_index.max_length, sizeof(temp_index.max_length)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      fatal_error("Error reading entry max length!");
    }
    sfo_input.read((char *)&temp_index.data_offset, sizeof(temp_index.data_offset)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      fatal_error("Error reading entry data offset!");
    }
    indices_.push_back(temp_index);
  }

  // Read key names
  for (auto &&entry : indices_) {
    sfo_input.seekg(header_.key_table_offset + entry.key_offset, sfo_input.beg);
    std::getline(sfo_input, entry.key_name, '\0');
  }

  // Read data
  sfo_input.seekg(0, sfo_input.end);
  uint32_t data_table_length = sfo_input.tellg();
  data_table_length -= header_.data_table_offset;
  unsigned char temp_data_table[data_table_length];
  sfo_input.seekg(header_.data_table_offset, sfo_input.beg);
  sfo_input.read((char *)&temp_data_table, data_table_length); // Flawfinder: ignore
  if (!sfo_input.good()) {
    sfo_input.close();
    fatal_error("Error reading data table!");
  }
  for (uint32_t i = 0; i < data_table_length; i++) {
    data_table_.push_back(temp_data_table[i]);
  }

  sfo_input.close();

  std::vector<std::string> sfo_keys = get_keys();
  if (std::count(sfo_keys.begin(), sfo_keys.end(), std::string("PUBTOOLINFO"))) {
    std::vector<unsigned char> pubtoolinfo_buffer = get_value("PUBTOOLINFO");
    std::stringstream ss(std::string(pubtoolinfo_buffer.begin(), pubtoolinfo_buffer.end()));

    std::vector<std::string> csv;
    while (ss.good()) {
      std::string substr;
      getline(ss, substr, ',');
      csv.push_back(substr);
    }

    for (auto &&value : csv) {
      SfoPubtoolinfoIndex temp_index;
      temp_index.key_name = value.substr(0, value.find('='));
      temp_index.value = value.substr(value.find('=') + 1, value.size());
      pubtool_indices_.push_back(temp_index);
    }
  }
}

std::vector<std::string> sfo::get_keys() {
  std::vector<std::string> temp_key_list;
  for (auto &&entry : indices_) {
    temp_key_list.push_back(entry.key_name);
  }
  return temp_key_list;
}

uint16_t sfo::get_format(const std::string &key) {
  for (auto &&entry : indices_) {
    if (entry.key_name == key) {
      return entry.format;
    }
  }
  fatal_error("Could not find key");
}

uint32_t sfo::get_length(const std::string &key) {
  for (auto &&entry : indices_) {
    if (entry.key_name == key) {
      return entry.length;
    }
  }
  fatal_error("Could not find key");
}

uint32_t sfo::get_max_length(const std::string &key) {
  for (auto &&entry : indices_) {
    if (entry.key_name == key) {
      return entry.max_length;
    }
  }
  fatal_error("Could not find key");
}

std::vector<unsigned char> sfo::get_value(const std::string &key) {
  for (auto &&entry : indices_) {
    if (entry.key_name == key) {
      std::vector<unsigned char> buffer;
      for (uint32_t i = 0; i < entry.length; i++) {
        buffer.push_back(data_table_[entry.data_offset + i]);
      }
      return buffer;
    }
  }
  fatal_error("Could not find key");
}

std::vector<std::string> sfo::get_pubtoolinfo_keys() {
  std::vector<std::string> temp_key_list;
  for (auto &&entry : pubtool_indices_) {
    temp_key_list.push_back(entry.key_name);
  }
  return temp_key_list;
}

std::string sfo::get_pubtoolinfo_value(const std::string &key) {
  for (auto &&entry : pubtool_indices_) {
    if (entry.key_name == key) {
      return entry.value;
    }
  }
  fatal_error("Could not find key");
}
