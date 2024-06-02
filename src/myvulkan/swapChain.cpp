//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// swap chain
//
#include "swapChain.hpp"
#include <iostream>
#include <vulkan/vulkan_core.h>

namespace myVulkan {

    myVulkanSwapChain::myVulkanSwapChain(VkInstance instance, ImyVulkanWindow& window, myVulkanPhysicalDevice& physicalDevice, VkDevice& logicalDevice) : _logicalDevice(logicalDevice)
    {
        std::clog << "Initializing Swap chain..." << std::endl;
        const swapChainSupportDetails& swapCDetails = physicalDevice.getSwapChainSupportDetails();
        const queueFamilyIndexes& queueIndexes = physicalDevice.getQueueFamilyIndexs();
        uint32_t indexesTab[] =
            {queueIndexes._graphicsFamily.value(), queueIndexes._presentFamily.value()};
        uint32_t imageCount = swapCDetails._capabilities.minImageCount + 1;
        VkSwapchainCreateInfoKHR createInfo{};

        this->_surfaceFormat = swapCDetails.getSurfaceFormat();
        this->_presentationMode = swapCDetails.getPresentationMode();
        this->_extent2D = swapCDetails.getSwapExtent2D(window.getFrameBufferSize());
        //* imageCount
        if (swapCDetails._capabilities.maxImageCount > 0
        &&  imageCount > swapCDetails._capabilities.maxImageCount) {
            imageCount = swapCDetails._capabilities.maxImageCount;
        }
        //* createInfo
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = window.getSurface(instance);
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
        this->updateSwapChainImages();
        this->initImageViews();
        return;
    }

    myVulkanSwapChain::~myVulkanSwapChain()
    {
        for (auto currentView : this->_swapChainImagesViews) {
            vkDestroyImageView(this->_logicalDevice, currentView, nullptr);
        }
        return;
    }
    myVulkanSwapChain myVulkanSwapChain::operator=(const myVulkanSwapChain&)
    {
        return *this;
    }


    VkSwapchainKHR
    myVulkanSwapChain::getSwapChain() const
    {
        return this->_swapChain;
    }

    void
    myVulkanSwapChain::updateSwapChainImages()
    {
        uint32_t swapChainImagesCount = 0;

        vkGetSwapchainImagesKHR(this->_logicalDevice, this->_swapChain, &(swapChainImagesCount), nullptr);
        this->_swapChainImages.reserve(swapChainImagesCount);
        vkGetSwapchainImagesKHR(this->_logicalDevice, this->_swapChain, &(swapChainImagesCount), this->_swapChainImages.data());

    }

    void
    myVulkanSwapChain::initImageViews()
    {
        std::clog << "Initializing Image views..." << std::endl;
        VkImageViewCreateInfo createInfo{};

        //* createInfo
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->_surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        //* this->_swapChainImagesViews
        this->_swapChainImagesViews.resize(this->_swapChainImages.size());
        for (size_t i = 0; i < this->_swapChainImages.size(); i++) {
            createInfo.image = this->_swapChainImages[i];
            if (vkCreateImageView(this->_logicalDevice, &createInfo, nullptr, &(this->_swapChainImagesViews[i])) != VK_SUCCESS) {
                throw myVulkanSwapChainImageViewsInitializationException();
            }
        }
    }

}
