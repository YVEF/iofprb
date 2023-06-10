#include "application.h"
#include <iostream>
#include "elements/topmenu.h"
//#include "elements/start_menu.h"
//#include "elements/bench_chart.h"
#include "elements/main_canvas.h"
#include <implot.h>

namespace ui {

#define APP_HIGH 1000
#define APP_WIDTH 550

bool application::init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window_, true);
    ImGui_ImplVulkan_InitInfo init_info{};
    init_info.Instance = g_Instance;
    init_info.PhysicalDevice = g_PhysicalDevice;
    init_info.Device = g_Device;
    init_info.QueueFamily = g_QueueFamily;
    init_info.Queue = g_Queue;
    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = g_DescriptorPool;
    init_info.Subpass = 0;
    init_info.MinImageCount = g_MinImageCount;
    init_info.ImageCount = wd_->ImageCount;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = g_Allocator;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info, wd_->RenderPass);

    {
        // Use any command queue
        VkCommandPool command_pool = wd_->Frames[wd_->FrameIndex].CommandPool;
        VkCommandBuffer command_buffer = wd_->Frames[wd_->FrameIndex].CommandBuffer;

        err_ = vkResetCommandPool(g_Device, command_pool, 0);
        check_vk_result(err_);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err_ = vkBeginCommandBuffer(command_buffer, &begin_info);
        check_vk_result(err_);

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err_ = vkEndCommandBuffer(command_buffer);
        check_vk_result(err_);
        err_ = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
        check_vk_result(err_);

        err_ = vkDeviceWaitIdle(g_Device);
        check_vk_result(err_);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    return true;
}

bool application::init() noexcept
{
    if(init_vulkan())
    {
        init_imgui();
    }

    elements_.emplace_back(std::make_shared<topmenu>());
    elements_.emplace_back(std::make_shared<main_canvas>(config_, driveprv_));
    for(const auto& el : elements_)
        el->adjust();

    return false;
}

bool application::init_vulkan()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window_ = glfwCreateWindow(APP_HIGH, APP_WIDTH, "iofprb", nullptr, nullptr);


    if (!glfwVulkanSupported())
    {
        fprintf(stderr, "GLFW: Vulkan Not Supported << std::endl\n");
        return false;
    }

    ImVector<const char*> extensions;
    uint32_t extensions_count = 0;
    const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
    for (uint32_t i = 0; i < extensions_count; i++)
        extensions.push_back(glfw_extensions[i]);
    SetupVulkan(extensions);

    // Create Window Surface
    VkSurfaceKHR surface;
    err_ = glfwCreateWindowSurface(g_Instance, window_, g_Allocator, &surface);
    check_vk_result(err_);

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(window_, &w, &h);
    wd_ = &g_MainWindowData;
    SetupVulkanWindow(wd_, surface, w, h);

    return true;
}


void application::run() noexcept
{
    ImVec4 clear_color = ImVec4(0.35f, 0.35f, 0.35f, 0.35f);

    // Main loop
    while (!glfwWindowShouldClose(window_))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Resize swap chain?
        if (g_SwapChainRebuild)
        {
            int width, height;
            glfwGetFramebufferSize(window_, &width, &height);
            if (width > 0 && height > 0)
            {
                ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, g_Allocator, width, height, g_MinImageCount);
                g_MainWindowData.FrameIndex = 0;
                g_SwapChainRebuild = false;
            }
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowBorderSize = 0.0f;

        ImGui::PushStyleColor(ImGuiCol_WindowBg, styles_.main_color_dark2);
        render_context ctx(styles_);
        for(const auto& el : elements_)
        {
            if(el->skip(ctx))
                continue;

            el->render(ctx);
        }

        ImGui::PopStyleColor();


        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!is_minimized)
        {
            wd_->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            wd_->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            wd_->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            wd_->ClearValue.color.float32[3] = clear_color.w;
            FrameRender(wd_, draw_data);
            FramePresent(wd_);
        }
    }
}

void application::cleanup() noexcept
{
    // Cleanup
    err_ = vkDeviceWaitIdle(g_Device);
    check_vk_result(err_);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupVulkanWindow();
    CleanupVulkan();

    glfwDestroyWindow(window_);
    glfwTerminate();
}


}