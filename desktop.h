#pragma once
#include "item.h"

#include <comdef.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <wtypes.h>
#include <atlcomcli.h>

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

extern "C" DESKTOP_FRAMEWORK_API  int desktop_folders_count();
extern "C" DESKTOP_FRAMEWORK_API  void desktop_folders(item_data * items);
