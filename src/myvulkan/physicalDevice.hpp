//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// physical device handler
//
#pragma once
    #include <cstdint>
    #include <exception>
    #include <vector>
    #include <optional>
    #include <string>
    #include <set>

    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #include <vulkan/vulkan_core.h>

namespace myVulkan {

    class myVulkanNoCompatibleGpuException : public std::exception {
        public:
            const char *what() const throw() { return "Failed to find GPUs with Vulkan support!"; };
    };

    const std::vector<const char *> neccesaryExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    //* queueFamilyIndexs struct
    struct queueFamilyIndexs {
        public:
            std::optional<uint32_t> _graphicsFamily;

#ifdef _WIN32
            std::optional<uint32_t> _windowFamily;
#endif

            bool
            isComplete();

            void
            completeFromPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface = VK_NULL_HANDLE);
#ifdef _WIN32
            void
            completeFromPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
            void
            checkWin32Support(VkPhysicalDevice device, VkSurfaceKHR surface, uint64_t index);
#endif
    };


    class myVulkanPhysicalDevice {
        public:
            myVulkanPhysicalDevice(VkInstance const *instance);
            ~myVulkanPhysicalDevice();
            myVulkanPhysicalDevice operator=(const myVulkanPhysicalDevice& other);
        public:
            void
            createDevice();
        public:
            VkPhysicalDevice&
            getDevice();
            VkSurfaceKHR&
            getSurface();
        private:
            VkInstance const* _instance;
            VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
            uint64_t
            getDeviceSuitabilityScore(VkPhysicalDevice device);
            bool
            checkDeviceExtensionSupport(VkPhysicalDevice device);
        private:
            queueFamilyIndexs _queueFamilyIndexs = {0};
            const queueFamilyIndexs&
            getQueueFamilyIndexs();
        private:
            VkSurfaceKHR _surface = VK_NULL_HANDLE;
#ifdef _WIN32
            void
            initSurface(VkInstance& instance);
#endif
    };

}
