// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#include "pkg.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "common.h"

namespace pkg {
std::string get_entry_name_by_type(uint32_t type) {
  std::stringstream ss;

  if (type == 0x0400) {
    ss << "license.dat";
  } else if (type == 0x0401) {
    ss << "license.info";
  } else if (type == 0x0402) {
    ss << "nptitle.dat";
  } else if (type == 0x0403) {
    ss << "npbind.dat";
  } else if (type == 0x0404) {
    ss << "selfinfo.dat";
  } else if (type == 0x0406) {
    ss << "imageinfo.dat";
  } else if (type == 0x0407) {
    ss << "target-deltainfo.dat";
  } else if (type == 0x0408) {
    ss << "origin-deltainfo.dat";
  } else if (type == 0x0409) {
    ss << "psreserved.dat";
  } else if (type == 0x1000) {
    ss << "param.sfo";
  } else if (type == 0x1001) {
    ss << "playgo-chunk.dat";
  } else if (type == 0x1002) {
    ss << "playgo-chunk.sha";
  } else if (type == 0x1003) {
    ss << "playgo-manifest.xml";
  } else if (type == 0x1004) {
    ss << "pronunciation.xml";
  } else if (type == 0x1005) {
    ss << "pronunciation.sig";
  } else if (type == 0x1006) {
    ss << "pic1.png";
  } else if (type == 0x1007) {
    ss << "pubtoolinfo.dat";
  } else if (type == 0x1008) {
    ss << "app/playgo-chunk.dat";
  } else if (type == 0x1009) {
    ss << "app/playgo-chunk.sha";
  } else if (type == 0x100A) {
    ss << "app/playgo-manifest.xml";
  } else if (type == 0x100B) {
    ss << "shareparam.json";
  } else if (type == 0x100C) {
    ss << "shareoverlayimage.png";
  } else if (type == 0x100D) {
    ss << "save_data.png";
  } else if (type == 0x100E) {
    ss << "shareprivacyguardimage.png";
  } else if (type == 0x1200) {
    ss << "icon0.png";
  } else if ((type >= 0x1201) && (type <= 0x121F)) {
    ss << "icon0_" << std::setfill('0') << std::setw(2) << type - 0x1201 << ".png";
  } else if (type == 0x1220) {
    ss << "pic0.png";
  } else if (type == 0x1240) {
    ss << "snd0.at9";
  } else if ((type >= 0x1241) && (type <= 0x125F)) {
    ss << "pic1_" << std::setfill('0') << std::setw(2) << type - 0x1241 << ".png";
  } else if (type == 0x1260) {
    ss << "changeinfo/changeinfo.xml";
  } else if ((type >= 0x1261) && (type <= 0x127F)) {
    ss << "changeinfo/changeinfo_" << std::setfill('0') << std::setw(2) << type - 0x1261 << ".xml";
  } else if (type == 0x1280) {
    ss << "icon0.dds";
  } else if ((type >= 0x1281) && (type <= 0x129F)) {
    ss << "icon0_" << std::setfill('0') << std::setw(2) << type - 0x1281 << ".dds";
  } else if (type == 0x12A0) {
    ss << "pic0.dds";
  } else if (type == 0x12C0) {
    ss << "pic1.dds";
  } else if ((type >= 0x12C1) && (type <= 0x12DF)) {
    ss << "pic1_" << std::setfill('0') << std::setw(2) << type - 0x12C1 << ".dds";
  } else if ((type >= 0x1400) && (type <= 0x147F)) {
    ss << "trophy/trophy" << std::setfill('0') << std::setw(2) << type - 0x1400 << ".trp";
  } else if ((type >= 0x1600) && (type <= 0x1609)) {
    ss << "keymap_rp/" << std::setfill('0') << std::setw(3) << type - 0x15FF << ".png";
  } else if ((type >= 0x1610) && (type <= 0x16F5)) {
    ss << "keymap_rp/" << std::setfill('0') << std::setw(2) << (type - 0x1610) / 10 << "/" << std::setfill('0') << std::setw(3) << (((type - 0x160F) % 10) ? (type - 0x160F) % 10 : 10) << ".png";
  }

  return ss.str();
}

// TODO: Decrypt encrypted entries
//   - IMAGE_KEY (type == 0x0100)
//   - license.info (Can't decrypt?)
//   - license.dat
//   - npbind.dat
//   - nptitle.dat
void extract_sc0(const std::string &pkg_path, const std::string &output_path) {
  // Check for empty or pure whitespace path
  if (pkg_path.empty() || std::all_of(pkg_path.begin(), pkg_path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty input path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(pkg_path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream pkg_input(pkg_path, std::ios::in | std::ios::binary);
  if (!pkg_input || !pkg_input.good()) {
    pkg_input.close();
    FATAL_ERROR("Cannot open input file: " + std::string(pkg_path));
  }

  // Check file magic (Read in whole header)
  PkgHeader header;
  pkg_input.read(reinterpret_cast<char *>(&header), sizeof(header)); // Flawfinder: ignore
  if (!pkg_input.good()) {
    pkg_input.close();
    FATAL_ERROR("Error reading PKG header!");
  }
  if (__builtin_bswap32(header.magic) != PKG_MAGIC) {
    pkg_input.close();
    // #include <iomanip>
    // std::stringstream ss;
    // ss << "File magic does not match a PKG! Expected: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << PKG_MAGIC << " | Actual: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << __builtin_bswap32(header.magic);
    // FATAL_ERROR(ss.str());
    FATAL_ERROR("Input path is not a PKG!");
  }

  // Read PKG entry table entries
  std::vector<PkgTableEntry> entries;
  pkg_input.seekg(__builtin_bswap32(header.entry_table_offset), pkg_input.beg);
  for (uint32_t i = 0; i < __builtin_bswap32(header.entry_count); i++) {
    PkgTableEntry temp_entry;
    pkg_input.read(reinterpret_cast<char *>(&temp_entry), sizeof(temp_entry)); // Flawfinder: ignore
    if (!pkg_input.good()) {
      pkg_input.close();
      FATAL_ERROR("Error reading entry table!");
    }
    entries.push_back(temp_entry);
  }

  // Check for empty or pure whitespace path
  if (output_path.empty() || std::all_of(output_path.begin(), output_path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty output path argument!");
  }

  // Make sure output directory path exists or can be created
  if (!std::filesystem::is_directory(output_path) && !std::filesystem::create_directories(output_path)) {
    pkg_input.close();
    FATAL_ERROR("Unable to open/create output directory");
  }

  // Extract sc0 entries
  for (auto &&entry : entries) {
    std::string entry_name = get_entry_name_by_type(__builtin_bswap32(entry.id));
    if (!entry_name.empty()) {
      std::filesystem::path temp_output_path(output_path);
      temp_output_path /= entry_name;

      pkg_input.seekg(__builtin_bswap32(entry.offset), pkg_input.beg);
      unsigned char temp_file[__builtin_bswap32(entry.size)];
      pkg_input.read(reinterpret_cast<char *>(&temp_file), __builtin_bswap32(entry.size)); // Flawfinder: ignore
      if (!pkg_input.good()) {
        pkg_input.close();
        FATAL_ERROR("Error reading entry data!");
      }

      std::filesystem::path temp_output_dir = temp_output_path;
      temp_output_dir.remove_filename();

      if (!std::filesystem::is_directory(temp_output_dir) && !std::filesystem::create_directories(temp_output_dir)) {
        pkg_input.close();
        FATAL_ERROR("Unable to open/create output subdirectory");
      }

      // Open path
      std::ofstream output_file(temp_output_path, std::ios::out | std::ios::trunc | std::ios::binary);
      if (!output_file || !output_file.good()) {
        pkg_input.close();
        output_file.close();
        FATAL_ERROR("Cannot open file: " + std::string(temp_output_path));
      }

      // Write to file
      std::stringstream ss;
      ss.write(reinterpret_cast<const char *>(&temp_file), __builtin_bswap32(entry.size));
      output_file << ss.rdbuf();
      output_file.close();
    }
  }
  pkg_input.close();
}
} // namespace pkg
