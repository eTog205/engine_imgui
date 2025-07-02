module;
#include <imgui.h>

export module thongso_engine;

import std;

export
{
	struct thongso_engine
	{

		ImFont*                                      font_hientai = nullptr;
		std::vector<std::pair<std::string, ImFont*>> danhsach_font;
	};

	inline thongso_engine ts_eg{};
}
