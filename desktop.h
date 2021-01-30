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

extern HWND desktop_handle;
extern HANDLE desktop_process;
extern IShellFolder* desktop_shell_folder;
extern IFolderView2* desktop_folder_view;

extern "C" DESKTOP_FRAMEWORK_API  void desktop_initialize();

extern "C" DESKTOP_FRAMEWORK_API  int desktop_get_item_indices2(LPWSTR * absolute_paths);
extern "C" DESKTOP_FRAMEWORK_API  void desktop_set_item_position2(int index, int x, int y);

extern "C" DESKTOP_FRAMEWORK_API  void desktop_get_item_position(LPWSTR absolute_path, int* x, int* y);
extern "C" DESKTOP_FRAMEWORK_API  void desktop_set_item_position(LPWSTR absolute_path, int x, int y);

extern "C" DESKTOP_FRAMEWORK_API bool desktop_get_style(DWORD flag);
extern "C" DESKTOP_FRAMEWORK_API void desktop_set_style(DWORD flag, bool state);

extern "C" DESKTOP_FRAMEWORK_API void desktop_get_icon_size(FOLDERVIEWMODE * folderViewMode, int* iconSize);
extern "C" DESKTOP_FRAMEWORK_API void desktop_set_icon_size(FOLDERVIEWMODE folderViewMode, int iconSize);
