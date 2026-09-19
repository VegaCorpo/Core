#pragma once

#include <any>
#include <expected>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <typeindex>
#include "src/SharedLoader/SharedLoader.hpp"

namespace core {
    enum class ModuleManagerError {
        SUCCESS,
        FAILED_TO_LOAD_MODULE,
    };

    class ModuleManager {
        public:
            ModuleManagerError loadEngines() noexcept;

            template <typename Interface>
            Interface* get_Module()
            {
                auto it = this->_engines.find(std::type_index(typeid(Interface)));
                if (it == this->_engines.end())
                    return nullptr;
                return std::any_cast<std::shared_ptr<Interface>&>(it->second).get();
            }

        private:
            utils::SharedLoader _loader;
            std::map<std::type_index, std::any> _engines;

            ModuleManagerError _getModuleInterface();

            template <typename Interface>
            core::ModuleManagerError _loadModule(const std::string& pluginPath, const std::string& loadSymbol,
                                                 const std::string& getSymbol)
            {
                auto handle = this->_loader.load<std::shared_ptr<Interface>()>(pluginPath, loadSymbol, getSymbol);
                if (this->reportLoaderError(handle) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
                    return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

                auto factory = this->_loader.get<std::shared_ptr<Interface>()>(getSymbol);
                if (this->reportLoaderError(factory) == core::ModuleManagerError::FAILED_TO_LOAD_MODULE)
                    return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;

                this->_store(factory.value()());
                return core::ModuleManagerError::SUCCESS;
            }

            template <typename T, typename E>
            [[nodiscard]] ModuleManagerError reportLoaderError(std::expected<T, E> sharedLib)
            {
                if (!sharedLib) {
                    std::cerr << sharedLib.error() << std::endl;
                    return core::ModuleManagerError::FAILED_TO_LOAD_MODULE;
                }
                return core::ModuleManagerError::SUCCESS;
            }

            template <typename Interface>
            void _store(std::shared_ptr<Interface> engine)
            {
                this->_engines[std::type_index(typeid(Interface))] = std::move(engine);
            }
    };
} // namespace core

