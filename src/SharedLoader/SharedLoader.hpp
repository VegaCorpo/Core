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
            void load(const std::string& pathToLib, const std::string& symbole, const std::string& libName)
            {
                try {
                    boost::dll::fs::path libPath = pathToLib;
                    auto sym =
                        boost::dll::import_symbol<T>(libPath, symbole, boost::dll::load_mode::append_decorations);

                    this->_loadedLib[libName] = std::function<T>(sym);
                }
                catch (const boost::wrapexcept<boost::system::system_error>& error) {
                    throw SharedLoaderError(std::format("Library loading failed {}", error.what()));
                }
            };

            template <typename T>
            [[nodiscard]] std::function<T> get(const std::string& libName)
            {
                if (this->_loadedLib.find(libName) == this->_loadedLib.end()) {
                    throw SharedLoaderError("Symbol not found");
                }
                return std::any_cast<std::function<T>>(this->_loadedLib[libName]);
            }

        private:
            std::unordered_map<std::string, std::any> _loadedLib;
    };

} // namespace utils
