//
// EPITECH PROJECT, 2024
// Vulkan
// File description:
// idk what that validation layer is but sure
//
#pragma once
    #include <exception>
    #include <vector>
    #include <vulkan/vulkan_core.h>
    #define GLFW_INCLUDE_VULKAN
    #include <GLFW/glfw3.h>
    #define GLM_FORCE_RADIANS
    #define GLM_FORCE_DEPTH_ZERO_TO_ONE
    #include <glm/vec4.hpp>
    #include <glm/mat4x4.hpp>
    #include <iostream>

namespace myVulkan {
    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    class myVulkanUnavailableValidationLayerException : public std::exception {
        public:
            const char *what() const throw() { return "A requested validation layer cannot be found."; };
    };
    void assertValidationLayerSupport();
}
