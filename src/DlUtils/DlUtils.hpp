#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include "types/types.hpp"

namespace utils {
    class DlUtils {
        public:
            class DlUtilsError : public std::exception {
                public:
                    DlUtilsError(std::string msg) : _msg(std::move(msg)) {}

                    [[nodiscard]] const char* what() const noexcept override { return this->_msg.c_str(); };

                private:
                    std::string _msg;
            };

            void open(const std::string& pathToLib, const std::string& libName);
            void close(const std::string& libName);

        private:
            static common::ModuleType getModuleType(void* lib);
            std::unordered_map<std::string, void*> _loadedLib;
            std::unordered_map<std::string, void*> _functions;
    };

} // namespace utils
