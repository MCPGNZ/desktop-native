#pragma once

#ifdef DESKTOP_FRAMEWORK_EXPORTS
#define DESKTOP_FRAMEWORK_API __declspec(dllexport)
#else
#define DESKTOP_FRAMEWORK_API __declspec(dllimport)
#endif

typedef void(_stdcall* CallbackInfo)(const char*);
typedef void(_stdcall* CallbackError)(const char*);
