#include "framework.h"

#include <iostream>
#include <shlobj.h>

static CallbackInfo framework_callback_info;
static CallbackError framework_callback_error;

void log(const bool result, const std::string& message)
{
    if (result == true)
    {
        framework_callback_info(message.c_str());
    }
    else
    {
        framework_callback_error(message.c_str());
    }
}
void log(const HRESULT hr, const std::string& message)
{
    TCHAR* error_message;
    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&error_message, 0, NULL) != 0)
    {
        log(hr == S_OK, message);
        log(hr == S_OK, "HRESULT: " + std::string(error_message));
        LocalFree(error_message);
    }
    else
    {
        log(hr == S_OK, message);
    }
}
void log(const HWND hwnd, const std::string& message)
{
    log(hwnd != nullptr, message);
}

void framework_initialize(CallbackInfo callbackInfo, CallbackError callbackError)
{
    framework_callback_info = callbackInfo;
    framework_callback_error = callbackError;

    auto hr = CoInitialize(NULL);
    log(hr, " CoInitialize(NULL)");
}
void framework_cleanup()
{
    CoUninitialize();
}