#include <iostream>

#include "core/vk_engine.h"

int main() {
    VkEngine vk_engine;
    vk_engine.init();
    vk_engine.cleanup();
    
    std::cout << "Exited successfully" << std::endl;
    return 0; 
}

