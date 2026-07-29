cmake_minimum_required(VERSION 3.27)
project(fluid_sim)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)


include(FetchContent)

FetchContent_Declare(
        fetch_vk_bootstrap
        GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
        GIT_TAG        v1.4.356
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)

FetchContent_Declare(
        fetch_vma
        GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(fetch_vma)

FetchContent_Declare(
        fetch_sdl
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.4.12
)
FetchContent_MakeAvailable(fetch_sdl)

FetchContent_Declare(
        fetch_glm
        GIT_REPOSITORY	https://github.com/g-truc/glm.git
        GIT_TAG 	    v1.0.3
)
FetchContent_MakeAvailable(fetch_glm)

FetchContent_Declare(
        fetch_fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        12.2.0
)
FetchContent_MakeAvailable(fetch_fmt)

FetchContent_Declare(
        fetch_stb_image
        URL "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
        DOWNLOAD_NO_EXTRACT TRUE
)
FetchContent_MakeAvailable(fetch_stb_image)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE)

add_executable(fluid_sim main.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
            Vulkan::Vulkan
            vk-bootstrap::vk-bootstrap
            GPUOpen::VulkanMemoryAllocator
            SDL3::SDL3
            glm::glm
            fmt::fmt
            stb_image
)

