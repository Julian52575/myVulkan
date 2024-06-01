//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// glfw Window
//
#pragma once
    #include <cstdint>
#include <exception>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
    #include <string>

#ifdef _WIN32
    #define VK_USE_PLATFORM_WIN32_KHR
    #include <GLFW/glfw3.h>
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>

#else
    #include <GLFW/glfw3.h>

#endif

    #include "../../../interfaces/ImyVulkanWindow.hpp"

namespace myVulkan {

    class muVulkanGLFWwindowInitializationException : public std::exception {
        public:
            virtual const char *what() const throw() { return "An error occured when initializing the glfw window."; };
    };
    class muVulkanGLFWwindowSurfaceInitializationException : public std::exception {
        public:
            virtual const char *what() const throw() { return "An error occured when initializing the glfw window surface."; };
    };

    class myVulkanGLFWwindow : public ImyVulkanWindow {
        public:
            myVulkanGLFWwindow(const std::string& title, uint64_t width = 1920, uint64_t height = 1080);
            ~myVulkanGLFWwindow();
        public:
            void
            runFrame(void);
        public:
            uint64_t
            getWidth() const;
            uint64_t
            getHeight() const;
            const myVulkan2PointInt&
            getFrameBufferSize();
            VkSurfaceKHR&
            getSurface(VkInstance& instance);
#ifdef _WIN32
        public:
            HWND
            getHWND() const;
#endif
        private:
            GLFWwindow *_window = nullptr;
            uint64_t _width = 0;
            uint64_t _height = 0;
            myVulkan2PointInt _frameBufferSize;
            VkSurfaceKHR _surface = VK_NULL_HANDLE;

    }; //class glfwWindow


}
