// Copyright (c) 2021 Al Azif
// License: GPLv3

#include "sfo.hpp"
#include "common.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace sfo {
bool is_sfo(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream sfo_input(path, std::ios::in | std::ios::binary);
  if (!sfo_input || !sfo_input.good()) {
    sfo_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Read SFO header
  SfoHeader header;
  sfo_input.read((char *)&header, sizeof(header)); // Flawfinder: ignore
  if (!sfo_input.good()) {
    sfo_input.close();
    return false;
  }
  sfo_input.close();

  // Compare magic
  if (__builtin_bswap32(header.magic) == SFO_MAGIC) {
    return true;
  }

  return false;
}

std::vector<SfoData> read(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream sfo_input(path, std::ios::in | std::ios::binary);
  if (!sfo_input || !sfo_input.good()) {
    sfo_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check to make sure file is a SFO
  if (!is_sfo(path)) {
    sfo_input.close();
    FATAL_ERROR("Input path is not a SFO!");
  }

  // Read SFO header
  SfoHeader header;
  sfo_input.read((char *)&header, sizeof(header)); // Flawfinder: ignore
  if (!sfo_input.good()) {
    // Should never reach here... will affect coverage %
    sfo_input.close();
    FATAL_ERROR("Error reading SFO header!");
  }

  std::vector<SfoData> data;

  // Read data
  for (size_t i = 0; i < header.num_entries; i++) {
    // Cannot just read sizeof(SfoData) as it includes the std::string valude for key_name which is added in the next loop and the actual data added shortly after that
    SfoData temp_data;
    sfo_input.read((char *)&temp_data.key_offset, sizeof(temp_data.key_offset)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      FATAL_ERROR("Error reading entry key offset!");
    }
    sfo_input.read((char *)&temp_data.format, sizeof(temp_data.format)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      FATAL_ERROR("Error reading entry format!");
    }
    sfo_input.read((char *)&temp_data.length, sizeof(temp_data.length)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      FATAL_ERROR("Error reading entry length!");
    }
    sfo_input.read((char *)&temp_data.max_length, sizeof(temp_data.max_length)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      FATAL_ERROR("Error reading entry max length!");
    }
    sfo_input.read((char *)&temp_data.data_offset, sizeof(temp_data.data_offset)); // Flawfinder: ignore
    if (!sfo_input.good()) {
      sfo_input.close();
      FATAL_ERROR("Error reading entry data offset!");
    }
    data.push_back(temp_data);
  }

  // Read key names and data
  for (auto &&entry : data) {
    sfo_input.seekg(header.key_table_offset + entry.key_offset, sfo_input.beg);
    std::getline(sfo_input, entry.key_name, '\0');

    // TODO: This bit is broken
    unsigned char buffer[entry.length];
    sfo_input.seekg(header.data_table_offset + entry.data_offset, sfo_input.beg);
    sfo_input.read((char *)&buffer, sizeof(buffer));
    if (!sfo_input.good()) {
      sfo_input.close();
      FATAL_ERROR("Error reading data table!");
    }

    for (uint32_t i = 0; i < sizeof(buffer); i++) {
      entry.data.push_back(buffer[i]);
    }
  }
  sfo_input.close();

  return data;
}

std::vector<std::string> get_keys(std::vector<SfoData> data) {
  std::vector<std::string> temp_key_list;
  for (auto &&entry : data) {
    temp_key_list.push_back(entry.key_name);
  }
  return temp_key_list;
}

uint16_t get_format(const std::string &key, std::vector<SfoData> data) {
  for (auto &&entry : data) {
    if (entry.key_name == key) {
      return entry.format;
    }
  }
  FATAL_ERROR("Could not find key");
}

uint32_t get_length(const std::string &key, std::vector<SfoData> data) {
  for (auto &&entry : data) {
    if (entry.key_name == key) {
      return entry.length;
    }
  }
  FATAL_ERROR("Could not find key");
}

uint32_t get_max_length(const std::string &key, std::vector<SfoData> data) {
  for (auto &&entry : data) {
    if (entry.key_name == key) {
      return entry.max_length;
    }
  }
  FATAL_ERROR("Could not find key");
}

std::vector<unsigned char> get_value(const std::string &key, std::vector<SfoData> data) {
  for (auto &&entry : data) {
    if (entry.key_name == key) {
      std::vector<unsigned char> buffer;
      for (uint32_t i = 0; i < entry.length; i++) {
        buffer.push_back(entry.data[i]);
      }
      return buffer;
    }
  }
  FATAL_ERROR("Could not find key");
}

std::vector<SfoPubtoolinfoIndex> read_pubtool_data(std::vector<SfoData> data) {
  std::vector<std::string> sfo_keys = get_keys(data);
  std::vector<SfoPubtoolinfoIndex> pubtool_data;

  if (std::count(sfo_keys.begin(), sfo_keys.end(), std::string("PUBTOOLINFO"))) {
    std::vector<unsigned char> pubtoolinfo_buffer = get_value("PUBTOOLINFO", data);
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
      pubtool_data.push_back(temp_index);
    }
  }

  return pubtool_data;
}

std::vector<std::string> get_pubtool_keys(std::vector<SfoPubtoolinfoIndex> data) {
  std::vector<std::string> temp_key_list;
  for (auto &&entry : data) {
    temp_key_list.push_back(entry.key_name);
  }
  return temp_key_list;
}

std::string get_pubtool_value(const std::string &key, std::vector<SfoPubtoolinfoIndex> data) {
  for (auto &&entry : data) {
    if (entry.key_name == key) {
      return entry.value;
    }
  }
  FATAL_ERROR("Could not find key");
}

std::vector<SfoData> add_key(SfoData add_data, std::vector<SfoData> current_data) {
  // TODO
  UNUSED(add_data);
  UNUSED(current_data);

  return current_data;
}

std::vector<SfoData> add_pubtool_key(SfoPubtoolinfoIndex add_data, std::vector<SfoData> current_data) {
  // TODO
  UNUSED(add_data);
  UNUSED(current_data);

  return current_data;
}

std::vector<SfoData> remove_key(std::string remove_key, std::vector<SfoData> current_data) {
  // TODO
  UNUSED(remove_key);
  UNUSED(current_data);

  return current_data;
}

std::vector<SfoData> remove_pubtool_key(std::string remove_key, std::vector<SfoData> current_data) {
  // TODO
  UNUSED(remove_key);
  UNUSED(current_data);

  return current_data;
}

void write(std::vector<SfoData> data) {
  // TODO
  UNUSED(data);
}
} // namespace sfo
