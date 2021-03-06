// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#include "elf.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "common.h"

#include <sha256.h>

namespace elf {
uint64_t get_sce_header_offset(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check to make sure file is a SELF
  if (!is_self(path)) {
    self_input.close();
    FATAL_ERROR("Input path is not a SELF!");
  }

  // Read SELF header
  SelfHeader self_header;
  self_input.read(reinterpret_cast<char *>(&self_header), sizeof(self_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    // Should never reach here... will affect coverage %
    self_input.close();
    FATAL_ERROR("Error reading SELF header!");
  }

  // Calculate ELF header offset from the number of SELF segments
  uint64_t elf_header_offset = sizeof(self_header) + self_header.num_of_segments * sizeof(SelfEntry);

  // Read ELF header
  Elf64_Ehdr elf_header;
  self_input.seekg(elf_header_offset, self_input.beg);
  self_input.read(reinterpret_cast<char *>(&elf_header), sizeof(elf_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    self_input.close();
    FATAL_ERROR("Error reading ELF header!");
  }
  self_input.close();

  // Check ELF magic
  // Can this be less bad/robust?
  unsigned char elf_magic[4];
  elf_magic[0] = (ELF_MAGIC >> 24) & 0xFF;
  elf_magic[1] = (ELF_MAGIC >> 16) & 0xFF;
  elf_magic[2] = (ELF_MAGIC >> 8) & 0xFF;
  elf_magic[3] = (ELF_MAGIC >> 0) & 0xFF;

  if (std::memcmp(elf_header.e_ident, elf_magic, 4) != 0) {
    FATAL_ERROR("Error reading ELF magic!");
  }

  // Calculate SCE header offset from number of ELF entries
  uint64_t sce_header_offset = elf_header_offset + elf_header.e_ehsize + elf_header.e_phnum * elf_header.e_phentsize;

  // Align
  while (sce_header_offset % 0x10 != 0) {
    sce_header_offset++;
  }

  return sce_header_offset;
}

SceHeader get_sce_header(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check to make sure file is a SELF
  if (!is_self(path)) {
    self_input.close();
    FATAL_ERROR("Input path is not a SELF!");
  }

  // Calculate SCE header offset from number of ELF entries
  uint64_t sce_header_offset = get_sce_header_offset(path);

  self_input.seekg(sce_header_offset, self_input.beg);
  SceHeader sce_header;
  self_input.read(reinterpret_cast<char *>(&sce_header), sizeof(sce_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    self_input.close();
    FATAL_ERROR("Error reading SCE header!");
  }

  return sce_header;
}

SceHeaderNpdrm get_sce_header_npdrm(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check to make sure file is a SELF
  if (!is_self(path)) {
    self_input.close();
    FATAL_ERROR("Input path is not a SELF!");
  }

  // Calculate SCE header offset from number of ELF entries
  uint64_t sce_header_offset = get_sce_header_offset(path);

  self_input.seekg(sce_header_offset, self_input.beg);
  SceHeaderNpdrm sce_header;
  self_input.read(reinterpret_cast<char *>(&sce_header), sizeof(sce_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    self_input.close();
    FATAL_ERROR("Error reading SCE header!");
  }

  return sce_header;
}

bool is_elf(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream elf_input(path, std::ios::in | std::ios::binary);
  if (!elf_input || !elf_input.good()) {
    elf_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Read ELF header
  SelfHeader elf_header;
  elf_input.read(reinterpret_cast<char *>(&elf_header), sizeof(Elf64_Ehdr)); // Flawfinder: ignore
  if (!elf_input.good()) {
    elf_input.close();
    return false;
  }
  elf_input.close();

  // Compare magic
  if (__builtin_bswap32(elf_header.magic) == ELF_MAGIC) {
    return true;
  }

  return false;
}

bool is_self(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Read SELF header
  SelfHeader self_header;
  self_input.read(reinterpret_cast<char *>(&self_header), sizeof(self_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    self_input.close();
    return false;
  }
  self_input.close();

  // Compare magic
  if (__builtin_bswap32(self_header.magic) == SELF_MAGIC) {
    return true;
  }

  return false;
}

bool is_npdrm(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check if the file is a SELF
  if (!is_self(path)) {
    self_input.close();
    FATAL_ERROR("Input path is not a SELF!");
  }

  // Read SELF header
  SelfHeader self_header;
  self_input.read(reinterpret_cast<char *>(&self_header), sizeof(self_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    // Should never reach here... will affect coverage %
    self_input.close();
    FATAL_ERROR("Error reading SELF header!");
  }

  uint64_t program_type = self_header.program_type;
  while (program_type >= 0x10) {
    program_type -= 0x10;
  }

  // TODO: Is it just npdrm_exec that are considered NPDRM or is npdrm_dynlib as well? What about fake?
  if (program_type == 0x4) {
    return true;
  }

  return false;
}

std::string get_ptype(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path (To check permissions)
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }
  self_input.close();

  // Check if the file is a SELF. If it's not it *should* not have a SCE header
  if (!is_self(path)) {
    FATAL_ERROR("Input path is not a SELF!");
  }

  uint64_t program_type;
  if (is_npdrm(path)) {
    program_type = get_sce_header_npdrm(path).program_type;
  } else {
    program_type = get_sce_header(path).program_type;
  }

  std::string output;
  switch (program_type) {
  case 0x1:
    output = "fake";
    break;
  case 0x4:
    output = "npdrm_exec";
    break;
  case 0x5:
    output = "npdrm_dynlib";
    break;
  case 0x8:
    output = "system_exec";
    break;
  case 0x9:
    output = "system_dynlib"; // Includes mono libraries
    break;
  case 0xC:
    output = "host_kernel";
    break;
  case 0xE:
    output = "secure_module";
    break;
  case 0xF:
    output = "secure_kernel";
    break;
  default:
    FATAL_ERROR("Unknown ptype!");
    break;
  }

  return output;
}

// https://www.psdevwiki.com/ps4/Program_Authority_ID
uint64_t get_paid(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path (To check permissions)
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }
  self_input.close();

  // Check if the file is a SELF. If it's not it *should* not have a SCE header
  if (!is_self(path)) {
    FATAL_ERROR("Input path is not a SELF!");
  }

  if (is_npdrm(path)) {
    return get_sce_header_npdrm(path).program_authority_id;
  }

  return get_sce_header(path).program_authority_id;
}

uint64_t get_app_version(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path (To check permissions)
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }
  self_input.close();

  // Check if the file is a SELF. If it's not it *should* not have a SCE header
  if (!is_self(path)) {
    FATAL_ERROR("Input path is not a SELF!");
  }

  if (is_npdrm(path)) {
    return get_sce_header_npdrm(path).app_version;
  }

  return get_sce_header(path).app_version;
}

uint64_t get_fw_version(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path (To check permissions)
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }
  self_input.close();

  // Check if the file is a SELF. If it's not it *should* not have a SCE header
  if (!is_self(path)) {
    FATAL_ERROR("Input path is not a SELF!");
  }

  if (is_npdrm(path)) {
    return get_sce_header_npdrm(path).fw_version;
  }

  return get_sce_header(path).fw_version;
}

std::vector<unsigned char> get_digest(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path (To check permissions)
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }
  self_input.close();

  // Check if the file is a SELF. If it's not it *should* not have a SCE header
  if (!is_self(path)) {
    FATAL_ERROR("Input path is not a SELF!");
  }

  std::vector<unsigned char> digest;
  if (is_npdrm(path)) {
    SceHeaderNpdrm sce_header = get_sce_header_npdrm(path);
    for (size_t i = 0; i < sizeof(sce_header.digest); i++) {
      digest.push_back(sce_header.digest[i]);
    }
  } else {
    SceHeader sce_header = get_sce_header(path);
    for (size_t i = 0; i < sizeof(sce_header.digest); i++) {
      digest.push_back(sce_header.digest[i]);
    }
  }

  return digest;
}

// https://www.psdevwiki.com/ps4/Auth_Info
std::vector<unsigned char> get_auth_info(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path (To check permissions)
  std::ifstream self_input(path, std::ios::in | std::ios::binary);
  if (!self_input || !self_input.good()) {
    self_input.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }
  self_input.close();

  // Check if the file is a SELF
  if (!is_self(path)) {
    FATAL_ERROR("Input path is not a SELF!");
  }

#if defined(__ORBIS__)
  // TODO:
  // This may be better as a Mira fuctions to expose the required kernel functions as getting the auth_info is done in kernel
  // int kern_get_self_auth_info(struct thread* td, const char* path, int pathseg, char* info);
  // int self_kget_auth_info(struct thread *td, struct path_kmethod_uap_t *uap) {
  //   char auth_info[0x88];
  //   memset(auth_info, 0, sizeof(auth_info));
  //
  //   const char *path = uap->path;
  //   int res = kern_get_self_auth_info(td, path, (int)UIO_SYSSPACE, auth_info);
  //   if (res == 0) {
  //     khexdump("AUTH_INFO", auth_info, 0x88);
  //     kdprintf("END_AUTH_INFO\n");
  //   } else {
  //     kdprintf("Failed to get AUTH_INFO\n");
  //   }
  // }
#elif defined(__TEST__)
  // TODO: Some sort of output to know the above code is functioning as much as can be expected
#else
  // TODO: Some sort of output to know the above code is functioning as much as can be expected
#endif

  std::vector<unsigned char> blah;

  return blah;
}

bool is_valid_decrypt(const std::string &original_path, const std::string &decrypted_path) {
  // Check for empty or pure whitespace path
  if (original_path.empty() || std::all_of(original_path.begin(), original_path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty original path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(original_path)) {
    FATAL_ERROR("Input original path does not exist or is not a file!");
  }

  // Open path
  std::ifstream original_self(original_path, std::ios::in | std::ios::binary);
  if (!original_self || !original_self.good()) {
    original_self.close();
    FATAL_ERROR("Cannot open original file: " + std::string(original_path));
  }
  original_self.close();

  // Check if file is a SELF
  if (!is_self(original_path)) {
    FATAL_ERROR("Input original path is not a SELF!");
  }

  // Check for empty or pure whitespace path
  if (decrypted_path.empty() || std::all_of(decrypted_path.begin(), decrypted_path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty decrypted path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(decrypted_path)) {
    FATAL_ERROR("Input decrypted path does not exist or is not a file!");
  }

  // Open path
  std::ifstream decrypted_elf(decrypted_path, std::ios::in | std::ios::binary);
  if (!decrypted_elf || !decrypted_elf.good()) {
    decrypted_elf.close();
    FATAL_ERROR("Cannot open decrypted file: " + std::string(decrypted_path));
  }

  // Check if file is an ELF
  if (!is_elf(decrypted_path)) {
    decrypted_elf.close();
    FATAL_ERROR("Input decrypted path is not an ELF!");
  }

  // Read digest
  std::vector<unsigned char> digest = get_digest(original_path);
  std::vector<unsigned char> calculated_digest(digest.size());

  SHA256 sha256;
  while (decrypted_elf.good()) {
    std::vector<unsigned char> buffer(PAGE_SIZE);
    decrypted_elf.read(reinterpret_cast<char *>(&buffer[0]), buffer.size()); // Flawfinder: ignore
    sha256.add(&buffer[0], decrypted_elf.gcount());
  }
  decrypted_elf.close();
  sha256.getHash(&calculated_digest[0]);

  if (std::memcmp(&calculated_digest[0], &digest[0], digest.size()) != 0) {
    return false;
  }

  return true;
}

// This is done in other dumpers but is it necessary?
void zero_section_header(const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Check if file exists and is file
  if (!std::filesystem::is_regular_file(path)) {
    FATAL_ERROR("Input path does not exist or is not a file!");
  }

  // Open path
  std::ifstream elf_path(path, std::ios::in | std::ios::binary);
  if (!elf_path || !elf_path.good()) {
    elf_path.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  // Check if file is an ELF
  if (!is_elf(path)) {
    elf_path.close();
    FATAL_ERROR("Input path is not an ELF!");
  }

  elf_path.seekg(0, elf_path.beg);
  Elf64_Ehdr elf_header;
  elf_path.read(reinterpret_cast<char *>(&elf_header), sizeof(elf_header)); // Flawfinder: ignore
  if (!elf_path.good()) {
    // Should never reach here... will affect coverage %
    elf_path.close();
    FATAL_ERROR("Error reading ELF header!");
  }
  elf_path.close();

  // Zero section headers as they are invalid
  elf_header.e_shoff = 0;
  elf_header.e_shnum = 0;
  elf_header.e_shentsize = 0;
  elf_header.e_shstrndx = 0;

  std::ofstream output_file(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!output_file || !output_file.good()) {
    // Should never reach here... will affect coverage %
    output_file.close();
    FATAL_ERROR("Cannot open file: " + std::string(path));
  }

  output_file.seekp(0, output_file.beg);
  output_file.write(reinterpret_cast<char *>(&elf_header), sizeof(elf_header));
  output_file.close();
}

// The following code inspired from:
// - https://github.com/AlexAltea/orbital
// - https://github.com/xvortex/ps4-dumper-vtx
void decrypt(const std::string &input, const std::string &output) {
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
    FATAL_ERROR("Cannot open input file: " + std::string(input));
  }

  // Check if file is already decrypted
  if (!is_elf(input) && !is_self(input)) {
    self_input.close();
    FATAL_ERROR("Input path is not a (S)ELF!");
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
    FATAL_ERROR("Output path exists, but is not a file!");
  }

  // Open path
  std::ofstream output_file(output_path, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!output_file || !output_file.good()) {
    self_input.close();
    output_file.close();
    FATAL_ERROR("Cannot open output file: " + std::string(output_path));
  }

  // File is already decrypted, just copy the file to the output location
  if (elf::is_elf(input)) {
    output_file << self_input.rdbuf();
    self_input.close();
    output_file.close();
    return;
  }

  // Read SELF header
  SelfHeader self_header;
  self_input.read(reinterpret_cast<char *>(&self_header), sizeof(self_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    // Should never reach here... will affect coverage %
    self_input.close();
    output_file.close();
    FATAL_ERROR("Error reading SELF header!");
  }

  // Calculate ELF header offset from the number of SELF segments
  uint64_t elf_header_offset = sizeof(self_header) + self_header.num_of_segments * sizeof(SelfEntry);
  Elf64_Ehdr elf_header;
  self_input.seekg(elf_header_offset, self_input.beg);
  self_input.read(reinterpret_cast<char *>(&elf_header), sizeof(elf_header)); // Flawfinder: ignore
  if (!self_input.good()) {
    // Should never reach here... will affect coverage %
    self_input.close();
    output_file.close();
    FATAL_ERROR("Error reading ELF header!");
  }

  // Calculate minimum ELF size
  uint64_t elf_size = elf_header.e_phoff + (elf_header.e_phnum * elf_header.e_phentsize);

  // Get program headers
  std::vector<Elf64_Phdr> prog_headers;
  for (uint16_t i = 0; i < elf_header.e_phnum; i++) {
    Elf64_Phdr prog_header;
    self_input.seekg(elf_header.e_phoff + (i * elf_header.e_phentsize), self_input.beg);
    self_input.read(reinterpret_cast<char *>(&prog_header), elf_header.e_phentsize); // Flawfinder: ignore
    if (!self_input.good()) {
      self_input.close();
      output_file.close();
      FATAL_ERROR("Error reading prog header!");
    }
    if ((prog_header.p_type == PT_LOAD || prog_header.p_type == PT_NID || prog_header.p_type == PT_DYNAMIC) && (elf_size < prog_header.p_offset + prog_header.p_filesz)) {
      elf_size = prog_header.p_offset + prog_header.p_filesz;
    }
    prog_headers.push_back(prog_header);
  }

  std::vector<uint8_t> self_data(self_header.self_size);
  self_input.seekg(0, self_input.beg);
  self_input.read(reinterpret_cast<char *>(&self_data[0]), self_data.size()); // Flawfinder: ignore
  if (!self_input.good()) {
    self_input.close();
    output_file.close();
    FATAL_ERROR("Error reading SELF data!");
  }

  // We're done with the input as a stream
  self_input.close();

  // Copy ELF header to elf_data
  std::vector<uint8_t> elf_data;
  for (uint16_t i = 0; i < elf_header.e_ehsize; i++) {
    elf_data.insert(elf_data.begin() + i, self_data[elf_header_offset + i]);
    // TODO: The above line replaced the line below... test it
    // elf_data[i] = self_data[elf_header_offset + i]; // Array overrun is possible. The 'i' index is pointing beyond array bound.
  }

  // Open input file as descriptor
  int fd = open(input.c_str(), O_RDONLY, 0);
  if (fd < 0) {
    // Should never reach here... will affect coverage %
    output_file.close();
    FATAL_ERROR("Cannot open input file: " + std::string(input));
  }

  // Loop: Copy program headers and then segments via mmap
  for (uint64_t i = 0; i < elf_header.e_phnum; i++) {
    // Copy program header to correct offset in elf_data
    for (uint64_t j = 0; j < elf_header.e_phentsize; j++) {
      uint64_t offset = elf_header.e_phoff + (i * elf_header.e_phentsize) + j;
      elf_data.insert(elf_data.begin() + elf_header_offset + offset, self_data[elf_header_offset + offset]);
      // TODO: The above line replaced the line below... test it
      // elf_data[elf_header_offset + offset] = self_data[elf_header_offset + offset];
    }

    // Copy program data to correct offset in elf_data via mmap to decrypt
    if (prog_headers[i].p_type == PT_LOAD || prog_headers[i].p_type == PT_NID) {
      lseek(fd, 0, SEEK_SET);

#if defined(__ORBIS__)
      uint64_t *elf_segment = static_cast<uint64_t *>(mmap(NULL, prog_headers[i].p_filesz, PROT_READ, MAP_SHARED | MAP_SELF, fd, i << 32));
      for (uint64_t j = 0; j < prog_headers[i].p_filesz; j++) {
        elf_data.insert(elf_data.begin() + prog_headers[i].p_offset + j, elf_segment[j]);
        // TODO: The above line replaced the line below... test it
        // elf_data[prog_headers[i].p_offset + j] = elf_segment[j];
      }
      munmap(elf_segment, prog_headers[i].p_filesz);
#elif defined(__TEST__)
      // TODO: Some sort of output to know the above code is functioning as much as can be expected
#else
      // TODO: Some sort of output to know the above code is functioning as much as can be expected
#endif
    }
  }

  close(fd);

  // Write decrypted data to output path
  for (auto &&byte : elf_data) {
    output_file << byte;
  }

  output_file.close();
}
} // namespace elf
