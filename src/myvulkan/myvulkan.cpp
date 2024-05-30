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

    bool
    queueFamilyIndexs::isComplete()
    {
        if (this->_graphicsFamily.has_value() == false)
            return false;
#ifdef _WIN32
        if (this->_graphicsFamily.has_value() == false)
            return false;
#endif
        return true;
    }

    void
    queueFamilyIndexs::completeFromPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t queueFamilyCount = 0;
        uint64_t index = 0;
        uint64_t bestScore = 0;
        std::optional<uint64_t> bestQueueFamilyIndex;

        //* queueFamilyList
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());
        //* indexs
        for (const auto& currentQueueFamily : queueFamilyList) {
            //* graphic
            if (currentQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                this->_graphicsFamily = index;
            //* windows os support
#ifdef _WIN32
            VkBool32 khrSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &khrSupport);
            if (khrSupport)
                this->_windowFamily = index;
#endif

            index += 1;
        }
    }


    myVulkan::myVulkan(myVulkanGLFWwindow& window) : _window(window)
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
        this->initSurface();
        this->initPhysicalDevice();
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
        if (this->_physicalDevice == VK_NULL_HANDLE)
            throw myVulkanUninitializedGpuException();
        this->_queueFamilyIndexs.completeFromPhysicalDevice(this->_physicalDevice, this->_surface);
        if (this->_queueFamilyIndexs.isComplete() == false)
            throw myVulkanMissingQueueException();
        return;
    }


    void
    myVulkan::initLogicalDevice()
    {
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
        vkGetPhysicalDeviceFeatures(this->_physicalDevice, &deviceFeatures);
        createInfo.pEnabledFeatures = &deviceFeatures;
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        //* this->_logicalDevice
        VkResult result = vkCreateDevice(this->_physicalDevice, &createInfo, nullptr, &this->_logicalDevice);
        if (result != VK_SUCCESS)
            throw myVulkanLogicalDeviceInitializationException();
        vkGetDeviceQueue(this->_logicalDevice, this->_queueFamilyIndexs._graphicsFamily.value(), 0, &this->_graphicsQueue);
        return;
    }

    void
    myVulkan::initSurface()
    {
#ifdef _WIN32
        VkWin32SurfaceCreateInfoKHR createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(window);
        createInfo.hinstance = GetModuleHandle(nullptr);
        if (vkCreateWin32SurfaceKHR(this->_instance, &createInfo, nullptr, &this->_surface) != VK_SUCCESS)
            throw std::exception();
#endif
    }

}
