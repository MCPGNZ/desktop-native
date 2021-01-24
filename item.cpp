#include "item.h"

#include <string>
#include <atlbase.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <filesystem>

#include "framework.h"
#include "desktop.h"

void item_rename(item_data* token, LPWSTR name)
{
    SFGAOF attributes;
    auto hr = token->item->GetAttributes(SFGAO_FILESYSTEM, &attributes);
    log(hr, "item->GetAttributes(SFGAO_FILESYSTEM, &attributes)");

    if ((attributes & SFGAO_FILESYSTEM) != 0)
    {
        CComHeapPtr<WCHAR> path;
        hr = token->item->GetDisplayName(SIGDN_FILESYSPATH, &path);
        log(hr, "item->GetDisplayName(SIGDN_FILESYSPATH, &name)");

        if (!path) { return; }

        const auto original_path = std::wstring(path);
        const auto directory = std::filesystem::path(original_path).parent_path();
        const auto new_path = std::wstring(directory) + L"//" + std::wstring(name);

        CoTaskMemFree(path);

        /* think about: MoveFileExW with MOVEFILE_WRITE_THROUGH to block? */
        //const auto r =  MoveFileW(original_path.c_str(), new_path.c_str());
       // log(r != FALSE, "MoveFileW((LPCWSTR)path, (LPCWSTR)result)");

        CComPtr<ITransferSource> transfer_source;
        hr = token->item->BindToHandler(nullptr, BHID_Transfer, IID_PPV_ARGS(&transfer_source));
        log(hr, "item->BindToHandler(BHID_Transfer)");

        CComPtr<IShellItem> parent;
        hr =  token->item->GetParent(&parent);
        log(hr, "item->GetParent(&parent)");

        IShellItem* result;
        hr = transfer_source->RenameItem(token->item, name, TSF_NORMAL, &result);
        log(hr, "transfer_source->RenameItem");

        token->item = result;
        token->name = name;
        return;
    }
    log(false, "item_rename: no SFGAO_FILESYSTEM for item, cannot rename");
}

void item_update_position(item_data* token)
{
    POINT position;
    position.x = token->x;
    position.y = token->y;

    CComHeapPtr<ITEMIDLIST_ABSOLUTE> pidl;
    auto hr = CComQIPtr<IPersistIDList>(token->item)->GetIDList(&pidl);
    log(hr, "CComQIPtr<IPersistIDList>(token->item)->GetIDList(&pidl)");

    LPCITEMIDLIST pidlArr = {pidl};

    hr = desktop_folder_view->SelectAndPositionItems(1, &pidlArr, &position, SVSI_NOSTATECHANGE);
    log(hr, "desktop_folder_view->SelectAndPositionItems");
}