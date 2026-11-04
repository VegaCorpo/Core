#include "src/ModuleManager/ModuleManager.hpp"
#include "interfaces/IInputsEngine.hpp"
#include "interfaces/ILoaderEngine.hpp"
#include "interfaces/IPhysicsEngine.hpp"
#include "interfaces/IRenderEngine.hpp"
#include "interfaces/IUIEngine.hpp"
#include "types/ModuleInfo.hpp"

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

core::ModuleManagerError core::ModuleManager::loadPluginsVersion() noexcept
{
    this->_pluginsInfo.clear();

    if (this->_loadPluginInfo("plugins/Physics/liborbital_physics", "get_physics_info") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    if (this->_loadPluginInfo("plugins/Renderer/liborbital_render", "get_render_info") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    if (this->_loadPluginInfo("plugins/UI/liborbital_ui", "get_ui_info") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    if (this->_loadPluginInfo("plugins/Loader/liborbital_loader.so", "get_loader_info") ==
        core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    return core::ModuleManagerError::SUCCESS;
}

core::ModuleManagerError core::ModuleManager::_loadPluginInfo(const std::string& pluginPath,
                                                              const std::string& getSymbol)
{
    auto handle = this->_loader.load<decltype(get_module_info)>(pluginPath, "get_module_info", getSymbol);
    if (this->reportLoaderError(handle) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    auto getInfo = this->_loader.get<decltype(get_module_info)>(getSymbol);
    if (this->reportLoaderError(getInfo) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    const common::ModuleInfo info = getInfo.value()();

    auto copy = [](const char* value) { return std::string(value != nullptr ? value : "unknown"); };
    this->_pluginsInfo.push_back({copy(info.name), copy(info.version), info.type});
    return core::ModuleManagerError::SUCCESS;
}
