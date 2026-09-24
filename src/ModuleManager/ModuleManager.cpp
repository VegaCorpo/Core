#include "src/ModuleManager/ModuleManager.hpp"
#include "interfaces/ILoaderEngine.hpp"
#include "interfaces/IPhysicsEngine.hpp"
#include "interfaces/IRenderEngine.hpp"
#include "interfaces/IUIEngine.hpp"

core::ModuleManagerError core::ModuleManager::_getModuleInterface()
{
    if (this->_loadModule<common::IPhysicsEngine>("plugins/Physics/liborbital_physics", "get_engine",
                                                  "get_physics_engine") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    if (this->_loadModule<common::IRenderEngine>("plugins/Renderer/liborbital_render", "get_engine",
                                                 "get_render_engine") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    if (this->_loadModule<common::IUIEngine>("plugins/UI/liborbital_ui", "get_engine", "get_ui_engine") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    if (this->_loadModule<common::ILoaderEngine>("plugins/Loader/liborbital_loader.so", "get_engine",
                                                 "get_loader_engine") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    return core::ModuleManagerError::SUCCESS;
}

core::ModuleManagerError core::ModuleManager::loadEngines() noexcept
{
    return core::ModuleManager::_getModuleInterface();
}
