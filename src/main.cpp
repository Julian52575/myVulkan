//
// EPITECH PROJECT, 2024
// vulkan experience
// File description:
// main
//

#include "myvulkan/myvulkan.hpp"
#include "myvulkan/window/glfwWindow.hpp"

namespace myVulkan {

    myVulkanGLFWwindow
    getWindow(const std::string& title)
    {
        myVulkanGLFWwindow window = myVulkanGLFWwindow(title);

        return myVulkanGLFWwindow(title);
    };

}

int main(void)
{
    myVulkan::myVulkanGLFWwindow window = myVulkan::getWindow("Hello world");
    myVulkan::myVulkan vulkan = myVulkan::myVulkan(window);

    std::cout << "Succesfully created myVulkan !" << std::endl;
    window.runFrame();
    return 0;
}
