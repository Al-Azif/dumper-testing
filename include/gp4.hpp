// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef GP4_HPP_
#define GP4_HPP_

#include <iostream>
#include <vector>

namespace gp4 {
void generate(const std::string &sfo_path, const std::string &output_path, const std::string &gp4_path, std::vector<std::string> &self_files, const std::string &type);
}

#endif // GP4_HPP_
