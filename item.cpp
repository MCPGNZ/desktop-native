#include "item.h"
#include <Shlobj.h>
#include <string>

/* https://stackoverflow.com/questions/16945892/custom-folder-icons-with-desktop-ini-instant-refreshing/19440996#19440996 */
void set_icon(LPWSTR absolute_path, LPWSTR iconPath)
{
    SHFOLDERCUSTOMSETTINGS fcs = {0};
    fcs.dwSize = sizeof(SHFOLDERCUSTOMSETTINGS);
    fcs.dwMask = FCSM_LOGO | FCSM_ICONFILE;
    fcs.pszIconFile = iconPath;
    fcs.cchIconFile = 0;
    fcs.iIconIndex = 0;
    SHGetSetFolderCustomSettings(&fcs, absolute_path, FCS_FORCEWRITE);
}

void set_tooltip(LPWSTR absolute_path, LPWSTR tooltip)
{
    SHFOLDERCUSTOMSETTINGS fcs = {0};
    fcs.dwSize = sizeof(SHFOLDERCUSTOMSETTINGS);
    fcs.dwMask = FCSM_INFOTIP;
    fcs.pszInfoTip = tooltip;
    fcs.cchInfoTip = 0;
    SHGetSetFolderCustomSettings(&fcs, absolute_path, FCS_FORCEWRITE);
}