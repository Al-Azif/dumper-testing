// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#include "gp4.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "pugixml.hpp"

#include "common.h"
#include "elf.h" // `bool is_self(const std::string &path);`
#include "sfo.h"

namespace gp4 {
void recursive_directory(const std::string &path, pugi::xml_node &node) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  for (auto &&p : std::filesystem::directory_iterator(path)) {
    if (std::filesystem::is_directory(p.path())) {
      pugi::xml_node dir_node = node.append_child("dir");

      std::string temp_path = p.path();
      temp_path.erase(0, path.size());
      while (temp_path.back() == '/') {
        temp_path.pop_back();
      }
      while (temp_path.at(0) == '/') {
        temp_path.erase(0, 1);
      }
      dir_node.append_attribute("targ_name") = temp_path.c_str();

      recursive_directory(p.path(), dir_node);
    }
  }
}

pugi::xml_document make_volume(const std::string &content_id, const std::string &volume_type, std::string c_date, std::string c_time) {
  // Check input
  if (!std::regex_match(content_id, std::regex("^\\D{2}\\d{4}-\\D{4}\\d{5}_\\d{2}-\\w{16}$"))) {
    FATAL_ERROR("Malformed content ID");
  }

  if (!c_date.empty() && !std::regex_match(c_date, std::regex("^\\d{4}-\\d{2}-\\d{2}$"))) {
    FATAL_ERROR("Malformed c_date");
  }

  if (!c_time.empty() && !std::regex_match(c_time, std::regex("^\\d{6}$"))) {
    FATAL_ERROR("Malformed c_time");
  }

  if (volume_type != "pkg_ps4_app" && volume_type != "pkg_ps4_patch" && volume_type != "pkg_ps4_remaster" && volume_type != "pkg_ps4_theme" && volume_type != "additional-content-data" && volume_type != "additional-content-no-data") {
    FATAL_ERROR("Unknown volume type");
  }

  // Generate XML
  pugi::xml_document doc;

  pugi::xml_node volume_node = doc.append_child("volume");
  pugi::xml_node volume_type_node = volume_node.append_child("volume_type");
  volume_type_node.append_child(pugi::node_pcdata).set_value(volume_type.c_str());
  pugi::xml_node volume_ts_node = volume_node.append_child("volume_ts");

  // Get current time for volume_ts... and possibly c_date
  std::time_t t = std::time(0);
  std::tm *timeinfo = std::localtime(&t);

  char buffer[20];
  std::memset(buffer, '\0', sizeof(buffer));
  std::strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);
  volume_ts_node.append_child(pugi::node_pcdata).set_value(buffer);

  pugi::xml_node package_node = volume_node.append_child("package");
  package_node.append_attribute("content_id") = content_id.c_str();
  package_node.append_attribute("passcode") = "00000000000000000000000000000000";
  if (volume_type == "additional-content-data" || volume_type == "additional-content-no-data") {
    // TODO: Attempt to get entitlement_key based on content_id
    package_node.append_attribute("entitlement_key") = "";
  }
  package_node.append_attribute("storage_type") = "digital50";
  package_node.append_attribute("app_type") = "full";

  // Set c_date and possibly c_time
  if (c_date.empty()) {
    std::memset(buffer, '\0', sizeof(buffer));
    std::strftime(buffer, 20, "%Y-%m-%d", timeinfo);
    c_date = std::string(buffer);
  }
  package_node.append_attribute("c_date") = c_date.c_str();

  if (!c_time.empty()) {
    package_node.append_attribute("c_time") = c_time.c_str();
  }

  return doc;
}

pugi::xml_document make_playgo(const std::string &playgo_xml) {
  pugi::xml_document doc;

  // TODO: pugixml kills attributes in the <xml> node itself... get them back/add to default build

  // Return external XML if it exists
  if (std::filesystem::exists(playgo_xml) && std::filesystem::is_regular_file(playgo_xml) && doc.load_file(playgo_xml.c_str())) {
    return doc;
  }

  // Build default
  pugi::xml_node psproject_node = doc.append_child("psproject");
  psproject_node.append_attribute("fmt") = "playgo-manifest";
  psproject_node.append_attribute("version") = "1000";

  pugi::xml_node volume_node = psproject_node.append_child("volume");

  pugi::xml_node chunk_info_node = volume_node.append_child("chunk_info");
  chunk_info_node.append_attribute("chunk_count") = "1";
  chunk_info_node.append_attribute("scenario_count") = "1";

  pugi::xml_node chunks_node = chunk_info_node.append_child("chunks");

  pugi::xml_node chunk_node = chunks_node.append_child("chunk");
  chunk_node.append_attribute("id") = "0";
  chunk_node.append_attribute("layer_no") = "0";
  chunk_node.append_attribute("label") = "Chunk #0";

  pugi::xml_node scenarios_node = chunk_info_node.append_child("scenarios");
  scenarios_node.append_attribute("default_id") = "0";

  pugi::xml_node scenario_node = scenarios_node.append_child("scenario");
  scenario_node.append_attribute("id") = "0";
  scenario_node.append_attribute("type") = "sp";
  scenario_node.append_attribute("initial_chunk_count") = "1";
  scenario_node.append_attribute("label") = "Scenario #0";

  return doc;
}

pugi::xml_document make_files(const std::string &path, std::vector<std::string> &elf_files) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty path argument!");
  }

  // Generate XML
  pugi::xml_document doc;

  pugi::xml_node files_node = doc.append_child("files");
  files_node.append_attribute("img_no") = "0"; // TODO: PlayGo

  for (auto &&p : std::filesystem::recursive_directory_iterator(path)) {
    if (std::filesystem::is_regular_file(p.path())) {
      bool self;
      if ((self = elf::is_self(p.path()))) {
        elf_files.push_back(p.path());
      }

      std::filesystem::path targ_path = std::filesystem::relative(p.path(), path);
      std::filesystem::path orig_path = std::filesystem::relative(p.path(), path);

      // If SELF redirect to .fself
      if (self) {
        orig_path.replace_extension(".fself");
      }

      // TODO:
      //   - Add proper PlayGo options
      //   - Add PFS Compression option
      pugi::xml_node file_node = files_node.append_child("file");
      file_node.append_attribute("targ_path") = targ_path.c_str();
      file_node.append_attribute("orig_path") = orig_path.c_str();
    }
  }

  return doc;
}

pugi::xml_document make_directories(const std::string &path) {
  // Generate XML
  pugi::xml_document doc;

  pugi::xml_node rootdir_node = doc.append_child("rootdir");

  recursive_directory(path, rootdir_node);

  return doc;
}

pugi::xml_document assemble(const pugi::xml_document &volume, const pugi::xml_document &playgo, const pugi::xml_document &files, const pugi::xml_document &directories, const std::string &custom_version) {
  // Generate XML
  pugi::xml_document doc;

  // TODO: Set XML node attributes

  pugi::xml_node psproject_node = doc.append_child("psproject");
  psproject_node.append_attribute("xmlns:xsd") = "http://www.w3.org/2001/XMLSchema";
  psproject_node.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
  psproject_node.append_attribute("fmt") = "gp4";
  if (custom_version.empty()) {
    if (!playgo.child("psproject").attribute("version").empty()) {
      psproject_node.append_attribute("version") = playgo.child("psproject").attribute("version").value(); // TODO: Will this be correct?
    }
  } else {
    psproject_node.append_attribute("version") = custom_version.c_str();
  }

  psproject_node.append_copy(volume.child("volume"));
  psproject_node.child("volume").append_copy(playgo.child("psproject").child("volume").child("chunk_info"));
  psproject_node.append_copy(files.child("files"));
  psproject_node.append_copy(directories.child("rootdir"));

  return doc;
}

void write(const pugi::xml_document &xml, const std::string &path) {
  // Check for empty or pure whitespace path
  if (path.empty() || std::all_of(path.begin(), path.end(), [](char c) { return std::isspace(c); })) {
    FATAL_ERROR("Empty output path argument!");
  }

  std::filesystem::path output_path(path);

  // Exists, but is not a file
  if (std::filesystem::exists(output_path) && !std::filesystem::is_regular_file(output_path)) {
    FATAL_ERROR("Output object exists but is not a file!");
  }

  // Open path
  std::ofstream output_file(output_path, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!output_file || !output_file.good()) {
    output_file.close();
    FATAL_ERROR("Cannot open file: " + std::string(output_path));
  }

  xml.save(output_file);

  output_file.close();
}

void generate(const std::string &sfo_path, const std::string &output_path, const std::string &gp4_path, std::vector<std::string> &self_files, const std::string &type) {
  if (type != "base" && type != "patch" && type != "remaster" && type != "theme" && type != "theme-unlock" && type != "additional-content-data" && type != "additional-content-no-data") {
    FATAL_ERROR("Unknown asset type");
  }

  std::vector<sfo::SfoData> sfo_data = sfo::read(sfo_path); // Flawfinder: ignore
  std::vector<std::string> sfo_keys = sfo::get_keys(sfo_data);

  if (!std::count(sfo_keys.begin(), sfo_keys.end(), std::string("CONTENT_ID"))) {
    FATAL_ERROR("param.sfo does not contain `CONTENT_ID`!");
  }

  std::vector<sfo::SfoPubtoolinfoIndex> pubtool_data = sfo::read_pubtool_data(sfo_data);
  std::vector<std::string> pubtool_keys;
  if (std::count(sfo_keys.begin(), sfo_keys.end(), std::string("PUBTOOLINFO"))) {
    pubtool_keys = sfo::get_pubtool_keys(pubtool_data);
  }

  std::vector<unsigned char> temp_content_id = sfo::get_value("CONTENT_ID", sfo_data);
  std::string content_id(temp_content_id.begin(), temp_content_id.end());

  std::string c_date; // "YYYY-MM-DD"
  if (std::count(pubtool_keys.begin(), pubtool_keys.end(), std::string("c_date"))) {
    try {
      c_date = sfo::get_pubtool_value("c_date", pubtool_data);
    } catch (...) {
    } // get_pubtool_value throws if key is not found but we don't care and c_date will remain empty
  }

  std::string c_time; // "XXXXXX"
  if (std::count(pubtool_keys.begin(), pubtool_keys.end(), std::string("c_time"))) {
    try {
      c_time = sfo::get_pubtool_value("c_time", pubtool_data);
    } catch (...) {
    } // get_pubtool_value throws if key is not found but we don't care and c_time will remain empty
  }

  // Get content type string for GP4
  std::string content_type;
  if (type == "base") {
    content_type = "pkg_ps4_app";
  } else if (type == "patch") {
    content_type = "pkg_ps4_patch";
  } else if (type == "remaster") {
    content_type = "pkg_ps4_remaster";
  } else if (type == "theme") {
    content_type = "pkg_ps4_theme";
  } else if (type == "theme-unlock") {
    content_type = "pkg_ps4_ac_nodata"; // Use "pkg_ps4_ac_nodata" as the GP4 is only used for the "unlock" PKG in this case
  } else if (type == "additional-content-data") {
    content_type = "pkg_ps4_ac_data";
  } else if (type == "additional-content-no-data") {
    content_type = "pkg_ps4_ac_nodata";
  }

  // Generate actual GP4 file
  pugi::xml_document volume_xml = make_volume(content_id, content_type, c_date, c_time);
  pugi::xml_document files_xml = make_files(output_path, self_files);
  pugi::xml_document directories_xml = make_directories(output_path);
  pugi::xml_document playgo_xml;
  pugi::xml_document assembled_xml;

  std::filesystem::path playgo_xml_path(output_path);
  playgo_xml_path /= "sce_sys";
  playgo_xml_path /= "playgo-manifest.xml";
  if (std::filesystem::is_regular_file(playgo_xml_path)) {
    playgo_xml = make_playgo(playgo_xml_path);
    assembled_xml = assemble(volume_xml, playgo_xml, files_xml, directories_xml);
  } else {
    assembled_xml = assemble(volume_xml, playgo_xml, files_xml, directories_xml, "1000"); // TODO: Version number?
  }

  write(assembled_xml, gp4_path);
}
} // namespace gp4
