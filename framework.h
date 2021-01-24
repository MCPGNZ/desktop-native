#pragma once
#include <string>

#include <windows.h>
#include <winerror.h>

#include "Callbacks.h"

#ifdef DESKTOP_FRAMEWORK_EXPORTS
#define DESKTOP_FRAMEWORK_API __declspec(dllexport)
#else
#define DESKTOP_FRAMEWORK_API __declspec(dllimport)
#endif

void log(const bool result, const std::string& message);
void log(const HRESULT hr, const std::string& message);
void log(const HWND hwnd, const std::string& message);

extern "C" DESKTOP_FRAMEWORK_API void framework_initialize(CallbackInfo, CallbackError);
extern "C" DESKTOP_FRAMEWORK_API void framework_cleanup();
