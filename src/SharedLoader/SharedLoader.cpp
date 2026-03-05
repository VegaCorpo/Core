//
// EPITECH PROJECT, 2025
// Arcade
// File description:
// DlFunctions.cpp
//

#include "SharedLoader.hpp"
#include <boost/dll/import.hpp>
#include <iostream>
#include <string>
#include "types/types.hpp"

void utils::SharedLoader::open(const std::string& pathToLib, const std::string& libName)
{
    boost::dll::fs::path lib_path = pathToLib;
    std::cout << "Hello from open" << std::endl;
    // this->_loadedLib[libName] = lib;
}

void utils::SharedLoader::close(const std::string& libName)
{}

// common::ModuleType utils::SharedLoader::getModuleType(void* lib)
// {
//     auto* const module = reinterpret_cast<common::ModuleType (*)()>(dlsym(lib, "get_module_type"));
//     if (module == nullptr)
//         throw DlUtilsError(dlerror());
//     return module();
// }

// std::unique_ptr<ANAL::IRenderer> Arcade::DlUtils::loadDisplay(void* lib)
// {
//     const auto module = reinterpret_cast<std::unique_ptr<ANAL::IRenderer> (*)()>(dlsym(lib,
//     "uwu_entrypoint_renderer")); if (module == nullptr)
//         throw DlUtilsError(dlerror());
//     std::unique_ptr<ANAL::IRenderer> renderer = module();
//     return std::move(renderer);
// }
