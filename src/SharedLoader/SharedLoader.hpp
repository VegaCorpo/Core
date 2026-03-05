#pragma once

#include <any>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "types/types.hpp"

namespace utils {
    class SharedLoader {
        public:
            class SharedLoaderError : public std::exception {
                public:
                    explicit SharedLoaderError(std::string msg) : _msg(std::move(msg)) {}

                    [[nodiscard]] const char* what() const noexcept override { return this->_msg.c_str(); };

                private:
                    std::string _msg;
            };

            template <typename T>
            void load(const std::string& pathToLib, const std::string& libName)
            {
                try {
                    boost::dll::fs::path lib_path = pathToLib;
                    auto module =
                        boost::dll::import_symbol<T>(lib_path, "get_engine", boost::dll::load_mode::append_decorations);

                    if (module == nullptr) {
                        throw SharedLoaderError("Unable to load lib");
                    }
                    this->_loadedLib[libName] = module;
                }
                catch (const boost::wrapexcept<boost::system::system_error>& error) {
                    throw SharedLoaderError(std::format("Library loading failed {}", error.what()));
                }
            };

            template <typename T>
            T& get(const std::string& libName)
            {
                if (this->_loadedLib.find(libName) == this->_loadedLib.end()) {
                    throw SharedLoaderError("Symbol not found");
                }
                return *std::any_cast<boost::shared_ptr<T>>(this->_loadedLib[libName]);
            }

        private:
            std::unordered_map<std::string, std::any> _loadedLib;
    };

} // namespace utils
