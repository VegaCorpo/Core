#pragma once

#include <any>
#include <functional>
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

            void open(const std::string& pathToLib, const std::string& libName);
            void close(const std::string& libName);

        private:
            // static common::ModuleType getModuleType(std::function<std::any> lib);

            std::unordered_map<std::string, std::function<std::any(std::any)>> _loadedLib;
            std::unordered_map<std::string, std::function<std::any(std::any)>> _functions;
    };

} // namespace utils
