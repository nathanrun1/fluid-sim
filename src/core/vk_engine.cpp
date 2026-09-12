//
// Created by Nathan Reilly on 2026-08-07.
//

#include <iostream>

#include "vk_engine.h"

#include "VkBootstrap.h"
#include "SDL3/SDL_vulkan.h"

#define VK_CHECK(x) \
do { \
    VkResult err = x; \
if (err != VK_SUCCESS) { \
    std::cout << "Detected Vulkan error: " << err << std::endl; \
    abort(); \
} \
} while (0)

constexpr bool USE_VALIDATION_LAYERS = true;

void VkEngine::init()
{
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_WindowFlags window_flags = SDL_WINDOW_VULKAN;
    
    window = SDL_CreateWindow(
        "Fluid Sim",
        window_extent.width,
        window_extent.height,
        window_flags
    );
    
    init_vulkan();
    init_swapchain();
    init_commands();
    init_sync_structures();
    
    is_initialized = true;
}

void VkEngine::cleanup() const
{
    if (is_initialized)
    {
        destroy_swapchain();
        
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyDevice(device, nullptr);
        
        vkb::destroy_debug_utils_messenger(instance, debug_messenger);
        vkDestroyInstance(instance, nullptr);
        SDL_DestroyWindow(window);
    }
}

void VkEngine::draw()
{
}

void VkEngine::run()
{
}

void VkEngine::init_vulkan()
{
    vkb::InstanceBuilder builder;
    
    // Builder initializes VkInstance with default debug messenger for validation layers
    auto inst_ret = builder.set_app_name("Fluid Sim")
        .request_validation_layers(USE_VALIDATION_LAYERS)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();
    
    vkb::Instance vkb_inst = inst_ret.value();
    
    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;
    
    
    SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
    
    VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features13.dynamicRendering = true;
    features13.synchronization2 = true;
    
    VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;
    
    // Select physical device based on feature constraints 
    vkb::PhysicalDeviceSelector selector{ vkb_inst };
    vkb::PhysicalDevice physical_device = selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)  // Filter by available features
        .set_required_features_12(features12)  // ^
        .set_surface(surface)                  // Filter by surface presentation capability
        .select()                              // Run device selection
        .value();
    
    // Abstract device type
    vkb::DeviceBuilder device_builder{ physical_device };
    vkb::Device vkb_device = device_builder.build().value();
    
    device = vkb_device.device;
    chosen_gpu = physical_device.physical_device;
    
    graphics_queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
}

void VkEngine::init_swapchain()
{
    create_swapchain(window_extent.width, window_extent.height);
}

void VkEngine::init_commands()
{
    VkCommandPoolCreateInfo command_pool_info = {};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_info.pNext = nullptr;
    command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Allows us to reset individual command buffers
    command_pool_info.queueFamilyIndex = graphics_queue_family;  // Optimize allocation to graphics queue family
    
    for (int i = 0; i < FRAME_OVERLAP; ++i)  // One command pool per overlapped frame to allow concurrent per-frame allocations
    {
        VK_CHECK(vkCreateCommandPool(device, &command_pool_info, nullptr, &frames[i].command_pool));
        
        VkCommandBufferAllocateInfo cmd_alloc_info = {};
        cmd_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmd_alloc_info.pNext = nullptr;
        cmd_alloc_info.commandPool = frames[i].command_pool;
        cmd_alloc_info.commandBufferCount = 1;
        cmd_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        
        VK_CHECK(vkAllocateCommandBuffers(device, &cmd_alloc_info, &frames[i].command_buffer));
    }
}

void VkEngine::init_sync_structures()
{
    
}

void VkEngine::create_swapchain(uint32_t width, uint32_t height)
{
    vkb::SwapchainBuilder swapchain_builder{ chosen_gpu, device, surface };
    
    swapchain_image_format = VK_FORMAT_R8G8B8A8_UNORM;
    
    vkb::Swapchain vkb_swapchain = swapchain_builder
        .set_desired_format(VkSurfaceFormatKHR{ .format = swapchain_image_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(width, height)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();
    
    swapchain_extent = vkb_swapchain.extent;
    swapchain = vkb_swapchain.swapchain;
    swapchain_images = vkb_swapchain.get_images().value();
    swapchain_image_views = vkb_swapchain.get_image_views().value();
}

void VkEngine::destroy_swapchain() const
{
    // Cleanup swapchain object, in turn cleans up VkImage objects 
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    
    for (int i = 0; i < swapchain_image_views.size(); ++i)
    {
        vkDestroyImageView(device, swapchain_image_views[i], nullptr);
    }
}
