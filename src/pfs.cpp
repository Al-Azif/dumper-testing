// Copyright (c) 2021 Al Azif
// License: GPLv3

#include "pfs.hpp"
#include "common.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

namespace pfs {
pfs_header header;
size_t pfs_size;
size_t pfs_copied;
std::vector<di_d32> inodes;

std::ifstream pfs_input;

void __parse_directory(uint32_t ino, uint32_t level, const std::string &output_path, bool calculate_only) {
  for (uint32_t i = 0; i < inodes[ino].blocks; i++) {
    uint32_t db = inodes[ino].db[0] + i;
    uint64_t pos = (uint64_t)header.blocksz * db;
    uint64_t size = inodes[ino].size;
    uint64_t top = pos + size;

    while (pos < top) {
      dirent_t ent;

      pfs_input.seekg(pos, pfs_input.beg);
      pfs_input.read((char *)&ent, sizeof(ent)); // Flawfinder: ignore
      if (!pfs_input.good()) {
        pfs_input.close();
        FATAL_ERROR("Error reading entry!");
      }

      if (ent.type == 0) {
        break;
      }

      char name[ent.namelen + 1];
      memset(name, '\0', sizeof(name));
      if (level > 0) {
        pfs_input.seekg(pos + sizeof(dirent_t), pfs_input.beg);
        pfs_input.read((char *)&name, ent.namelen); // Flawfinder: ignore
        if (!pfs_input.good()) {
          pfs_input.close();
          FATAL_ERROR("Error reading entry name!");
        }
      }

      std::filesystem::path new_output_path(output_path);
      new_output_path /= std::string(name);

      if (ent.type == 2 && level > 0) {
        if (calculate_only) {
          pfs_size += inodes[ent.ino].size;
        } else {
          // Open path
          std::ofstream output_file(new_output_path, std::ios::out | std::ios::trunc | std::ios::binary);
          if (!output_file || !output_file.good()) {
            pfs_input.close();
            output_file.close();
            FATAL_ERROR("Cannot open file: " + std::string(new_output_path));
          }

          unsigned char buffer[PFS_DUMP_BUFFER];
          memset(buffer, '\0', sizeof(buffer));

          std::stringstream ss;
          uint64_t dump_counter = 0;

          pfs_input.seekg((uint64_t)header.blocksz * inodes[ent.ino].db[0], pfs_input.beg);

          while (dump_counter + sizeof(buffer) <= inodes[ent.ino].size) {
            pfs_input.read((char *)&buffer, sizeof(buffer)); // Flawfinder: ignore
            if (!pfs_input.good()) {
              pfs_input.close();
              output_file.close();
              FATAL_ERROR("Error reading entry data!");
            }
            ss.write(reinterpret_cast<const char *>(&buffer), sizeof(buffer));
            output_file << ss.rdbuf();
            ss.str("");
            ss.clear();
            memset(buffer, '\0', sizeof(buffer));
            pfs_copied += sizeof(buffer);
            dump_counter += sizeof(buffer);
          }

          pfs_input.read((char *)&buffer, inodes[ent.ino].size - dump_counter); // Flawfinder: ignore
          if (!pfs_input.good()) {
            pfs_input.close();
            output_file.close();
            FATAL_ERROR("Error reading entry data!");
          }
          ss.write(reinterpret_cast<const char *>(&buffer), inodes[ent.ino].size - dump_counter);
          output_file << ss.rdbuf();
          ss.str("");
          ss.clear();
          dump_counter += inodes[ent.ino].size - dump_counter;
          pfs_copied += inodes[ent.ino].size - dump_counter;

          output_file.close();
        }
      } else if (ent.type == 3) {
        if (!calculate_only) {
          if (!std::filesystem::is_directory(new_output_path) && !std::filesystem::create_directory(new_output_path)) {
            pfs_input.close();
            FATAL_ERROR("Could not create output directory");
          }
        }
        __parse_directory(ent.ino, level + 1, new_output_path, calculate_only);
      }

      pos += ent.entsize;
    }
  }
}

void calculate_pfs_size(uint32_t ino, uint32_t level) {
  __parse_directory(ino, level, "", true);
}

void dump_pfs(uint32_t ino, uint32_t level, const std::string &output_path) {
  __parse_directory(ino, level, output_path, false);
}

void extract(const std::string &pfs_path, const std::string &output_path) {
  // Make sure output directory path exists or can be created
  if (!std::filesystem::is_directory(output_path) && !std::filesystem::create_directories(output_path)) {
    pfs_input.close();
    FATAL_ERROR("Unable to open/create output directory");
  }

  // Check for empty or pure whitespace path
  if (pfs_path.empty() || std::all_of(pfs_path.begin(), pfs_path.end(), [](char c) { return std::isspace(c); })) {
    pfs_input.close();
    FATAL_ERROR("Empty input path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(pfs_path)) {
    pfs_input.close();
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  pfs_input.open(pfs_path, std::ios::in | std::ios::binary);
  if (!pfs_input || !pfs_input.good()) {
    pfs_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(pfs_path));
  }

  // Check file magic (Read in whole header)
  pfs_input.read((char *)&header, sizeof(header)); // Flawfinder: ignore
  if (!pfs_input.good()) {
    pfs_input.close();
    FATAL_ERROR("Error reading header!");
  }
  if (__builtin_bswap64(header.magic) != PFS_MAGIC) {
    pfs_input.close();
    std::stringstream ss;
    ss << "[pfs::extract] File magic does not match pfs_image.dat! Expected: 0x" << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << PFS_MAGIC << " | Actual: 0x" << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << __builtin_bswap64(header.magic);
    FATAL_ERROR(ss.str());
  }

  // Read in inodes
  for (uint32_t i = 0; i < header.ndinodeblock; i++) {
    for (uint32_t j = 0; (j < (header.blocksz / sizeof(di_d32))) && (j < header.ndinode); j++) {
      di_d32 temp_inodes;
      pfs_input.seekg((uint64_t)header.blocksz * (i + 1) + sizeof(di_d32) * j, pfs_input.beg);
      pfs_input.read((char *)&temp_inodes, sizeof(di_d32)); // Flawfinder: ignore
      if (!pfs_input.good()) {
        pfs_input.close();
        FATAL_ERROR("Error reading inodes!");
      }
      inodes.push_back(temp_inodes);
    }
  }

  pfs_size = 0;
  pfs_copied = 0;

  calculate_pfs_size(header.superroot_ino, 0);
  dump_pfs(header.superroot_ino, 0, output_path);

  pfs_input.close();
}
} // namespace pfs
