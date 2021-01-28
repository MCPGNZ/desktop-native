#include "framework.h"

#include <iostream>
#include <optional>
#include <sstream>
#include <shlobj.h>

static CallbackInfo framework_callback_info;
static CallbackError framework_callback_error;

std::optional<std::string> format_error(DWORD error) {
    // Workaround for ExecutionEngineException: String conversion error: Illegal byte sequence encounted in the input.
    return std::nullopt;

    TCHAR* error_message;
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&error_message, 0, NULL);
    if (size == 0) {
        return std::nullopt;
    }

	std::string result(error_message);
	LocalFree(error_message);
	return error_message;
}

std::string to_hex(uint64_t num) {
    std::stringstream ss;
    ss << "0x" << std::hex << num;
    return ss.str();
}

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
    if (auto msg_opt = format_error((DWORD)hr))
    {
        log(hr == S_OK, message + ": " + msg_opt.value());
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
void log(const HWND hwnd, const DWORD error, const std::string& message)
{
    if (auto msg_opt = format_error(error))
    {
        log(error == ERROR_SUCCESS, message + " = " + to_hex((uint64_t)hwnd) + ": " + msg_opt.value());
    }
    else
    {
		log(error == ERROR_SUCCESS, message);
    }
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