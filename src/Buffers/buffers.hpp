#pragma once

#include <array>
#include <atomic>
#include <cstdint>

namespace core {

    template<typename T>
    class TripleBuffering {
        public:
        const T &getReader() const {
            return this->_buffers[this->_read];
        }

        T &getWriter() {
            return this->_buffers[this->_write];
        }
    
        void publish() {
            uint8_t last = this->_ready.exchange(this->_write | FRESH_BIT);
            this->_write = last & INDEX_MASK;
        }
        
        bool tryConsume() {
            if (this->_ready.load() & FRESH_BIT) {
                uint8_t last = this->_ready.exchange(this->_read);
                this->_read = last & INDEX_MASK;
                return true;
            }
            return false;
        }

        private:
            static constexpr uint8_t FRESH_BIT  = 0b100;
            static constexpr uint8_t INDEX_MASK = 0b011;

            uint8_t _write = 0;  // Used by physics Thread uniquely
            std::atomic<uint8_t> _ready {1};
            uint8_t _read = 2;  // Used by render Thread uniquely
            std::array<T, 3> _buffers;
    };
}