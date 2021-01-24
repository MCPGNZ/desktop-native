#include "desktop.h"

#include <iostream>
#include <system_error>
#include <string>
#include <map>
#include <stdlib.h>

#include <atlbase.h>

#include "framework.h"

/* ========= utility ========= */
static const LPCSTR progman_id = "Progman";
static const LPCSTR def_view_id = "SHELLDLL_DefView";
static const LPCSTR sys_list_id = "SysListView32";

HWND desktop_handle;
HANDLE desktop_process;
IShellFolder* desktop_shell_folder;
IFolderView2* desktop_folder_view;

static item_data* desktop_items;

HWND query_desktop_handle()
{
    const auto progman = FindWindow(progman_id, NULL);
    log(progman, "FindWindow(progman_id, NULL)");

    const auto def_view = FindWindowEx(progman, 0, def_view_id, NULL);
    log(def_view, "FindWindowEx(progman, 0, def_view_id, NULL)");

    const auto sys_list = FindWindowEx(def_view, 0, sys_list_id, NULL);
    log(sys_list, "FindWindowEx(def_view,  0, sys_list_id, NULL)");

    return sys_list;
}
HANDLE query_desktop_process(HWND handle)
{
    DWORD processID = 0;

    GetWindowThreadProcessId(handle, &processID);
    log(processID != 0, "GetWindowThreadProcessId(handle, &processID)");

    auto process = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, false, processID);
    log(process != 0, "OpenProcess");

    return process;
}
IFolderView2* query_desktop_folder_view()
{
    CComPtr<IShellWindows> shell_windows;
    CComPtr<IDispatch> dispatch;
    CComPtr<IShellBrowser> shell_browser;
    CComPtr<IShellView> shell_view;
    IFolderView2* folder_view;

    auto hr = shell_windows.CoCreateInstance(CLSID_ShellWindows);
    log(hr, "spShellWindows.CoCreateInstance(CLSID_ShellWindows)");

    CComVariant vtLoc(CSIDL_DESKTOP);
    CComVariant vtEmpty;
    long lhwnd;

    hr = shell_windows->FindWindowSW(
        &vtLoc, &vtEmpty,
        SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &dispatch);
    log(hr, "shell_windows->FindWindowSW");

    hr = CComQIPtr<IServiceProvider>(dispatch)->
        QueryService(SID_STopLevelBrowser,
            IID_PPV_ARGS(&shell_browser));
    log(hr, "QueryService(SID_STopLevelBrowser)");

    hr = shell_browser->QueryActiveShellView(&shell_view);
    log(hr, "QueryActiveShellView");

    hr = shell_view->QueryInterface(IID_IFolderView2, (void**)&folder_view);
    log(hr, "QueryInterface(IID_PPV_ARGS(&folder_view)");

    return folder_view;
}
IShellFolder* query_desktop_shell_folder()
{
    IShellFolder* shell_folder;
    auto hr = SHGetDesktopFolder(&shell_folder);
    log(hr, "SHGetDesktopFolder");

    return shell_folder;
}

/* =========== dll =========== */
void desktop_initialize()
{
    desktop_handle = query_desktop_handle();
    desktop_process = query_desktop_process(desktop_handle);
    desktop_folder_view  = query_desktop_folder_view();
    desktop_shell_folder = query_desktop_shell_folder();
}

int desktop_folders_count()
{
    int count;
    auto hr = desktop_folder_view->ItemCount(0, &count);
    log(hr, "folder_view->ItemCount(0, &count)");

    return count;
}
void desktop_folders(item_data* items)
{
    desktop_items = items;

    int count = desktop_folders_count();
    for (int i = 0; i < count; ++i)
    {
        CComHeapPtr<ITEMIDLIST> pidl;
        auto hr = desktop_folder_view->Item(i, &pidl);
        log(hr, "folder_view->Item(i, &pidl)");

        /* todo: make shure that item_data releases this */
        IShellItem* item;

        /* todo: make shure that item_data releases this */
        LPWSTR name;

        hr = SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&item));
        log(hr, "item->GetAttributes(SFGAO_FILESYSTEM, &attributes)");

        hr = item->GetDisplayName(SIGDN_NORMALDISPLAY, &name);
        log(hr, "item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)");

        POINT position;
        hr = desktop_folder_view->GetItemPosition(pidl, &position);
        log(hr, "desktop_folder_view->GetItemPosition");

        item_data current {};
        current.name = name;
        current.item = item;
        current.x = position.x;
        current.y = position.y;

        *(items + i) = current;
    }
}