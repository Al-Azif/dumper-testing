// Copyright (c) 2021-2022 Al Azif
// License: GPLv3

#ifndef DUMPER_INCLUDE_DUMP_H_
#define DUMPER_INCLUDE_DUMP_H_

#include <string>

namespace dump {
void __dump(const std::string &usb_device, const std::string &title_id, const std::string &type);
void dump_base(const std::string &usb_device, const std::string &title_id);
void dump_patch(const std::string &usb_device, const std::string &title_id);
void dump_remaster(const std::string &usb_device, const std::string &title_id);
void dump_theme(const std::string &usb_device, const std::string &theme_id);
void dump_theme_unlock(const std::string &usb_device, const std::string &theme_id);
void dump_ac(const std::string &usb_device, const std::string &ac_id);
void dump_ac_no_data(const std::string &usb_device, const std::string &ac_id);
} // namespace dump

#endif // DUMPER_INCLUDE_DUMP_H_
