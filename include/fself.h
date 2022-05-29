// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#ifndef DUMPER_INCLUDE_FSELF_H_
#define DUMPER_INCLUDE_FSELF_H_

#include <string>
#include <vector>

namespace fself {
bool is_fself(const std::string &path);
void make_fself(const std::string &input, const std::string &output, uint64_t paid, const std::string &ptype, uint64_t app_version, uint64_t fw_version, std::vector<unsigned char> auth_info);
void un_fself(const std::string &input, const std::string &output);
} // namespace fself

#endif // DUMPER_INCLUDE_FSELF_H_
