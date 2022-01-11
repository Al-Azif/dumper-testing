// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#include "dump.hpp"

#include "common.hpp"
#include "elf.hpp"
#include "fself.hpp"
#include "gp4.hpp"
#include "npbind.hpp"
#include "pfs.hpp"
#include "pkg.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace dump {
void __dump(const std::string &usb_device, const std::string &title_id, const std::string &type) {
  if (type != "base" && type != "patch" && type != "remaster" && type != "theme" && type != "theme-unlock" && type != "additional-content-data" && type != "additional-content-no-data") {
    FATAL_ERROR("Unknown asset type");
  }

  // TID vs CID and type checks
  if (type == "base" || type == "patch" || type == "remaster") {
    if (!std::regex_match(title_id, std::regex("^\\D{4}\\d{5}$"))) {
      FATAL_ERROR("Malformed title ID");
    }
  } else {
    if (!std::regex_match(title_id, std::regex("^\\D{2}\\d{4}-\\D{4}\\d{5}_\\d{2}-\\w{16}$"))) {
      FATAL_ERROR("Malformed content ID");
    }
  }

  std::string output_directory = title_id;
  if (type == "patch") {
    output_directory += "-patch"; // Add "-patch" to "patch" types so it doesn't overlap with "base"
  } else if (type == "theme-unlock") {
    output_directory += "-unlock"; // Add "-unlock" to theme type so we can differentiate between "install" and "unlock" PKGs
  }

  // Create base path
  std::filesystem::path output_path(usb_device);
  output_path /= output_directory;

  // Make sure the base path is a directory or can be created
  if (!std::filesystem::is_directory(output_path) && !std::filesystem::create_directories(output_path)) {
    FATAL_ERROR("Unable to create output directory");
  }

  // Check for .dumping semaphore
  std::filesystem::path dumping_semaphore(usb_device);
  dumping_semaphore /= output_directory + ".dumping";
  if (std::filesystem::exists(dumping_semaphore)) {
    FATAL_ERROR("This dump is currently dumping or closed unexpectedly! Please delete existing dump to enable dumping.");
  }

  // Check for .complete semaphore
  std::filesystem::path complete_semaphore(usb_device);
  complete_semaphore /= output_directory + ".complete";
  if (std::filesystem::exists(complete_semaphore)) {
    FATAL_ERROR("This dump has already been completed! Please delete existing dump to enable dumping.");
  }

  // Create .dumping semaphore
  std::ofstream dumping_sem_touch(dumping_semaphore);
  dumping_sem_touch.close();
  if (std::filesystem::exists(dumping_semaphore)) {
    FATAL_ERROR("Unable to create dumping semaphore!");
  }

  // Create "sce_sys" directory in the output directory
  std::filesystem::path sce_sys_path(output_path);
  sce_sys_path /= "sce_sys";
  if (!std::filesystem::is_directory(sce_sys_path) && !std::filesystem::create_directories(sce_sys_path)) {
    FATAL_ERROR("Unable to create `sce_sys` directory");
  }

  if (type == "theme-unlock" || type == "additional-content-no-data") {
    std::filesystem::path param_destination(sce_sys_path);
    param_destination /= "param.sfo";

    if (type == "theme-unlock") {
      // Copy theme install PKG
      std::filesystem::path install_source("/user/addcont/I00000002");
      install_source /= title_id;
      install_source /= "ac.pkg";

      std::filesystem::path install_destination(usb_device);
      install_destination /= title_id + "-install.pkg";

      // Copy param.sfo
      if (!std::filesystem::copy_file(install_source, install_destination, std::filesystem::copy_options::overwrite_existing)) {
        FATAL_ERROR("Unable to copy" + std::string(install_source) + " to " + std::string(install_destination));
      }

      std::filesystem::path param_source("/system_data/priv/appmeta/addcont/I00000002");
      param_source /= title_id;
      param_source /= "param.sfo";

      if (!std::filesystem::copy_file(param_source, param_destination, std::filesystem::copy_options::overwrite_existing)) {
        FATAL_ERROR("Unable to copy" + std::string(param_source) + " to " + std::string(param_destination));
      }
    } else {
      // TODO: Find and copy... or create... "param.sfo" for "additional-content-no-data" at param_destination
    }
  } else { // "base", "patch", "remaster", "theme", "additional-content-data"
    // UnPKG
    std::filesystem::path pkg_directory_path;
    pkg_directory_path /= "user";
    if (type == "base" || type == "remaster") {
      pkg_directory_path /= "app";
      pkg_directory_path /= title_id;
      pkg_directory_path /= "app.pkg";
    } else if (type == "patch") {
      pkg_directory_path /= "patch";
      pkg_directory_path /= title_id;
      pkg_directory_path /= "patch.pkg";
    } else if (type == "theme") {
      pkg_directory_path /= "addcont/I00000002";
      pkg_directory_path /= title_id;
      pkg_directory_path /= "ac.pkg";
    } else if (type == "additional-content-data") {
      pkg_directory_path /= "addcont";
      // This regex will match because of the checks at the beginning of the function
      std::smatch match;
      std::regex_search(title_id, match, std::regex("^\\D{2}\\d{4}-(\\D{4}\\d{5})_\\d{2}-(\\w{16})$"));
      pkg_directory_path /= match.str(1);
      pkg_directory_path /= match.str(2);
      pkg_directory_path /= "ac.pkg";
    }

    // Detect if on extended storage and make pkg_path
    std::filesystem::path pkg_path;
    std::filesystem::path ext_path("/mnt/ext0/");
    ext_path /= pkg_directory_path;
    if (std::filesystem::exists(ext_path) && std::filesystem::is_regular_file(ext_path)) {
      pkg_path = ext_path;
    } else {
      pkg_path = pkg_directory_path;
    }

    pkg::extract_sc0(pkg_path, sce_sys_path);

    // Trophy "decryption"
    std::filesystem::path npbind_file(sce_sys_path);
    npbind_file /= "npbind.dat";
    if (std::filesystem::is_regular_file(npbind_file)) {
      std::vector<npbind::NpBindEntry> npbind_entries = npbind::read(npbind_file); // Flawfinder: ignore

      for (auto &&entry : npbind_entries) {
        std::filesystem::path src("/user/trophy/conf");
        src /= std::string(entry.npcommid.data);
        src /= "TROPHY.TRP";

        std::filesystem::path dst(sce_sys_path);
        dst /= "trophy";
        uint32_t zerofill = 0;
        if (std::strlen(entry.trophy_number.data) < 2) {        // Flawfinder: ignore
          zerofill = 2 - std::strlen(entry.trophy_number.data); // Flawfinder: ignore
        }
        dst /= "trophy" + std::string(zerofill, '0') + std::string(entry.trophy_number.data) + ".trp";

        if (!std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing)) {
          FATAL_ERROR("Unable to copy" + std::string(src) + " to " + std::string(dst));
        }
      }
    }

    // UnPFS
    std::filesystem::path pfs_path("/mnt/sandbox/pfsmnt");
    pfs_path /= title_id;
    if (type == "base" || type == "remaster") {
      pfs_path += "-app0-nest";
    } else if (type == "patch") {
      pfs_path += "-patch0-nest";
    } else if (type == "theme" || type == "additional-content-data") {
      pfs_path += "-ac-nest";
    }
    pfs_path /= "pfs_image.dat";

    pfs::extract(pfs_path, output_path);
  }

  // Vector of strings for locations of SELF files for decryption
  std::vector<std::string> self_files;

  // Generate GP4
  std::filesystem::path sfo_path(sce_sys_path);
  sfo_path /= "param.sfo";

  std::filesystem::path gp4_path(output_path);
  gp4_path /= output_directory + ".gp4";

  gp4::generate(sfo_path, output_path, gp4_path, self_files, type);

  // Decrypt ELF files and make into FSELFs
  for (auto &&entry : self_files) {
    std::filesystem::path encrypted_path("/mnt/sandbox/pfsmnt");
    if (type == "base") {
      encrypted_path /= title_id + "-app0";
    } else if (type == "patch") {
      encrypted_path /= title_id + "-patch0";
    } else if (type == "theme-unlock") {
      encrypted_path /= title_id + "-ac";
    }

    std::filesystem::path decrypted_path(output_path);
    decrypted_path /= entry;

    std::filesystem::path fself_path(decrypted_path);
    fself_path.replace_extension(".fself");

    if (fself::is_fself(encrypted_path)) {
      if (!std::filesystem::copy_file(encrypted_path, fself_path, std::filesystem::copy_options::overwrite_existing)) {
        FATAL_ERROR("Unable to copy" + std::string(encrypted_path) + " to " + std::string(fself_path));
      }
    } else {
      // Get proper Program Authority ID, App Version, Firmware Version, and Auth Info
      uint64_t program_authority_id = elf::get_paid(encrypted_path);
      std::string ptype = "fake"; // elf::get_ptype(encrypted_path);
      uint64_t app_version = elf::get_app_version(encrypted_path);
      uint64_t fw_version = elf::get_fw_version(encrypted_path);
      std::vector<unsigned char> auth_info = elf::get_auth_info(encrypted_path);

      elf::decrypt(encrypted_path, decrypted_path);
      if (!elf::is_valid_decrypt(encrypted_path, decrypted_path)) {
        FATAL_ERROR("Invalid ELF decryption!");
      }
      elf::zero_section_header(decrypted_path);
      fself::make_fself(decrypted_path, fself_path, program_authority_id, ptype, app_version, fw_version, auth_info);
    }
  }

  // Delete .dumping semaphore
  if (!std::filesystem::remove(dumping_semaphore)) {
    FATAL_ERROR("Unable to delete dumping semaphore");
  }

  // Create .complete semaphore
  std::ofstream complete_sem_touch(complete_semaphore);
  complete_sem_touch.close();
  if (std::filesystem::exists(complete_semaphore)) {
    FATAL_ERROR("Unable to create completion semaphore!");
  }
}

void dump_base(const std::string &usb_device, const std::string &title_id) {
  __dump(usb_device, title_id, "base");
}

void dump_patch(const std::string &usb_device, const std::string &title_id) {
  __dump(usb_device, title_id, "patch");
}

void dump_remaster(const std::string &usb_device, const std::string &title_id) {
  __dump(usb_device, title_id, "remaster");
}

void dump_theme(const std::string &usb_device, const std::string &theme_id) {
  __dump(usb_device, theme_id, "theme");
}

void dump_theme_unlock(const std::string &usb_device, const std::string &theme_id) {
  __dump(usb_device, theme_id, "theme-unlock");
}

void dump_ac(const std::string &usb_device, const std::string &ac_id) {
  __dump(usb_device, ac_id, "additional-content-data");
}

void dump_ac_no_data(const std::string &usb_device, const std::string &ac_id) {
  __dump(usb_device, ac_id, "additional-content-no-data");
}
} // namespace dump
