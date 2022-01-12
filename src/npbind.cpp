// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#include "npbind.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "common.h"

#if defined(__ORBIS__)
#include <libsha1.h>
#else
#include <openssl/sha.h>
#endif // __ORBIS__

namespace npbind {
std::vector<npbind::NpBindEntry> read(const std::string &path) { // Flawfinder: ignore
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream npbind_input(path, std::ios::in | std::ios::binary);
  if (!npbind_input || !npbind_input.good()) {
    npbind_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check file magic (Read in whole header)
  NpBindHeader header;
  npbind_input.read(reinterpret_cast<char *>(&header), sizeof(header)); // Flawfinder: ignore
  if (!npbind_input.good()) {
    npbind_input.close();
    FATAL_ERROR("Error reading header!");
  }
  if (__builtin_bswap32(header.magic) != NPBIND_MAGIC) {
    npbind_input.close();
    // #include <iomanip>
    // std::stringstream ss;
    // ss << "File magic does not match npbind.dat! Expected: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << NPBIND_MAGIC << " | Actual: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << __builtin_bswap32(header.magic);
    // FATAL_ERROR(ss.str());
    FATAL_ERROR("Input path is not a npbind.dat!");
  }

  // Read in body(s)
  std::vector<NpBindEntry> entries;
  for (uint64_t i = 0; i < __builtin_bswap64(header.num_entries); i++) {
    NpBindEntry temp_entry;
    npbind_input.read(reinterpret_cast<char *>(&temp_entry), __builtin_bswap64(header.entry_size)); // Flawfinder: ignore
    if (!npbind_input.good()) {
      npbind_input.close();
      FATAL_ERROR("Error reading entries!");
    }
    entries.push_back(temp_entry);
  }

  // Read digest
  std::vector<unsigned char> digest(SHA_DIGEST_LENGTH);
  npbind_input.seekg(-digest.size(), npbind_input.end);                  // Make sure we are in the right place
  npbind_input.read(reinterpret_cast<char *>(&digest[0]),digest.size()); // Flawfinder: ignore
  if (!npbind_input.good()) {
    // Should never reach here... will affect coverage %
    npbind_input.close();
    FATAL_ERROR("Error reading digest!");
  }
  npbind_input.close();

  // Check digest
  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(&header), sizeof(header));

  for (uint64_t i = 0; i < __builtin_bswap64(header.num_entries); i++) {
    ss.write(reinterpret_cast<const char *>(&entries[i]), sizeof(NpBindEntry));
  }

  std::vector<unsigned char> data_to_hash;
  for (size_t i = 0; i < ss.str().size(); i++) {
    data_to_hash.push_back(ss.str().c_str()[i]);
  }

  std::vector<unsigned char> calculated_digest(digest.size());

#if defined(__ORBIS__)
  SceSha1Context context;
  sceSha1BlockInit(&context);
  sceSha1BlockUpdate(&context, &data_to_hash[0], ss.str().size());
  sceSha1BlockResult(&context, &calculated_digest[0]));
#else
  SHA_CTX context;
  SHA1_Init(&context);
  SHA1_Update(&context, &data_to_hash[0], ss.str().size());
  SHA1_Final(&calculated_digest[0], &context);
#endif // __ORBIS__

  if (std::memcmp(&calculated_digest[0], &digest[0], digest.size()) != 0) {
    FATAL_ERROR("Digests do not match! Aborting...");
  }

  return entries;
}
} // namespace npbind
