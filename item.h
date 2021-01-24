#pragma once
#include <Windows.h>
#include <shlobj.h>

#ifdef DESKTOP_FRAMEWORK_EXPORTS
#define DESKTOP_FRAMEWORK_API __declspec(dllexport)
#else
#define DESKTOP_FRAMEWORK_API __declspec(dllimport)
#endif

struct item_data
{
    LPCWSTR name;
    IShellItem* item;
    int x;
    int y;
};

extern "C" DESKTOP_FRAMEWORK_API  void item_name_get(item_data * token);

extern "C" DESKTOP_FRAMEWORK_API  void item_update_name(item_data * token);
extern "C" DESKTOP_FRAMEWORK_API  void item_update_position(item_data * token);