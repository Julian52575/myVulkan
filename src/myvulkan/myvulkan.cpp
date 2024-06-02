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
                std::cerr << "[Warning] The following exception occured while checking validation layers: \'"
                          << e.what() << "\'. Processing without validation layers." << std::endl;
                enableValidationLayers = false;
            }
        }
        this->_physicalDevice = myVulkanPhysicalDevice(&this->_instance, surface);
        this->initQueueFamilyIndex();
        this->initLogicalDevice();
        this->initSwapChain();
        return;
    }

    myVulkan::~myVulkan()
    {
        vkDestroySwapchainKHR(this->_logicalDevice, this->_swapChain, nullptr);
        if (this->_logicalDevice != VK_NULL_HANDLE)
            vkDestroyDevice(this->_logicalDevice, nullptr);
        if (this->_instance != VK_NULL_HANDLE)
            vkDestroyInstance(this->_instance, nullptr);
        return;
    }


    void
    myVulkan::initVulkanInstance()
    {
        std::clog << "Initializing Vulkan instance..." << std::endl;
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
        std::clog << "Initializing Queue indexes..." << std::endl;
        const queueFamilyIndexes& queueIndexes = this->_physicalDevice->getQueueFamilyIndexs();
        if (queueIndexes.isComplete() == false)
            throw myVulkanMissingQueueException();
        return;
    }


    void
    myVulkan::initLogicalDevice()
    {
        std::clog << "Initializing Logical device..." << std::endl;
        VkPhysicalDevice& physicalDevice = this->_physicalDevice->getDevice();
        const queueFamilyIndexes& queueFamilyIndexs = this->_physicalDevice->getQueueFamilyIndexs();
        VkDeviceQueueCreateInfo queueCreateInfo{};
        float queuePriority = 1.0f;
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};

        //* queueCreateInfo
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndexs._graphicsFamily.value();
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
        vkGetDeviceQueue(this->_logicalDevice, queueFamilyIndexs._graphicsFamily.value(), 0, &this->_graphicsQueue);
        return;
    }

    void
    myVulkan::initSwapChain()
    {
        std::clog << "Initializing Swap chain..." << std::endl;
        const swapChainSupportDetails& swapCDetails = this->_physicalDevice->getSwapChainSupportDetails();
        const queueFamilyIndexes& queueIndexes = this->_physicalDevice->getQueueFamilyIndexs();
        uint32_t indexesTab[] =
            {queueIndexes._graphicsFamily.value(), queueIndexes._presentFamily.value()};
        uint32_t imageCount = swapCDetails._capabilities.minImageCount + 1;
        VkSwapchainCreateInfoKHR createInfo{};

        this->_surfaceFormat = swapCDetails.getSurfaceFormat();
        this->_presentationMode = swapCDetails.getPresentationMode();
        this->_extent2D = swapCDetails.getSwapExtent2D(this->_window.getFrameBufferSize());
        //* imageCount
        if (swapCDetails._capabilities.maxImageCount > 0
        &&  imageCount > swapCDetails._capabilities.maxImageCount) {
            imageCount = swapCDetails._capabilities.maxImageCount;
        }
        //* createInfo
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = this->_window.getSurface(this->_instance);
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = this->_surfaceFormat.format;
        createInfo.imageColorSpace = this->_surfaceFormat.colorSpace;
        createInfo.imageExtent = this->_extent2D;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (queueIndexes._graphicsFamily != queueIndexes._presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = indexesTab;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = swapCDetails._capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = this->_presentationMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        //* swapChain
        if (vkCreateSwapchainKHR(this->_logicalDevice, &createInfo, nullptr, &(this->_swapChain)) != VK_SUCCESS)
            throw myVulkanSwapChainInitializationException();
        return;
    }

    void
    myVulkan::updateSwapChainImages()
    {
        vkGetSwapchainImagesKHR(this->_logicalDevice, this->_swapChain, &(this->_swapChainImagesCount), nullptr);
        this->_swapChainImages.reserve(this->_swapChainImagesCount);
        vkGetSwapchainImagesKHR(this->_logicalDevice, this->_swapChain, &(this->_swapChainImagesCount), this->_swapChainImages.data());

    }

}
