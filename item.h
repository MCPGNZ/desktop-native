#pragma once
#include <wtypes.h>

#ifdef DESKTOP_FRAMEWORK_EXPORTS
#define DESKTOP_FRAMEWORK_API __declspec(dllexport)
#else
#define DESKTOP_FRAMEWORK_API __declspec(dllimport)
#endif

extern "C" DESKTOP_FRAMEWORK_API  void set_icon(LPWSTR absolute_path, LPWSTR iconPath);
extern "C" DESKTOP_FRAMEWORK_API  void set_tooltip(LPWSTR absolute_path, LPWSTR tooltip);
