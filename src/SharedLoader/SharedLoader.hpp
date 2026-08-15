#pragma once

#include <any>
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <format>
#include <functional>
#include <expected>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

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
            std::expected<void, std::string> load(const std::string& pathToLib, const std::string& symbole, const std::string& libName)
            {
                try {
                    boost::dll::fs::path libPath = pathToLib;
                    auto sym =
                        boost::dll::import_symbol<T>(libPath, symbole, boost::dll::load_mode::append_decorations);

                    this->_loadedLib[libName] = std::function<T>(sym);
                }
                catch (const boost::wrapexcept<boost::system::system_error>& error) {
                    std::unexpected(std::format("Library loading failed {}", error.what()));
                }
                return {};
            };

            template <typename T>
            [[nodiscard]] std::expected<std::function<T>, std::string> get(const std::string& libName)
            {
                if (this->_loadedLib.find(libName) == this->_loadedLib.end()) {
                    return std::unexpected(std::format("Symbol not found {}", libName));
                }
                return std::any_cast<std::function<T>>(this->_loadedLib[libName]);
            }

            void getPlugins();

            void printAvailablePlugins();
        private:
            std::unordered_map<std::string, std::any> _loadedLib;
            std::map<std::string, std::vector<std::string>> _availabePlugins;
    };

} // namespace utils
