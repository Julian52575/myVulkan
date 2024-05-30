//
// EPITECH PROJECT, 2024
// vulkan
// File description:
// encapsulate vulkan
//

#include "myvulkan.hpp"
#include "validationLayers.hpp"
#include <GLFW/glfw3.h>
#include <exception>
#include <iostream>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <optional>

namespace myVulkan {

    myVulkan::myVulkan()
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
        if (enableValidationLayers == true) {
            try {
                assertValidationLayerSupport();
            } catch (const myVulkanUnavailableValidationLayerException& e) {
                std::cerr << "[Warning] An error occured while checking the validation layer: \'"
                          << e.what() << "\'. Processing without them." << std::endl;
                enableValidationLayers = false;
            }
        }
        this->initPhysicalDevice();
        this->initQueueFamilyIndex();
        return;
    }

    myVulkan::~myVulkan()
    {
        if (this->_instance != VK_NULL_HANDLE)
            vkDestroyInstance(this->_instance, nullptr);
        if (this->_logicalDevice != VK_NULL_HANDLE)
            vkDestroyDevice(this->_logicalDevice, nullptr);
        return;
    }


    void
    myVulkan::initPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
        uint64_t bestDevicePoint = 0;

        //* deviceCount
        vkEnumeratePhysicalDevices(this->_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw myVulkanNoCompatibleGpuException();
        }
        //* deviceList
        std::vector<VkPhysicalDevice> deviceList(deviceCount);
        vkEnumeratePhysicalDevices(this->_instance, &deviceCount, deviceList.data());
        //* this->_physicalDevice
        for (const VkPhysicalDevice& currentDevice : deviceList) {
            uint64_t currentDevicePoint = this->getDeviceSuitabilityScore(currentDevice);
            if (currentDevicePoint == 0)
                continue;
            if (currentDevicePoint > bestDevicePoint) {
                bestDevice = currentDevice;
                bestDevicePoint = currentDevicePoint;
            }
        }
        if (bestDevice == VK_NULL_HANDLE)
            throw myVulkanNoCompatibleGpuException();
        this->_physicalDevice = bestDevice;
        return;
    }


    uint64_t
    myVulkan::getDeviceSuitabilityScore(VkPhysicalDevice device)
    {
        //Note: arbitrary score value for now
        uint64_t score = 51;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        //* deviceProperties
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 5000;
        score += deviceProperties.limits.maxImageDimension2D;
        //* deviceFeatures
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        if (!deviceFeatures.geometryShader)
            score -= 50;
        //* score
        return score;
    }


    void
    myVulkan::initQueueFamilyIndex()
    {
        uint32_t queueFamilyCount = 0;
        uint64_t index = 0;
        uint64_t bestScore = 0;
        std::optional<uint64_t> bestQueueFamilyIndex;

        if (this->_physicalDevice == VK_NULL_HANDLE)
            throw myVulkanUninitializedGpuException();
        //* queueFamilyList
        vkGetPhysicalDeviceQueueFamilyProperties(this->_physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(this->_physicalDevice, &queueFamilyCount, queueFamilyList.data());
        //* index && bestScore
        for (const auto& currentQueueFamily : queueFamilyList) {
            uint64_t currentScore = this->getQueueFamilySuitabilityScore(currentQueueFamily);
            if (currentScore == 0)
                continue;
            if (currentScore > bestScore) {
                bestScore = currentScore;
                bestQueueFamilyIndex = index;
            }
            index += 1;
        }
        if (bestQueueFamilyIndex.has_value() == false) {
            throw myVulkanNoCompatibleGpuException();
        }
        this->_queueFamilyIndex = *bestQueueFamilyIndex;
        return;
    }

    uint64_t
    myVulkan::getQueueFamilySuitabilityScore(VkQueueFamilyProperties queueFamilyProperties)
    {
        uint64_t score = 0;

        if (queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            score += 1000;
        return score;
    }

    void
    myVulkan::initLogicalDevice()
    {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};

        //* queueCreateInfo
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = this->_queueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        //* createInfo
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        VkResult result = vkCreateDevice(this->_physicalDevice, &createInfo, nullptr, &this->_logicalDevice);
        if (result != VK_SUCCESS)
            throw myVulkanLogicalDeviceInitializationException();
        vkGetDeviceQueue(this->_logicalDevice, this->_queueFamilyIndex, 0, &this->_graphicsQueue);
        return;
    }

}
