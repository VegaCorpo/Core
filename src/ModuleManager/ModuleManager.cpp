#include "src/ModuleManager/ModuleManager.hpp"
#include "interfaces/ILoaderEngine.hpp"

core::ModuleManagerError core::ModuleManager::_loadPhysics() {
    auto physics = this->_loader.load<std::unique_ptr<common::IPhysicsEngine>()>("plugins/Physics/liborbital_physics",
                                                                                 "get_engine", "get_physics_engine");

    if (this->reportLoaderError(physics) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    auto physicsFactory = this->_loader.get<std::unique_ptr<common::IPhysicsEngine>()>("get_physics_engine");
    if (this->reportLoaderError(physicsFactory) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    this->_physicsEngine = physicsFactory.value()();

    return core::ModuleManagerError::SUCCESS;
}

core::ModuleManagerError core::ModuleManager::_loadRenderer() {
    auto render = this->_loader.load<std::unique_ptr<common::IRenderEngine>()>("plugins/Renderer/liborbital_render",
                                                                               "get_engine", "get_render_engine");

    if (this->reportLoaderError(render) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    auto renderFactory = this->_loader.get<std::unique_ptr<common::IRenderEngine>()>("get_render_engine");
    if (this->reportLoaderError(renderFactory) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    this->_renderEngine = renderFactory.value()();

    return core::ModuleManagerError::SUCCESS;
}

core::ModuleManagerError core::ModuleManager::_loadUi() {
    auto ui = this->_loader.load<std::unique_ptr<common::IUIEngine>()>("plugins/UI/liborbital_ui", "get_engine",
                                                                       "get_ui_engine");
    if (this->reportLoaderError(ui) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    auto renderUiFactory = this->_loader.get<std::unique_ptr<common::IUIEngine>()>("get_ui_engine");

    if (this->reportLoaderError(renderUiFactory) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    this->_uiEngine = renderUiFactory.value()();

    return core::ModuleManagerError::SUCCESS;
}

core::ModuleManagerError core::ModuleManager::_loadLoader() {
    auto loader = this->_loader.load<std::unique_ptr<common::ILoaderEngine>()>("plugins/Loader/liborbital_loader.so", "get_engine",
                                                                       "get_loader_engine");
    if (this->reportLoaderError(loader) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    auto loaderFactory = this->_loader.get<std::unique_ptr<common::ILoaderEngine>()>("get_loader_engine");

    if (this->reportLoaderError(loaderFactory) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    this->_loaderEngine = loaderFactory.value()();

    return core::ModuleManagerError::SUCCESS;
}

core::ModuleManagerError core::ModuleManager::_getModuleInterface() {
    if (this->_loadPhysics() == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    if (this->_loadRenderer() == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    if (this->_loadUi() == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

    if (this->_loadLoader() == core::ModuleManagerError::FAILED_TO_LOAD_MODULE) {
        return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
    }

    return core::ModuleManagerError::SUCCESS;
}


core::ModuleManagerError core::ModuleManager::_loadEngines() noexcept
{
    return core::ModuleManager::_getModuleInterface();
}