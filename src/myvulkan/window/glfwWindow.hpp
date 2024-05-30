//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// glfw Window
//
#pragma once
    #include <exception>
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


namespace myVulkan {

    class muVulkanGLFWwindowInitializationException : public std::exception {
        public:
            virtual const char *what() const throw() { return "An error occured when initializing the glfw window."; };
    };

    class myVulkanGLFWwindow {
        public:
            myVulkanGLFWwindow(const std::string& title);
            ~myVulkanGLFWwindow();

            void
            runFrame(void);

#ifdef _WIN32
            HWND
            getHWND() const;
#endif

        private:
            GLFWwindow *_window = nullptr;

    }; //class glfwWindow


}
