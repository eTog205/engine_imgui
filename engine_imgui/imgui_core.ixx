module;
#include <wtypes.h>

export module imgui_core;

import std;

export
{
	struct cauhinh_imgui
	{
		float       kichthuoc   = 20.0f;
		std::string thumuc_font = "font/";
	};

	enum class co_imgui : std::uint32_t
	{
		ko             = 0,
		font_tu_thumuc = 1u << 0
	};

	[[nodiscard]]
	inline constexpr co_imgui operator|(co_imgui a, co_imgui b) noexcept
	{
		return static_cast<co_imgui>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
	}

	[[nodiscard]]
	inline constexpr co_imgui operator&(co_imgui a, co_imgui b) noexcept
	{
		return static_cast<co_imgui>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
	}

	inline co_imgui& operator|=(co_imgui& a, co_imgui b) noexcept
	{
		return a = a | b;
	}

	[[nodiscard]]
	inline constexpr bool có_cờ(co_imgui giatri, co_imgui co) noexcept
	{
		return (static_cast<std::uint32_t>(giatri) & static_cast<std::uint32_t>(co)) != 0;
	}

	void khoitao_imgui(HWND hwnd, co_imgui co = co_imgui::ko);

	void dondep_imgui();

	extern cauhinh_imgui ch_ig;
}