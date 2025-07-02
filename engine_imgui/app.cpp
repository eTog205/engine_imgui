module;
#include <imgui.h>
#include <tchar.h>
#include <wtypes.h>

module app;

import imgui_core;
import thongso_engine;
import vulkan;

import std;

namespace
{
	std::string tên_cửasổ = "zitapp";

	int chiềurộng_mặcđịnh_cửasổ          = 1280;
	int chiềucao_mặcđịnh_cửasổ           = 720;
	int chiềurộng_tốithiểu_mặcđịnh_cửasổ = 1280;
	int chiềucao_tốithiểu_mặcđịnh_cửasổ  = 720;
	int tọađộx_cửasổ_mặcđịnh             = 0;
	int tọađộy_cửasổ_mặcđịnh             = 0;
} // namespace

HWND      g_cuaso    = nullptr;
HINSTANCE g_instance = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SIZE:
			ts.yeucau_resize = true;
			break;
		case WM_PAINT:
			if (ImGui::GetCurrentContext() && !IsIconic(hwnd))
				xuly_frame_vulkan();
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_GETMINMAXINFO:
		{
			auto mmi              = (MINMAXINFO*)lParam;
			mmi->ptMinTrackSize.x = chiềurộng_tốithiểu_mặcđịnh_cửasổ;
			mmi->ptMinTrackSize.y = chiềucao_tốithiểu_mặcđịnh_cửasổ;
			return 0;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

std::wstring convert_to_wstring(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

bool khoitao_ungdung()
{
	g_instance = GetModuleHandle(nullptr);

	WNDCLASSEX wc    = {};
	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = g_instance;
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = _T("ZitAppWindowClass");

	if (!RegisterClassEx(&wc))
		return false;

	int chieurong = chiềurộng_mặcđịnh_cửasổ;
	int chieucao  = chiềucao_mặcđịnh_cửasổ;

	int chieurong_manhinh = GetSystemMetrics(SM_CXSCREEN);
	int chieucao_manhinh  = GetSystemMetrics(SM_CYSCREEN);

	int x = (chieurong_manhinh - chieurong) / 2;
	int y = (chieucao_manhinh - chieucao) / 2;

	g_cuaso = CreateWindowEx(0,
	                         wc.lpszClassName,
	                         convert_to_wstring(tên_cửasổ).c_str(),
	                         WS_OVERLAPPEDWINDOW,
	                         x,
	                         y,
	                         chiềurộng_mặcđịnh_cửasổ,
	                         chiềucao_mặcđịnh_cửasổ,
	                         nullptr,
	                         nullptr,
	                         g_instance,
	                         nullptr);
	if (!g_cuaso)
	{
		std::cerr << "❌ Không tạo được cửa sổ Win32\n";
		return false;
	}

	ShowWindow(g_cuaso, SW_SHOW);
	UpdateWindow(g_cuaso);

	if (!khoitao_vulkan(g_cuaso))
		return false;

	cauhinh_imgui ch_ig{.kichthuoc = 24.0f, .thumuc_font = ""};

	khoitao_imgui(g_cuaso, co_imgui::ko);
	return true;
}

void vonglap_ungdung()
{
	MSG msg;
	while (true)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!IsIconic(g_cuaso))
			xuly_frame_vulkan();
	}
}

void dondep_ungdung()
{
	dondep_imgui();
	dondep_vulkan();
}
