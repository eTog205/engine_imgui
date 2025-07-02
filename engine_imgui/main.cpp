import app;
import thongso_engine;

import std;

int main()
{
	try
	{
		if (!khoitao_ungdung())
			return 1;
	}
	catch (const std::exception& e)
	{
		std::println("🛑 Lỗi nghiêm trọng: {}", e.what());
		return 1;
	}

	vonglap_ungdung();

	dondep_ungdung();
}
