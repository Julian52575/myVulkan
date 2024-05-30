//
// EPITECH PROJECT, 2024
// my vulkan
// File description:
// validation have layers, just like onions
//
#include "validationLayers.hpp"
#include <iostream>
#include <vector>
#include <cstring>

namespace myVulkan {

    void assertValidationLayerSupport()
    {
        uint32_t layerCount = 0;
        std::vector<VkLayerProperties> availableLayers;

        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        availableLayers.reserve(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *currentLayerName : validationLayers) {
            bool foundCurrent = false;
            for (const auto& layerProperties : availableLayers) {
                if (strcmp(currentLayerName, layerProperties.layerName) == 0) {
                    foundCurrent = true;
                    break;
                }
            }//
            if (foundCurrent == false) {
                std::cerr << "Error: Cannot find validation layer \'" << currentLayerName << "\'" << std::endl;
                throw myVulkanUnavailableValidationLayerException();
            }
        }
    }

};
