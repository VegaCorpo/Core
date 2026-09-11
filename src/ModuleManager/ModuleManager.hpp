#pragma once

#include <expected>
#include <iostream>
#include <memory>
#include "interfaces/ILoaderEngine.hpp"
#include "interfaces/IPhysicsEngine.hpp"
#include "interfaces/IRenderEngine.hpp"
#include "interfaces/IUIEngine.hpp"
#include "src/SharedLoader/SharedLoader.hpp"

namespace core {
    enum class ModuleManagerError {
        SUCCESS,
        FAILED_TO_LOAD_MODULE,
    };

    class ModuleManager {
        public:
            ModuleManagerError _loadEngines() noexcept;

            // temporary in public
            utils::SharedLoader _loader;

            std::unique_ptr<common::ILoaderEngine> _loaderEngine = nullptr;
            std::unique_ptr<common::IPhysicsEngine> _physicsEngine = nullptr;
            std::unique_ptr<common::IUIEngine> _uiEngine = nullptr;
            std::unique_ptr<common::IRenderEngine> _renderEngine = nullptr;

        private:
            ModuleManagerError _getModuleInterface();

            ModuleManagerError _loadPhysics();
            ModuleManagerError _loadRenderer();
            ModuleManagerError _loadUi();
            ModuleManagerError _loadLoader();

            template <typename T, typename E>
            [[nodiscard]] ModuleManagerError reportLoaderError(std::expected<T, E> sharedLib)
            {
                if (!sharedLib) {
                    std::cerr << sharedLib.error() << std::endl;
                    return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
                }
                return core::ModuleManagerError::SUCCESS;
            }


    };
}