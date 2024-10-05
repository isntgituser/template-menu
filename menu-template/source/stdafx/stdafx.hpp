#pragma once

#include <windows.h>

#include <unordered_map>
#include <functional>
#include <iostream>
#include <format>

#include <d3d11.h>
#include <d3dx11.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <imgui_freetype.h>

#include <cpr\cpr.h>

#ifdef _DEBUG
#define DEBUG_MSG( str, ... ) std::cout << "[DBG]: " << std::vformat( str, std::make_format_args( __VA_ARGS__ ) ) << std::endl
#else
#define DEBUG_MSG( ... )
#endif