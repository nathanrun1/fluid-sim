//
// Created by Nathan Reilly on 2026-08-07.
//

#ifndef FLUID_SIM_VK_ENGINE_H
#define FLUID_SIM_VK_ENGINE_H
#include <vector>

#include "SDL3/SDL.h"
#include "vulkan/vulkan_core.h"

constexpr unsigned int FRAME_OVERLAP = 2;

struct FrameData
{
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
};

class VkEngine
{
public:
    bool is_initialized{false};
    bool frame_number{0};
    VkExtent2D window_extent{1700, 900};
    
    FrameData frames[FRAME_OVERLAP];
    FrameData& current_frame() { return frames[frame_number % FRAME_OVERLAP]; }
    
    SDL_Window* window{nullptr};
    
    VkInstance instance;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkPhysicalDevice chosen_gpu;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkFormat swapchain_image_format;
    VkQueue graphics_queue;
    uint32_t graphics_queue_family;
    
    std::vector<VkImage> swapchain_images;
    std::vector<VkImageView> swapchain_image_views;
    VkExtent2D swapchain_extent;
    
    
    /** Initializes the engine */
    void init();
    
    /** Cleans up engine resources and shuts down */
    void cleanup() const;
    
    /** Runs engine draw loop */ 
    void draw();
    
    /** Run engine main loop */
    void run();
private:
    void init_vulkan();
    void init_swapchain();
    void init_commands();
    void init_sync_structures();
    void create_swapchain(uint32_t width, uint32_t height);
    void destroy_swapchain() const;
};


#endif //FLUID_SIM_VK_ENGINE_H
