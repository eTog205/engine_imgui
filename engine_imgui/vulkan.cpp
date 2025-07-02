module;
#define VK_USE_PLATFORM_WIN32_KHR
#include <imgui_impl_vulkan.h>
#include <imgui_impl_win32.h>

module vulkan;

import thongso_engine;
import std;

ts_vk ts;

namespace
{
	constexpr int                                      MAX_FRAMES_IN_FLIGHT = 2;
	std::array<FrameSyncObjects, MAX_FRAMES_IN_FLIGHT> g_sync_objects       = {};
	int                                                g_current_frame      = 0;

	HWND                     g_cuaso           = nullptr;
	VkInstance               g_instance        = VK_NULL_HANDLE;
	VkPhysicalDevice         g_physical_device = VK_NULL_HANDLE;
	VkDevice                 g_device          = VK_NULL_HANDLE;
	VkQueue                  g_queue           = VK_NULL_HANDLE;
	uint32_t                 g_queue_family    = static_cast<uint32_t>(-1);
	VkDescriptorPool         g_descriptor_pool = VK_NULL_HANDLE;
	ImGui_ImplVulkanH_Window g_main_window;
	VkAllocationCallbacks*   g_allocator       = nullptr;
	uint32_t                 g_min_image_count = 2;
	VkPipelineCache          pipeline_cache    = VK_NULL_HANDLE;

	void check_vk(VkResult result, const char* thongtin = "")
	{
		if (result != VK_SUCCESS)
			throw std::runtime_error(std::format("❌ VkResult = {} tại {}", static_cast<int>(result), thongtin));
	}

	void check_vk_wrapper(VkResult result)
	{
		check_vk(result);
	}

	void taolai_surface_va_resize()
	{
		vkDeviceWaitIdle(g_device);
		ImGui_ImplVulkanH_DestroyWindow(g_instance, g_device, &g_main_window, g_allocator);

		RECT rect;
		GetClientRect(g_cuaso, &rect);
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		ImGui_ImplVulkanH_CreateOrResizeWindow(g_instance,
		                                       g_physical_device,
		                                       g_device,
		                                       &g_main_window,
		                                       g_queue_family,
		                                       g_allocator,
		                                       w,
		                                       h,
		                                       g_min_image_count);
	}
} // namespace

ImGui_ImplVulkan_InitInfo lay_cauhinh_imgui_vulkan()
{
	ImGui_ImplVulkan_InitInfo info = {};
	info.Instance                  = g_instance;
	info.PhysicalDevice            = g_physical_device;
	info.Device                    = g_device;
	info.QueueFamily               = g_queue_family;
	info.Queue                     = g_queue;
	info.PipelineCache             = VK_NULL_HANDLE;
	info.DescriptorPool            = g_descriptor_pool;
	info.RenderPass                = g_main_window.RenderPass;
	info.Subpass                   = 0;
	info.MinImageCount             = g_min_image_count;
	info.ImageCount                = g_main_window.ImageCount;
	info.MSAASamples               = VK_SAMPLE_COUNT_1_BIT;
	info.Allocator                 = g_allocator;
	info.CheckVkResultFn           = check_vk_wrapper;

	return info;
}

void tao_sync_objects()
{
	VkSemaphoreCreateInfo si = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	for (auto& sync : g_sync_objects)
	{
		check_vk(vkCreateSemaphore(g_device, &si, g_allocator, &sync.acquired), "vkCreateSemaphore acquired");
		check_vk(vkCreateSemaphore(g_device, &si, g_allocator, &sync.rendered), "vkCreateSemaphore rendered");
	}
}

void xoa_sync_objects()
{
	for (auto& sync : g_sync_objects)
	{
		if (sync.acquired)
			vkDestroySemaphore(g_device, sync.acquired, g_allocator);
		if (sync.rendered)
			vkDestroySemaphore(g_device, sync.rendered, g_allocator);
		sync.acquired = VK_NULL_HANDLE;
		sync.rendered = VK_NULL_HANDLE;
	}
}

bool khoitao_vulkan(HWND hwnd)
{
	g_cuaso = hwnd;

	const char* extensions[] = {"VK_KHR_surface", "VK_KHR_win32_surface"};

	VkInstanceCreateInfo ici    = {};
	ici.enabledExtensionCount   = 2;
	ici.ppEnabledExtensionNames = extensions;
	ici.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	check_vk(vkCreateInstance(&ici, g_allocator, &g_instance), "vkCreateInstance");

	g_physical_device = ImGui_ImplVulkanH_SelectPhysicalDevice(g_instance);
	g_queue_family    = ImGui_ImplVulkanH_SelectQueueFamilyIndex(g_physical_device);

	const float             prio[] = {1.0f};
	VkDeviceQueueCreateInfo qci    = {};
	qci.sType                      = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qci.queueFamilyIndex           = g_queue_family;
	qci.queueCount                 = 1;
	qci.pQueuePriorities           = prio;

	const char*        device_ext[] = {"VK_KHR_swapchain"};
	VkDeviceCreateInfo dci          = {};
	dci.sType                       = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	dci.queueCreateInfoCount        = 1;
	dci.pQueueCreateInfos           = &qci;
	dci.enabledExtensionCount       = 1;
	dci.ppEnabledExtensionNames     = device_ext;

	check_vk(vkCreateDevice(g_physical_device, &dci, g_allocator, &g_device), "vkCreateDevice");
	vkGetDeviceQueue(g_device, g_queue_family, 0, &g_queue);

	VkPipelineCacheCreateInfo pci = {VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO};
	check_vk(vkCreatePipelineCache(g_device, &pci, g_allocator, &pipeline_cache), "vkCreatePipelineCache");

	VkDescriptorPoolSize       pool_sizes[] = {{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}};
	VkDescriptorPoolCreateInfo dpci         = {};
	dpci.sType                              = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	dpci.flags                              = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	dpci.maxSets                            = 100;
	dpci.poolSizeCount                      = 1;
	dpci.pPoolSizes                         = pool_sizes;
	check_vk(vkCreateDescriptorPool(g_device, &dpci, g_allocator, &g_descriptor_pool), "vkCreateDescriptorPool");

	VkSurfaceKHR                surface;
	VkWin32SurfaceCreateInfoKHR sci{};
	sci.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	sci.hwnd      = g_cuaso;
	sci.hinstance = GetModuleHandle(nullptr);
	check_vk(vkCreateWin32SurfaceKHR(g_instance, &sci, g_allocator, &surface));

	RECT rect;
	GetClientRect(g_cuaso, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	g_main_window.Surface = surface;
	VkBool32 ok;
	check_vk(vkGetPhysicalDeviceSurfaceSupportKHR(g_physical_device, g_queue_family, surface, &ok),
	         "vkGetPhysicalDeviceSurfaceSupportKHR");
	if (!ok)
		return false;

	VkFormat fmts[] = {VK_FORMAT_B8G8R8A8_UNORM};
	g_main_window.SurfaceFormat =
	    ImGui_ImplVulkanH_SelectSurfaceFormat(g_physical_device, surface, fmts, 1, VK_COLORSPACE_SRGB_NONLINEAR_KHR);

	VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR};
	g_main_window.PresentMode =
	    ImGui_ImplVulkanH_SelectPresentMode(g_physical_device, surface, present_modes, IM_ARRAYSIZE(present_modes));

	ImGui_ImplVulkanH_CreateOrResizeWindow(
	    g_instance, g_physical_device, g_device, &g_main_window, g_queue_family, g_allocator, w, h, g_min_image_count);

	tao_sync_objects();

	return true;
}

void xuly_frame_vulkan()
{
	auto& sync = g_sync_objects[g_current_frame];

	RECT rect;
	GetClientRect(g_cuaso, &rect);
	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	if (!ImGui::GetCurrentContext())
	{
		std::cerr << "ImGui context chưa khởi tạo (xuly_frame_vulkan)\n";
		return;
	}

	ImGuiIO& io    = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));

	if (ts.yeucau_resize)
	{
		ts.yeucau_resize = false;
		vkDeviceWaitIdle(g_device);
		ImGui_ImplVulkanH_CreateOrResizeWindow(g_instance,
		                                       g_physical_device,
		                                       g_device,
		                                       &g_main_window,
		                                       g_queue_family,
		                                       g_allocator,
		                                       w,
		                                       h,
		                                       g_min_image_count);
	}

	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// giaodien_main(w, h);
	// giaodien_kiemtra_loi(w, h);

	ImGui::Render();

	uint32_t index;
	VkResult err =
	    vkAcquireNextImageKHR(g_device, g_main_window.Swapchain, UINT64_MAX, sync.acquired, VK_NULL_HANDLE, &index);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
	{
		ts.yeucau_resize = false;
		taolai_surface_va_resize();
		return;
	}
	check_vk(err, "vkAcquireNextImageKHR");

	if (index >= g_main_window.ImageCount)
		return;

	ImGui_ImplVulkanH_Frame* fd = &g_main_window.Frames[index];
	check_vk(vkWaitForFences(g_device, 1, &fd->Fence, VK_TRUE, UINT64_MAX), "vkWaitForFences");
	check_vk(vkResetFences(g_device, 1, &fd->Fence), "vkResetFences");
	check_vk(vkResetCommandPool(g_device, fd->CommandPool, 0), "vkResetCommandPool");

	VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
	begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	check_vk(vkBeginCommandBuffer(fd->CommandBuffer, &begin_info), "vkBeginCommandBuffer");

	VkClearValue clear_value = {{{1.0f, 1.0f, 1.0f, 1.0f}}}; // trắng
	// VkClearValue clear_value = {{{0.1f, 0.1f, 0.1f, 1.0f}}}; // xám

	VkRenderPassBeginInfo rp_info    = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
	rp_info.renderPass               = g_main_window.RenderPass;
	rp_info.framebuffer              = fd->Framebuffer;
	rp_info.renderArea.extent.width  = g_main_window.Width;
	rp_info.renderArea.extent.height = g_main_window.Height;
	rp_info.clearValueCount          = 1;
	rp_info.pClearValues             = &clear_value;

	vkCmdBeginRenderPass(fd->CommandBuffer, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);
	vkCmdEndRenderPass(fd->CommandBuffer);
	check_vk(vkEndCommandBuffer(fd->CommandBuffer), "vkEndCommandBuffer");

	VkPipelineStageFlags wait_stage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo         submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
	submit_info.waitSemaphoreCount   = 1;
	submit_info.pWaitSemaphores      = &sync.acquired;
	submit_info.pWaitDstStageMask    = &wait_stage;
	submit_info.commandBufferCount   = 1;
	submit_info.pCommandBuffers      = &fd->CommandBuffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores    = &sync.rendered;

	check_vk(vkQueueSubmit(g_queue, 1, &submit_info, fd->Fence), "vkQueueSubmit");

	VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
	present_info.waitSemaphoreCount = 1;
	present_info.pWaitSemaphores    = &sync.rendered;
	present_info.swapchainCount     = 1;
	present_info.pSwapchains        = &g_main_window.Swapchain;
	present_info.pImageIndices      = &index;

	VkResult err2 = vkQueuePresentKHR(g_queue, &present_info);
	if (err2 == VK_ERROR_OUT_OF_DATE_KHR || err2 == VK_SUBOPTIMAL_KHR)
	{
		ts.yeucau_resize = false;
		taolai_surface_va_resize();
	}
	else
	{
		check_vk(err2, "vkQueuePresentKHR");
	}

	g_main_window.FrameIndex     = (g_main_window.FrameIndex + 1) % g_main_window.ImageCount;
	g_main_window.SemaphoreIndex = (g_main_window.SemaphoreIndex + 1) % g_main_window.ImageCount;
	g_current_frame              = (g_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void dondep_vulkan()
{
	vkDeviceWaitIdle(g_device);
	xoa_sync_objects();

	ImGui_ImplVulkanH_DestroyWindow(g_instance, g_device, &g_main_window, g_allocator);
	vkDestroyDescriptorPool(g_device, g_descriptor_pool, g_allocator);
	if (pipeline_cache)
		vkDestroyPipelineCache(g_device, pipeline_cache, g_allocator);
	vkDestroyDevice(g_device, g_allocator);
	vkDestroySurfaceKHR(g_instance, g_main_window.Surface, g_allocator);
	vkDestroyInstance(g_instance, g_allocator);
}
