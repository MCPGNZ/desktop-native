#pragma once
#include "item.h"

#include <comdef.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <wtypes.h>

#ifdef DESKTOP_FRAMEWORK_EXPORTS
#define DESKTOP_FRAMEWORK_API __declspec(dllexport)
#else
#define DESKTOP_FRAMEWORK_API __declspec(dllimport)
#endif

struct Item
{
    LPWSTR absolute_path;
    int x;
    int y;
};

extern HWND desktop_handle;
extern HANDLE desktop_process;
extern IShellFolder* desktop_shell_folder;
extern IFolderView2* desktop_folder_view;

extern "C" DESKTOP_FRAMEWORK_API  void desktop_initialize();

extern "C" DESKTOP_FRAMEWORK_API  void desktop_get_item_position(LPWSTR absolute_path, int* x, int* y);
extern "C" DESKTOP_FRAMEWORK_API  void desktop_set_item_position(LPWSTR absolute_path, int x, int y);
extern "C" DESKTOP_FRAMEWORK_API  void desktop_set_item_positions(Item * items, long num_items);

extern "C" DESKTOP_FRAMEWORK_API bool desktop_get_autoarrange();
extern "C" DESKTOP_FRAMEWORK_API void desktop_set_autoarrange(bool state);

extern "C" DESKTOP_FRAMEWORK_API bool desktop_get_gridallign();
extern "C" DESKTOP_FRAMEWORK_API void desktop_set_gridallign(bool state);
