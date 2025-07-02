module;
#include <imgui_impl_vulkan.h>
#include <wtypes.h>

export module vulkan;

export
{
	struct ts_vk
	{
		bool yeucau_resize = false;
	};

	struct FrameSyncObjects
	{
		VkSemaphore acquired = VK_NULL_HANDLE;
		VkSemaphore rendered = VK_NULL_HANDLE;
	};

	void tao_sync_objects();
	void xoa_sync_objects();

	ImGui_ImplVulkan_InitInfo lay_cauhinh_imgui_vulkan();

	bool khoitao_vulkan(HWND hwnd);
	void xuly_frame_vulkan();
	void dondep_vulkan();

	extern ts_vk ts;
}