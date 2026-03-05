//
// EPITECH PROJECT, 2025
// Arcade
// File description:
// DlFunctions.cpp
//

#include "DlUtils.hpp"
#include <iostream>
#include <string>
#include "dlfcn.h"
#include "types/types.hpp"

void utils::DlUtils::open(const std::string& pathToLib, const std::string& libName)
{
    void* lib = dlopen(pathToLib.c_str(), RTLD_LAZY);
    if (lib == nullptr)
        throw DlUtilsError(dlerror());
    this->_loadedLib[libName] = lib;
}

void utils::DlUtils::close(const std::string& libName)
{
    if (this->_loadedLib.find(libName) == this->_loadedLib.end() || dlclose(this->_loadedLib[libName]) != 0)
        throw DlUtilsError(dlerror());
    this->_loadedLib.erase(libName);
}

// common::ModuleType utils::DlUtils::getModuleType(void* lib)
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
