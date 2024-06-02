//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// swap chain encapsulation
//
#pragma once
    #include <iostream>
#include <vulkan/vulkan_core.h>

    #include "../../interfaces/ImyVulkanWindow.hpp"
    #include "physicalDevice.hpp"

namespace myVulkan {

    class myVulkanSwapChainInitializationException : public std::exception {
        public:
            const char *what() const throw() { return "Cannot initialize swap chain."; };
    };
    class myVulkanSwapChainImageViewsInitializationException : public std::exception {
        public:
            const char *what() const throw() { return "Cannot initialize swap chain image view."; };
    };

    class myVulkanSwapChain {
        public:
            myVulkanSwapChain(VkInstance instance, ImyVulkanWindow& window, myVulkanPhysicalDevice& physicalDevice, VkDevice& logicalDevice);
            ~myVulkanSwapChain();
            myVulkanSwapChain operator=(const myVulkanSwapChain&);
        public:
            void
            updateSwapChainImages();
            VkSwapchainKHR
            getSwapChain() const ;

        private:
            VkDevice& _logicalDevice;
        private:
            VkSurfaceFormatKHR _surfaceFormat;
            VkPresentModeKHR _presentationMode;
            VkExtent2D _extent2D;
            VkSwapchainKHR _swapChain;
            uint32_t _swapChainImagesCount = 0;
            std::vector<VkImage> _swapChainImages;
        private:
            std::vector<VkImageView> _swapChainImagesViews;
            void
            initImageViews();

    };

}
