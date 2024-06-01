//
// EPITECH PROJECT, 2024
// myVulkan
// File description:
// interface to follow for window implementation
//
#pragma once
    #include <cstdint>

    #include <vulkan/vulkan_core.h>
    #include "../commons/2PointInt.hpp"

namespace myVulkan {

    class ImyVulkanWindow {
        public:
            virtual ~ImyVulkanWindow() = default;
            virtual void
            runFrame(void) = 0;
        public:
            virtual uint64_t
            getWidth() const = 0;
            virtual uint64_t
            getHeight() const = 0;
            virtual const myVulkan2PointInt&
            getFrameBufferSize() = 0;
            virtual VkSurfaceKHR&
            getSurface(VkInstance& instance) = 0;

#ifdef _WIN32
        public:
            virtual HWND
            getHWND() const = 0;
#endif
    };

}

