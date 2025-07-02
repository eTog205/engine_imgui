module;
#include <imgui_impl_vulkan.h>
#include <imgui_impl_win32.h>
#include <wtypes.h>

module imgui_core;

import vulkan;
import thongso_engine;
import std;

namespace fs = std::filesystem;

cauhinh_imgui ch_ig;

namespace
{
	void tai_tatca_font_trong_thumuc()
	{
		if (!fs::exists(ch_ig.thumuc_font))
		{
			ImGui::GetIO().Fonts->AddFontDefault();
			throw std::runtime_error("❌ Thư mục font [" + ch_ig.thumuc_font + "] không tồn tại");
		}

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->Clear();

		ts_eg.danhsach_font.clear();

		ImFontConfig config;
		config.OversampleH        = 3;
		config.OversampleV        = 3;
		config.RasterizerMultiply = 1.0f;

		const ImWchar* glyph_ranges = io.Fonts->GetGlyphRangesVietnamese();

		for (const auto& entry : fs::directory_iterator(ch_ig.thumuc_font))
		{
			if (!entry.is_regular_file())
				continue;

			const auto& path = entry.path();
			if (path.extension() != ".ttf" && path.extension() != ".otf")
				continue;

			if (path.filename().string().find("seguiemj") != std::string::npos)
				continue;

			ImFont* font = io.Fonts->AddFontFromFileTTF(path.string().c_str(), ch_ig.kichthuoc, &config, glyph_ranges);
			if (font)
				ts_eg.danhsach_font.emplace_back(path.filename().string(), font);
			else
				throw std::runtime_error("❌ Không thể nạp font: " + path.string());
		}

		if (!ts_eg.danhsach_font.empty())
		{
			io.Fonts->AddFontDefault();
			throw std::runtime_error("Không tìm thấy font hợp lệ trong: " + ch_ig.thumuc_font);
		}

		ts_eg.font_hientai = ts_eg.danhsach_font[0].second;
		io.FontDefault     = ts_eg.font_hientai;

		for (const auto& entry : fs::directory_iterator(ch_ig.thumuc_font))
		{
			if (!entry.is_regular_file())
				continue;

			const auto& path = entry.path();
			if (path.filename().string().find("seguiemj") == std::string::npos)
				continue;

			ImFontConfig emoji_config;
			emoji_config.MergeMode   = true;
			emoji_config.OversampleH = 1;
			emoji_config.OversampleV = 1;
			emoji_config.PixelSnapH  = true;

			const ImWchar emoji_ranges[] = {0x1F300, 0x1F64F, 0x2600, 0x26FF, 0x2700, 0x27BF, 0x1F680, 0x1F6FF, 0};
			io.Fonts->AddFontFromFileTTF(path.string().c_str(), ch_ig.kichthuoc, &emoji_config, emoji_ranges);
			break;
		}

		io.Fonts->Build();
	}

} // namespace

void khoitao_imgui(HWND hwnd, co_imgui co)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (có_cờ(co, co_imgui::font_tu_thumuc))
	{
		try
		{
			tai_tatca_font_trong_thumuc();
		}
		catch (const std::exception& ex)
		{
			std::printf("⚠️ Lỗi khi tải font: %s\n", ex.what());
		}
	}
	else
	{
		ImGui::GetIO().Fonts->AddFontDefault();
	}

	ImGui_ImplWin32_Init(hwnd);

	ImGui_ImplVulkan_InitInfo info = lay_cauhinh_imgui_vulkan();
	ImGui_ImplVulkan_Init(&info);
}

void dondep_imgui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
