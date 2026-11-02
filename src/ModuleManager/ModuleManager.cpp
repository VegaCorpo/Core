#include "src/ModuleManager/ModuleManager.hpp"
#include "interfaces/IInputsEngine.hpp"
#include "interfaces/ILoaderEngine.hpp"
#include "interfaces/IPhysicsEngine.hpp"
#include "interfaces/IRenderEngine.hpp"
#include "interfaces/IUIEngine.hpp"

core::ModuleManagerError core::ModuleManager::loadEngines() noexcept
{
    const bool allLoaded =
        this->_tryLoad<common::IPhysicsEngine>("plugins/Physics/liborbital_physics", "get_physics_engine") &&
        this->_tryLoad<common::IRenderEngine>("plugins/Renderer/liborbital_render", "get_render_engine") &&
        this->_tryLoad<common::IUIEngine>("plugins/UI/liborbital_ui", "get_ui_engine") &&
        this->_tryLoad<common::ILoaderEngine>("plugins/Loader/liborbital_loader.so", "get_loader_engine") &&
        this->_tryLoad<common::IInputsEngine>("plugins/Inputs/liborbital_inputs", "get_inputs_engine");

    return allLoaded ? core::ModuleManagerError::SUCCESS : core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
}
