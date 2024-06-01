//
// EPITECH PROJECT, 2024
// physical device
// File description:
// myVulkan
//
#include "physicalDevice.hpp"
#include "myvulkan.hpp"
#include <cstddef>
#include <vulkan/vulkan_core.h>

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
            this->checkWin32Support(device, index);
            #endif
            index += 1;
        }
        //*     unused variable fix
        if (surface == VK_NULL_HANDLE)
            return;
        return;
    }

#ifdef _WIN32
    void
    queueFamilyIndexs::checkWin32Support(VkPhysicalDevice device, VkSurfaceKHR surface, uint64_t index)
    {
        VkBool32 khrSupport = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &khrSupport);
        if (khrSupport)
            this->_windowFamily = index;
        return;
    }
#endif

    //*         QueueIndexes





    //*         Physical Device

    myVulkanPhysicalDevice::myVulkanPhysicalDevice(VkInstance const *instance) : _instance(instance)
    {
        if (instance == nullptr)
            throw myVulkanInitializationException();
        this->createDevice();
        return;
    }

    void
    myVulkanPhysicalDevice::createDevice()
    {
        uint32_t deviceCount = 0;
        VkPhysicalDevice bestDevice = VK_NULL_HANDLE;
        uint64_t bestDevicePoint = 0;

        //* deviceCount
        vkEnumeratePhysicalDevices(*this->_instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw myVulkanNoCompatibleGpuException();
        }
        //* deviceList
        std::vector<VkPhysicalDevice> deviceList(deviceCount);
        vkEnumeratePhysicalDevices(*this->_instance, &deviceCount, deviceList.data());
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
#ifdef _WIN32
        this->initSurface(instance);
#endif
        return;
    }

    myVulkanPhysicalDevice::~myVulkanPhysicalDevice()
    {
#ifdef _WIN32
        vkDestroySurfaceKHR(this->_instance, this->_surface, nullptr);
#endif
        return;
    }
    myVulkanPhysicalDevice myVulkanPhysicalDevice::operator=(const myVulkanPhysicalDevice& other)
    {
        return *this;
    }

    uint64_t
    myVulkanPhysicalDevice::getDeviceSuitabilityScore(VkPhysicalDevice device)
    {
        //Note: arbitrary score value for now
        uint64_t score = 0;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        if (this->checkDeviceExtensionSupport(device) == false)
            return 0;
        //* deviceProperties
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        score += deviceProperties.limits.maxImageDimension2D;
        //* deviceFeatures
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        if (!deviceFeatures.geometryShader)
           return 0;
        return score;
    }

    bool
    myVulkanPhysicalDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount = 0;

        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        std::set<std::string> requiredExtensions(neccesaryExtensions.begin(), neccesaryExtensions.end());
        for (const auto& currentExtension : availableExtensions) {
            requiredExtensions.erase(currentExtension.extensionName);
        }
        return requiredExtensions.empty();
    }

    VkPhysicalDevice&
    myVulkanPhysicalDevice::getDevice()
    {
        return this->_physicalDevice;
    }

    const queueFamilyIndexs&
    myVulkanPhysicalDevice::getQueueFamilyIndexs()
    {
        if (this->_queueFamilyIndexs.isComplete() == false)
            this->_queueFamilyIndexs.completeFromPhysicalDevice(this->_physicalDevice, this->_surface);
        return this->_queueFamilyIndexs;
    }


#ifdef _WIN32
    void
    myVulkanPhysicalDevice::initSurface(VkInstance& instance)
    {
        VkWin32SurfaceCreateInfoKHR createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = glfwGetWin32Window(window);
        createInfo.hinstance = GetModuleHandle(nullptr);
        if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &this->_surface) != VK_SUCCESS)
            throw std::exception();
        return;
    }
#endif

    VkSurfaceKHR&
    myVulkanPhysicalDevice::getSurface()
    {
        return this->_surface;
    }

}
