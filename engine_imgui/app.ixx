module;
#include <wtypes.h>

export module app;

export
{
	extern HWND      g_cuaso;
	extern HINSTANCE g_instance;

	bool khoitao_ungdung();
	void vonglap_ungdung();
	void dondep_ungdung();
}
