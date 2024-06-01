//
// EPITECH PROJECT, 2024
// vulkan
// File description:
// encapsulate vulkan
//

#include "myvulkan.hpp"
#include "physicalDevice.hpp"
#include "validationLayers.hpp"
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <optional>

namespace myVulkan {

    myVulkan::myVulkan(ImyVulkanWindow& window) : _window(window)
    {
        this->initVulkanInstance();
        VkSurfaceKHR& surface = window.getSurface(this->_instance);
        if (enableValidationLayers == true) {
            try {
                assertValidationLayerSupport();
            } catch (const myVulkanUnavailableValidationLayerException& e) {
                std::cerr << "[Warning] The following error occured while checking validation layers: \'"
                          << e.what() << "\'. Processing without validation layers." << std::endl;
                enableValidationLayers = false;
            }
        }
        this->_physicalDevice = myVulkanPhysicalDevice(&this->_instance, surface);
        this->initQueueFamilyIndex();
        this->initLogicalDevice();
        return;
    }

    myVulkan::~myVulkan()
    {
        if (this->_logicalDevice != VK_NULL_HANDLE)
            vkDestroyDevice(this->_logicalDevice, nullptr);
        if (this->_instance != VK_NULL_HANDLE)
            vkDestroyInstance(this->_instance, nullptr);
        return;
    }


    void
    myVulkan::initVulkanInstance()
    {
        VkApplicationInfo appInfo{};
        VkInstanceCreateInfo createInfo{};
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = nullptr;

        //* appInfo
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello myVulkan !";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        //* createInfo
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
            //* glfwExtensions
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;
        //* this->instance
        VkResult result = vkCreateInstance(&createInfo, nullptr, &this->_instance);
        if (result != VK_SUCCESS) {
            if (result == VK_ERROR_INCOMPATIBLE_DRIVER) {
                std::cerr << "Bozo MacOS" << std::endl;
            }
            throw myVulkanInitializationException();
        }
    }

    void
    myVulkan::initQueueFamilyIndex()
    {
        VkPhysicalDevice& physicalDevice = this->_physicalDevice->getDevice();

        if (physicalDevice == VK_NULL_HANDLE)
            throw myVulkanUninitializedGpuException();
#ifdef _WIN32
        VkSurfaceKHR& surface = this->_physicalDevice->getSurface();
        this->_queueFamilyIndexs.completeFromPhysicalDevice(physicalDevice, surface);
#else
        this->_queueFamilyIndexs.completeFromPhysicalDevice(physicalDevice);
#endif
        if (this->_queueFamilyIndexs.isComplete() == false)
            throw myVulkanMissingQueueException();
        return;
    }


    void
    myVulkan::initLogicalDevice()
    {
        VkPhysicalDevice& physicalDevice = this->_physicalDevice->getDevice();
        VkDeviceQueueCreateInfo queueCreateInfo{};
        float queuePriority = 1.0f;
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};

        //* queueCreateInfo
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = this->_queueFamilyIndexs._graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        //* createInfo
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
        createInfo.pEnabledFeatures = &deviceFeatures;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        createInfo.enabledExtensionCount = static_cast<uint32_t>(neccesaryExtensions.size());
        createInfo.ppEnabledExtensionNames = neccesaryExtensions.data();
        //* this->_logicalDevice
        VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &this->_logicalDevice);
        if (result != VK_SUCCESS)
            throw myVulkanLogicalDeviceInitializationException();
        vkGetDeviceQueue(this->_logicalDevice, this->_queueFamilyIndexs._graphicsFamily.value(), 0, &this->_graphicsQueue);
        return;
    }

    void
    myVulkan::initSwapChain()
    {
        swapChainSupportDetails& swapCDetails = this->_physicalDevice->getSwapChainSupportDetails();

        this->_surfaceFormat = swapCDetails.getSurfaceFormat();
        this->_presentationMode = swapCDetails.getPresentationMode();
        this->_extent2D = swapCDetails.getSwapExtent2D(this->_window.getFrameBufferSize());
        return;
    }


}
