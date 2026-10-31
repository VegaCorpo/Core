#pragma once

#include <chrono>

namespace core {
    using Seconds = std::chrono::duration<double>;
    using SteadyClock = std::chrono::steady_clock;

    class TickPacer {
        public:
            explicit TickPacer(SteadyClock::duration period);

            void waitNextTick();

            [[nodiscard]] Seconds lastInterval() const noexcept { return this->_lastInterval; }

        private:
            SteadyClock::duration _period;
            SteadyClock::time_point _nextTick;
            SteadyClock::time_point _previousTick;
            Seconds _lastInterval{};
    };
} // namespace core
