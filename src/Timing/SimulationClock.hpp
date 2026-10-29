#pragma once

#include <cstddef>
#include "src/Timing/TickPacer.hpp"

namespace core {
    class SimulationClock {
        public:
            SimulationClock(double timeScale, Seconds budget);

            [[nodiscard]] std::size_t stepsDue(Seconds elapsed, double stepDuration);

            void recordCost(Seconds duration, std::size_t steps);

        private:
            static constexpr std::size_t MINIMUM_STEPS = 1;

            [[nodiscard]] std::size_t _affordableSteps() const;

            double _timeScale;
            Seconds _budget;
            Seconds _stepCost{};
            double _debt = 0.0;
    };
} // namespace core
