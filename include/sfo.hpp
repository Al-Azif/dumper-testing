// Copyright (c) 2021 Al Azif
// License: GPLv3

#include <iostream>
#include <vector>

#ifndef SFO_HPP_
#define SFO_HPP_

#define SFO_MAGIC 0x00505346

class sfo {
public:
  explicit sfo(const std::string &path);

  std::vector<std::string> get_keys();
  uint16_t get_format(const std::string &key);
  uint32_t get_length(const std::string &key);
  uint32_t get_max_length(const std::string &key);
  std::vector<unsigned char> get_value(const std::string &key);
  std::vector<std::string> get_pubtoolinfo_keys();
  std::string get_pubtoolinfo_value(const std::string &key);

private:
  typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t key_table_offset;
    uint32_t data_table_offset;
    uint32_t num_entries;
  } SfoHeader;

  typedef struct {
    std::string key_name;
    uint16_t key_offset;
    uint16_t format;
    uint32_t length;
    uint32_t max_length;
    uint32_t data_offset;
  } SfoIndex;

  typedef struct {
    std::string key_name;
    std::string value;
  } SfoPubtoolinfoIndex;

  SfoHeader header_;
  std::vector<SfoIndex> indices_;
  std::vector<SfoPubtoolinfoIndex> pubtool_indices_;
  std::vector<unsigned char> data_table_;
};

#endif // SFO_HPP_
