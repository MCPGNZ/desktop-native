#include "item.h"

#include <string>
#include <atlbase.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <filesystem>

#include "framework.h"
#include "desktop.h"

void item_name_get(item_data* token)
{
}

void item_update_name(item_data* token)
{
    CComPtr<ITransferSource> transfer_source;
    auto hr = token->item->BindToHandler(nullptr, BHID_Transfer, IID_PPV_ARGS(&transfer_source));
    log(hr, "item->BindToHandler(BHID_Transfer)");

    IShellItem* result;
    hr = transfer_source->RenameItem(token->item, token->name, TSF_USE_FULL_ACCESS, &result);

    /* skip successfull hresults */
    if ((hr < COPYENGINE_S_YES) && (hr > COPYENGINE_S_COLLISIONRESOLVED))
    {
        log(hr, "transfer_source->RenameItem");
    }

    token->item->Release();
    token->item = result;
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