//
// EPITECH PROJECT, 2024
// physical device
// File description:
// myVulkan
//
#include "physicalDevice.hpp"
#include "myvulkan.hpp"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <vulkan/vulkan_core.h>

namespace myVulkan {

    bool
    queueFamilyIndexes::isComplete() const
    {
        if (this->_graphicsFamily.has_value() == false)
            return false;
        if (this->_presentFamily.has_value() == false)
            return false;
        return true;
    }


    void
    queueFamilyIndexes::completeFromPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
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
            VkBool32 khrSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &khrSupport);
            if (khrSupport)
                this->_presentFamily = index;
            index += 1;
        }
        //*     unused variable fix
        if (surface == VK_NULL_HANDLE)
            return;
        return;
    }

    //*         QueueIndexes

    //*         swapChainSupportDetails
    void
    swapChainSupportDetails::completeFromPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t formatCount = 0;
        uint32_t presentModeCount = 0;

        //* this->_capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &this->_capabilities);
        //* this->_formatList
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount > 0) {
            this->_formatList.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, this->_formatList.data());
        }
        //* this->_presentModesList
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount > 0) {
            this->_presentModesList.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, this->_presentModesList.data());
        }
        return;
    }
    bool
    swapChainSupportDetails::isComplete() const
    {
        if (this->_formatList.empty() == true)
            return false;
        if (this->_presentModesList.empty() == true)
            return false;
        return true;
    }
    VkSurfaceFormatKHR
    swapChainSupportDetails::getSurfaceFormat() const
    {
        //* If we find then good color format, use it
        for (const auto& currentFormat : this->_formatList) {
            if (currentFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            &&  currentFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return currentFormat;
        }
        //* else settle for the first one available
        return this->_formatList[0];
    }
    VkPresentModeKHR
    swapChainSupportDetails::getPresentationMode() const
    {
        //* if we find the mailbox mode, use it
        for (const auto& currentPresentMode : this->_presentModesList) {
            if (currentPresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return currentPresentMode;
        }
        //* if not, settle for the most common one
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D
    swapChainSupportDetails::getSwapExtent2D(const myVulkan2PointInt &frameBufferSize) const
    {
        VkExtent2D actualExtent = {0, 0};

        if (this->_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return this->_capabilities.currentExtent;
        actualExtent = {static_cast<uint32_t>(frameBufferSize.x), static_cast<uint32_t>(frameBufferSize.y)};
        actualExtent.width = std::clamp(actualExtent.width, this->_capabilities.minImageExtent.width,
            this->_capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, this->_capabilities.minImageExtent.height,
            this->_capabilities.maxImageExtent.height);
        return actualExtent;
    }




    //*         Physical Device

    myVulkanPhysicalDevice::myVulkanPhysicalDevice(VkInstance const *instance, VkSurfaceKHR& surface)
        : _instance(instance), _surface(surface)
    {
        std::clog << "Initializing Physical device..." << std::endl;
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
        swapChainSupportDetails swapCDetails;

        if (this->checkDeviceExtensionSupport(device) == false)
            return 0;
        swapCDetails.completeFromPhysicalDevice(device, this->_surface);
        if (swapCDetails.isComplete() == false)
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

    const queueFamilyIndexes&
    myVulkanPhysicalDevice::getQueueFamilyIndexs()
    {
        if (this->_queueFamilyIndexs.isComplete() == false)
            this->_queueFamilyIndexs.completeFromPhysicalDevice(this->_physicalDevice, this->_surface);
        return this->_queueFamilyIndexs;
    }

    const swapChainSupportDetails&
    myVulkanPhysicalDevice::getSwapChainSupportDetails()
    {
        if (this->_swapChainDetails.isComplete() == 0)
            this->_swapChainDetails.completeFromPhysicalDevice(this->_physicalDevice, this->_surface);
        return this->_swapChainDetails;
    }

}
