// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#ifndef DUMPER_INCLUDE_GP4_H_
#define DUMPER_INCLUDE_GP4_H_

#include <string>
#include <vector>

#include "pugixml.hpp"

namespace gp4 {
void recursive_directory(const std::string &path, pugi::xml_node &node, bool validation = false);
pugi::xml_document make_volume(const std::string &content_id, const std::string &volume_type, std::string c_date = "", std::string c_time = "");
pugi::xml_document make_playgo(const std::string &playgo_xml);
pugi::xml_document make_files(const std::string &path, bool validation = false);
pugi::xml_document make_directories(const std::string &path, bool validation = false);
pugi::xml_document assemble(const pugi::xml_document &volume, const pugi::xml_document &playgo, const pugi::xml_document &files, const pugi::xml_document &directories, const std::string &custom_version = "");
void write(const pugi::xml_document &xml, const std::string &path);
void generate(const std::string &sfo_path, const std::string &output_path, const std::string &gp4_path, const std::string &type, bool validation = false);
} // namespace gp4

#endif // DUMPER_INCLUDE_GP4_H_
