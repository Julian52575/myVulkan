//
// EPITECH PROJECT, 2024
// vulkan
// File description:
// encapsulate vulkan
//
#pragma once
    #include "validationLayers.hpp"
    #include "window/glfwWindow.hpp"

    #include <exception>
    #include <vector>
    #include <vulkan/vulkan_core.h>

    #define VK_USE_PLATFORM_WIN32_KHR
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLM_FORCE_RADIANS
    #define GLM_FORCE_DEPTH_ZERO_TO_ONE
    #include <glm/vec4.hpp>
    #include <glm/mat4x4.hpp>
    #include <iostream>
    #include <optional>

namespace myVulkan {
    #ifdef NDEBUG
        static bool enableValidationLayers = false;
    #else
        static bool enableValidationLayers = true;
    #endif

    //* Exceptions
    class myVulkanInitializationException : public std::exception {
        public:
            const char *what() const throw() { return "Cannot initialize Vulkan instance."; };
    };
    class myVulkanNoCompatibleGpuException : public std::exception {
        public:
            const char *what() const throw() { return "Failed to find GPUs with Vulkan support!"; };
    };
    class myVulkanUninitializedGpuException : public std::exception {
        public:
            const char *what() const throw() { return "Trying to use an uninitialized GPU."; };
    };
    class myVulkanLogicalDeviceInitializationException : public std::exception {
        public:
            const char *what() const throw() { return "Cannot initialize logical device."; };
    };
    class myVulkanMissingQueueException : public std::exception {
        public:
            const char *what() const throw() { return "A queue is missing."; };
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
            completeFromPhysicalDevice(VkPhysicalDevice device, VkSurfaceKHR surface);
        private:
    };

    //* myVulkan class
    class myVulkan {
        public:
            myVulkan(myVulkanGLFWwindow& window);
            ~myVulkan();

        private:
            myVulkanGLFWwindow& _window;
            VkInstance _instance = VK_NULL_HANDLE;
        private:
            VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
            void
            initPhysicalDevice();
            uint64_t
            getDeviceSuitabilityScore(VkPhysicalDevice device);
        private:
            struct queueFamilyIndexs _queueFamilyIndexs;
            void
            initQueueFamilyIndex();
            uint64_t
            getQueueFamilySuitabilityScore(VkQueueFamilyProperties queueFamilyProperties);
        private:
            VkDevice _logicalDevice = VK_NULL_HANDLE;
            VkQueue _graphicsQueue;
            void
            initLogicalDevice();
        private:
            VkSurfaceKHR _surface = VK_NULL_HANDLE;
            void
            initSurface();

    }; //myVulkan

};

