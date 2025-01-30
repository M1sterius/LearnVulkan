cmake_minimum_required(VERSION 3.28)

project(LearnVulkan)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

add_executable(LearnVulkan
    Dependencies/stb_image/stb_image.cpp

    Source/main.cpp
    Source/Window.cpp
    Source/Window.hpp
    Source/VulkanInstance.cpp
    Source/VulkanInstance.hpp
    Source/GlobalUtility.cpp
    Source/GlobalUtility.hpp
    Source/VulkanDevice.cpp
    Source/VulkanDevice.hpp
    Source/Swapchain.cpp
    Source/Swapchain.hpp
    Source/VertexBuffer.cpp
    Source/VertexBuffer.hpp
    Source/IndexBuffer.cpp
    Source/IndexBuffer.hpp
    Source/Texture.cpp
    Source/Texture.hpp
        Source/Shader.cpp
        Source/Shader.hpp
        Source/Framebuffer.cpp
        Source/Framebuffer.hpp
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(LearnVulkan PRIVATE _DEBUG)
endif()

set(DEPENDENCIES_DIR ${CMAKE_SOURCE_DIR}/Dependencies)
set(VULKAN_SDK $ENV{VULKAN_SDK})
set(VULKAN_SDK_PATH "${VULKAN_SDK}")

find_package(Vulkan REQUIRED)

add_subdirectory(${DEPENDENCIES_DIR}/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)

target_link_libraries(LearnVulkan PRIVATE
    Vulkan::Vulkan
    glfw
)

target_include_directories(LearnVulkan PRIVATE
    Source
    ${Vulkan_INCLUDE_DIRS}
    ${VULKAN_SDK_PATH}/Include
    ${DEPENDENCIES_DIR}/glm
    ${DEPENDENCIES_DIR}/glfw/include
    ${DEPENDENCIES_DIR}/stb_image
)
