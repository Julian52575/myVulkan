//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// encapsulation of GLFWwindow
//
    #include "glfwWindow.hpp"
    #include <GLFW/glfw3.h>
#include <cstddef>
#include <exception>
    #include <string>

namespace myVulkan {

    myVulkanGLFWwindow::myVulkanGLFWwindow(const std::string& title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        this->_window = glfwCreateWindow(1920, 1080, title.c_str(), nullptr, nullptr);
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
        float ratio;
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
    }
#endif

}
