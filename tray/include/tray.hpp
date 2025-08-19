#pragma once
#include <tray/components/button.hpp>
#include <tray/components/imagebutton.hpp>
#include <tray/components/label.hpp>
#include <tray/components/separator.hpp>
#include <tray/components/submenu.hpp>
#include <tray/components/syncedtoggle.hpp>
#include <tray/components/toggle.hpp>

#if defined(_WIN32)
#include <tray/core/windows/tray.hpp>
#elif defined(__linux__)
#include <tray/core/linux/tray.hpp>
#endif
