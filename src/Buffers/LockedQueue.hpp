#pragma once

#include <mutex>
#include <utility>
#include <vector>

namespace core {
    template<typename T>
    class LockedQueue {
        public:
            void push(T value)
            {
                const std::scoped_lock lock(this->_mutex);

                this->_items.push_back(std::move(value));
            }

            [[nodiscard]] std::vector<T> takeAll()
            {
                const std::scoped_lock lock(this->_mutex);

                return std::exchange(this->_items, {});
            }

        private:
            std::mutex _mutex;
            std::vector<T> _items;
    };
} // namespace core
