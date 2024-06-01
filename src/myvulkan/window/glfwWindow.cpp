//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// encapsulation of GLFWwindow
//
    #include "glfwWindow.hpp"
    #include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <exception>
    #include <string>
#include <vulkan/vulkan_core.h>

namespace myVulkan {

    myVulkanGLFWwindow::myVulkanGLFWwindow(const std::string& title, uint64_t width, uint64_t height)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        this->_width = width;
        this->_height = height;
        this->_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (this->_window == NULL) {
            throw muVulkanGLFWwindowInitializationException();
        }
        return;
    }

    myVulkanGLFWwindow::~myVulkanGLFWwindow()
    {
        if (this->_window != nullptr)
            glfwDestroyWindow(this->_window);
        return;
    }

    void
    myVulkanGLFWwindow::runFrame()
    {
        if (glfwWindowShouldClose(this->_window) == true)
            return;
        glfwPollEvents();
        //* from the glfw documentation
        int width, height;

        glfwGetFramebufferSize(this->_window, &width, &height);


        glfwSwapBuffers(this->_window);
        return;
    }

#ifdef _WIN32
    HWND
    myVulkanGLFWwindow::getHWND() const
    {
        #warning getHWND compiled ! Windows user spotted !!!!
        return glfwGetWin32Window(this->_window);
    }
#endif

    uint64_t
    myVulkanGLFWwindow::getWidth() const
    {
        return this->_width;
    }
    uint64_t
    myVulkanGLFWwindow::getHeight() const
    {
        return this->_height;
    }
    const myVulkan2PointInt&
    myVulkanGLFWwindow::getFrameBufferSize()
    {
        if (this->_frameBufferSize.x == 0 || this->_frameBufferSize.y == 0) {
            glfwGetFramebufferSize(this->_window, &(this->_frameBufferSize.x), &(this->_frameBufferSize.y));
        }
        return this->_frameBufferSize;
    }

    VkSurfaceKHR&
    myVulkanGLFWwindow::getSurface(VkInstance& instance)
    {
        if (this->_surface != VK_NULL_HANDLE)
            return this->_surface;

#ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = this->_window.getHWND();
        createInfo.hinstance = GetModuleHandle(nullptr);
        if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &this->_surface) != VK_SUCCESS)
            throw std::exception();
#else
        VkResult rsult = glfwCreateWindowSurface(instance, this->_window, nullptr, &(this->_surface));
        if (rsult != VK_SUCCESS)
            throw muVulkanGLFWwindowSurfaceInitializationException();
#endif
        return this->_surface;
    }

}
