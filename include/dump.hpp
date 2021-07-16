// Copyright (c) 2021 Al Azif
// License: GPLv3

#ifndef DUMP_HPP_
#define DUMP_HPP_

#include <iostream>

namespace dump {
void dump_base(const std::string &usb_device, const std::string &title_id);
void dump_patch(const std::string &usb_device, const std::string &title_id);
void dump_remaster(const std::string &usb_device, const std::string &title_id);
void dump_theme(const std::string &usb_device, const std::string &theme_id);
void dump_theme_unlock(const std::string &usb_device, const std::string &theme_id);
void dump_ac(const std::string &usb_device, const std::string &ac_id);
void dump_ac_no_data(const std::string &usb_device, const std::string &ac_id);
} // namespace dump

#endif // DUMP_HPP_
