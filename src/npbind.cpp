// Copyright (c) 2021 Al Azif
// License: GPLv3

#include "npbind.hpp"
#include "common.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#ifdef __ORBIS__
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
  npbind_input.read((char *)&header, sizeof(header)); // Flawfinder: ignore
  if (!npbind_input.good()) {
    npbind_input.close();
    FATAL_ERROR("Error reading header!");
  }
  if (__builtin_bswap32(header.magic) != NPBIND_MAGIC) {
    npbind_input.close();
    std::stringstream ss;
    ss << "[npbind::read] File magic does not match npbind.dat! Expected: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << NPBIND_MAGIC << " | Actual: 0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << __builtin_bswap32(header.magic);
    FATAL_ERROR(ss.str());
  }

  // Read in body(s)
  std::vector<NpBindEntry> entries;
  for (uint64_t i = 0; i < __builtin_bswap64(header.num_entries); i++) {
    NpBindEntry temp_entry;
    npbind_input.read((char *)&temp_entry, __builtin_bswap64(header.entry_size)); // Flawfinder: ignore
    if (!npbind_input.good()) {
      npbind_input.close();
      FATAL_ERROR("Error reading entires!");
    }
    entries.push_back(temp_entry);
  }

  // Read digest
  unsigned char digest[SHA_DIGEST_LENGTH];
  npbind_input.seekg(-sizeof(digest), npbind_input.end); // Make sure we are in the right place
  npbind_input.read((char *)&digest, sizeof(digest));    // Flawfinder: ignore
  if (!npbind_input.good()) {
    npbind_input.close();
    FATAL_ERROR("Error reading digest!");
  }
  npbind_input.close();

  // Check digest
  unsigned char calculated_digest[sizeof(digest)];

  std::stringstream ss;
  ss.write(reinterpret_cast<const char *>(&header), sizeof(header));

  for (uint64_t i = 0; i < __builtin_bswap64(header.num_entries); i++) {
    ss.write(reinterpret_cast<const char *>(&entries[i]), sizeof(NpBindEntry));
  }

  unsigned char data_to_hash[ss.str().size()];
  for (size_t i = 0; i < ss.str().size(); i++) {
    data_to_hash[i] = ss.str().c_str()[i];
  }

#ifdef __ORBIS__
  SceSha1Context context;
  sceSha1BlockInit(&context);
  sceSha1BlockUpdate(&context, data_to_hash, ss.str().size());
  sceSha1BlockResult(calculated_digest, &context));
#else
  SHA_CTX context;
  SHA1_Init(&context);
  SHA1_Update(&context, data_to_hash, ss.str().size());
  SHA1_Final(calculated_digest, &context);
#endif // __ORBIS__

  if (std::memcmp(calculated_digest, digest, sizeof(digest)) != 0) {
    FATAL_ERROR("Digests do not match! Aborting...");
  }

  return entries;
}
} // namespace npbind
