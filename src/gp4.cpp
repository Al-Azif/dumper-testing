// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#include "gp4.h"

#include <time.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
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

  std::vector<std::string> skip_directories = {
      // "sce_sys/about" // Cannot be included for official PKG tools
  };

  for (auto &&p : std::filesystem::directory_iterator(path)) {
    // Skip files/directories contained in the `skip_directories` vector
    if (std::count(skip_directories.begin(), skip_directories.end(), std::filesystem::relative(p.path(), path))) {
      continue;
    }

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
  if (!std::regex_match(content_id, std::regex("^[A-Z]{2}\\d{4}-[A-Z]{4}\\d{5}_\\d{2}-[A-Z0-9]{16}$"))) {
    FATAL_ERROR("Malformed content ID");
  }

  // Just fucking trust me on this regex (Hits on Jan 01 1970+, but only valid dates... except leap years, Feb 29th is always assumed valid)
  if (!c_date.empty() && !std::regex_match(c_date, std::regex("^(?:19[7-9]\\d|[2-9]\\d{3})(?:02(?:0[1-9]|[12][0-9])|(?:0[469]|11)(?:0[1-9]|[12][0-9]|30)|(?:0[13578]|1[02])(?:0[1-9]|[12][0-9]|3[01]))$"))) {
    // TODO: Check for valid leap year so we get an invalid error here vs something else going wrong later when manipulating the date
    FATAL_ERROR("Malformed c_date");
  }

  if (!c_time.empty() && !std::regex_match(c_time, std::regex("^(?:[01]\\d|2[0-3])[0-5]\\d[0-5]\\d$"))) {
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

  char buffer[20]; // Flawfinder: ignore // Only used within these 4 lines
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

  // Set c_date
  std::string new_time = std::string("actual_datetime");
  if (!c_date.empty()) {
    std::string current_time_format;
    std::string new_time_format;

    if (!c_time.empty()) {
      new_time = c_date + std::string(" ") + c_time;
      current_time_format = "%Y%m%d %H%M%S";
      new_time_format = "%Y-%m-%d %H:%M:%S";
      new_time.resize(sizeof("YYYY-MM-DD HH:MM:SS"));
    } else {
      new_time = c_date;
      current_time_format = "%Y%m%d";
      new_time_format = "%Y-%m-%d";
      new_time.resize(sizeof("YYYY-MM-DD"));
    }

    struct tm tm;
    strptime(new_time.c_str(), current_time_format.c_str(), &tm);
    strftime(&new_time[0], new_time.size(), new_time_format.c_str(), &tm);
  }
  package_node.append_attribute("c_date") = new_time.c_str();

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
  psproject_node.append_attribute("version") = "1000"; // TODO: Will this be correct?

  pugi::xml_node volume_node = psproject_node.append_child("volume");

  pugi::xml_node chunk_info_node = volume_node.append_child("chunk_info");
  chunk_info_node.append_attribute("chunk_count") = "1";
  chunk_info_node.append_attribute("scenario_count") = "1";

  pugi::xml_node chunks_node = chunk_info_node.append_child("chunks");

  pugi::xml_node chunk_node = chunks_node.append_child("chunk");
  chunk_node.append_attribute("id") = "0";
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

  // Files to skip when making GP4
  std::vector<std::string> skip_files = {
      // "sce_sys/about/right.sprx", // Cannot be included for official PKG tools
      "sce_sys/.digests",
      "sce_sys/.entry_keys",
      "sce_sys/.image_key",
      "sce_sys/.unknown_0x21",
      "sce_sys/.general_digests",
      "sce_sys/.unknown_0xC0",
      "sce_sys/.metas",
      "sce_sys/.entry_names",
      "sce_sys/license.dat",
      "sce_sys/license.info",
      "sce_sys/selfinfo.dat",         // ?
      "sce_sys/imageinfo.dat",        // ?
      "sce_sys/target-deltainfo.dat", // ?
      "sce_sys/origin-deltainfo.dat",
      "sce_sys/psreserved.dat",
      "sce_sys/playgo-chunk.dat",
      "sce_sys/playgo-chunk.sha",
      "sce_sys/playgo-manifest.xml",
      "sce_sys/pubtoolinfo.dat", // ?
      "sce_sys/app/playgo-chunk.dat",
      "sce_sys/app/playgo-chunk.sha",
      "sce_sys/app/playgo-manifest.xml",
      "sce_sys/icon0.dds",
      "sce_sys/pic0.dds",
      "sce_sys/pic1.dds"};

  for (uint8_t i = 0; i < 31; i++) {
    std::stringstream ss_image;
    ss_image << "sce_sys/icon0_" << std::dec << std::setfill('0') << std::setw(2) << i << ".dds";
    skip_files.push_back(ss_image.str());
    ss_image.str(std::string());
    ss_image << "sce_sys/pic1_" << std::dec << std::setfill('0') << std::setw(2) << i << ".dds";
    skip_files.push_back(ss_image.str());
  }

  for (auto &&p : std::filesystem::recursive_directory_iterator(path)) {
    // Skip files/directories contained in the `skip_files` vector
    if (std::count(skip_files.begin(), skip_files.end(), std::filesystem::relative(p.path(), path))) {
      continue;
    }

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

      std::string orig_path_mod = orig_path.c_str();
      std::replace(orig_path_mod.begin(), orig_path_mod.end(), '/', '\\');
      file_node.append_attribute("orig_path") = orig_path_mod.c_str();
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
  psproject_node.append_attribute("fmt") = "gp4";
  if (!custom_version.empty()) {
    psproject_node.append_attribute("version") = custom_version.c_str();
  } else {
    if (playgo.child("psproject").attribute("version").empty()) {
      psproject_node.append_attribute("version") = "1000"; // TODO: Will this be correct?
    } else {
      psproject_node.append_attribute("version") = playgo.child("psproject").attribute("version").value(); // TODO: Will this be correct?
    }
  }

  psproject_node.append_copy(volume.child("volume"));
  psproject_node.child("volume").append_copy(playgo.child("psproject").child("volume").child("chunk_info"));

  psproject_node.child("volume").child("chunk_info").prepend_child("chunks");
  uint64_t chunk_count = 1;
  if (!psproject_node.child("volume").child("chunk_info").attribute("chunk_count").empty()) {
    chunk_count = std::strtoull(psproject_node.child("volume").child("chunk_info").attribute("chunk_count").value(), NULL, 10);
  }

  for (uint64_t i = 0; i < chunk_count; i++) {
    pugi::xml_node chunk_node = psproject_node.child("volume").child("chunk_info").child("chunks").append_child("chunk");
    std::stringstream ss_id;
    std::stringstream ss_label;

    ss_id << "" << std::dec << i;
    ss_label << "Chunk #" << std::dec << i; // TODO: The labels are not what they actually are, need to see if they are stored in other playgo files

    chunk_node.append_attribute("id") = ss_id.str().c_str();
    chunk_node.append_attribute("label") = ss_label.str().c_str();
  }

  // Remove `initial_chunk_count_disc` from each `scenario` if it exists... we aren't a disc anymore
  for (pugi::xml_node scenarios : psproject_node.child("volume").child("chunk_info")) {
    for (pugi::xml_node scenario : scenarios.children("scenario")) {
      scenario.remove_attribute("initial_chunk_count_disc");
    }
  }

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
