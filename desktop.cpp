#include "desktop.h"

#include <iostream>
#include <system_error>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>

#include <atlbase.h>

#include "framework.h"

/* ========= utility ========= */
static const LPCSTR progman_id = "Progman";
static const LPCSTR workerw_id = "WorkerW";
static const LPCSTR def_view_id = "SHELLDLL_DefView";
static const LPCSTR sys_list_id = "SysListView32";

HWND desktop_handle;
HANDLE desktop_process;
IShellFolder* desktop_shell_folder;
IFolderView2* desktop_folder_view;

std::string join(const std::vector<std::string>::const_iterator begin, const std::vector<std::string>::const_iterator end, const std::string& separator) {
    if (begin == end) {
        return "";
    }

    std::stringstream ss;
    ss << *begin;
    for (auto it = begin + 1; it != end; ++it) {
        ss << separator << *it;
    }

    return ss.str();
}

std::string join(const std::vector<std::string>& segments, const std::string& separator) {
    return join(segments.begin(), segments.end(), separator);
}

HWND find_window_by_class_path(const std::vector<std::string>::const_iterator begin, const std::vector<std::string>::const_iterator end, const HWND root = GetDesktopWindow()) {
    if (begin == end) {
        return root;
    }

    HWND child = NULL;
    while ((child = FindWindowEx(root, child, begin->c_str(), NULL)) != NULL) {
        log(true, "try find " + join(begin, end, "/") + " under " + std::to_string((uint64_t)child));
        HWND result = find_window_by_class_path(begin + 1, end, child);
        if (result != NULL) {
            return result;
        }
    }

    return NULL;
}

HWND find_window_by_class_path(const std::vector<std::string>& path) {
    HWND hwnd = find_window_by_class_path(path.begin(), path.end());
    log(hwnd, GetLastError(), "find_window_by_class_path " + join(path, "/"));
    return hwnd;
}

HWND query_desktop_handle()
{
    HWND desktop = find_window_by_class_path({progman_id, def_view_id, sys_list_id});
    if (desktop != NULL) {
        return desktop;
    }
    return find_window_by_class_path({workerw_id, def_view_id, sys_list_id});
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

LPITEMIDLIST to_relative_pidl(LPWSTR absolute_path)
{
    CComPtr<IShellItem> item;
    auto hr = SHCreateItemFromParsingName(absolute_path, nullptr, IID_PPV_ARGS(&item));
    log(hr, "SHCreateItemFromParsingName(absolute_path, nullptr, IID_PPV_ARGS(&item))");

    CComHeapPtr<WCHAR> relative_path;
    hr = item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &relative_path);
    log(hr, "GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &relative_path)");

    return ILCreateFromPathW(relative_path);
}
LPITEMIDLIST to_absolute_pidl(LPWSTR absolute_path)
{
    return ILCreateFromPathW(absolute_path);
}

void desktop_get_item_position(LPWSTR absolute_path, int* x, int* y)
{
    CComHeapPtr<ITEMIDLIST> pidl {to_relative_pidl(absolute_path)};
    POINT point;

    auto hr = desktop_folder_view->GetItemPosition(pidl, &point);
    log(hr, "desktop_folder_view->GetItemPosition");

    (*x) = point.x;
    (*y) = point.y;
}
void desktop_set_item_position(LPWSTR absolute_path, int x, int y)
{
    LPCITEMIDLIST pidl = to_relative_pidl(absolute_path);
    POINT point;
    point.x = x;
    point.y = y;

    auto hr = desktop_folder_view->SelectAndPositionItems(1, &pidl, &point, SVSI_NOSTATECHANGE);
    log(hr, "desktop_folder_view->SelectAndPositionItems");
}
void desktop_set_item_positions(Item* items, long num_items)
{
    std::vector<LPCITEMIDLIST> pidls;
    pidls.reserve(num_items);

    std::vector<POINT> points;
    points.reserve(num_items);

    for (long i = 0; i < num_items; ++i) {
        pidls.push_back(to_relative_pidl(items[i].absolute_path));
        points.emplace_back(items[i].x, items[i].y);
    }

    auto hr = desktop_folder_view->SelectAndPositionItems(num_items, pidls.data(), points.data(), SVSI_NOSTATECHANGE);
    log(hr, "desktop_folder_view->SelectAndPositionItems");
}

bool desktop_get_style(DWORD flag)
{
    DWORD flags = 0;
    desktop_folder_view->GetCurrentFolderFlags(&flags);
    return (flags & flag) != 0;
}
void desktop_set_style(DWORD flag, bool state)
{
    DWORD flags = 0;
    desktop_folder_view->GetCurrentFolderFlags(&flags);
    desktop_folder_view->SetCurrentFolderFlags(flag, state ? flag : ~flag);
}