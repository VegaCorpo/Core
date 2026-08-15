#include "SharedLoader.hpp"
#include <filesystem>
#include <iostream>

void utils::SharedLoader::getPlugins() {
    const std::filesystem::path plugins("plugins");

    for (const auto &type : std::filesystem::directory_iterator(plugins)) {
        if (!type.is_directory())
            continue;
        for (const auto &plugin : std::filesystem::directory_iterator(type)) {
            if (plugin.is_regular_file() && plugin.path().extension() == ".so") {
                this->_availabePlugins[type.path().filename().string()].push_back(plugin.path().filename().string());
            }
        }
    }
}

void utils::SharedLoader::printAvailablePlugins() {
    for (const auto &type : this->_availabePlugins) {
        std::cout << "type: " << type.first << std::endl;
        for (const auto &plugin : type.second) {
            std::cout << "\t" << plugin << std::endl;
        }
    }
}