#pragma once

#include <optional>
#include <types/World.hpp>
#include "src/Interpolation/Snapshot.hpp"

namespace core {
    class SnapshotInterpolator {
        public:
            void push(const Snapshot& snapshot);

            [[nodiscard]] bool ready() const noexcept { return this->_current.has_value(); }

            [[nodiscard]] const common::WorldState& at(SteadyClock::time_point now);

        private:
            static constexpr double NO_PROGRESS = 0.0;
            static constexpr double FULL_PROGRESS = 1.0;

            [[nodiscard]] bool _sameBodies() const;
            [[nodiscard]] double _progressAt(SteadyClock::time_point now) const;
            void _blendPositions(double progress);

            std::optional<Snapshot> _previous;
            std::optional<Snapshot> _current;
            common::WorldState _frame;
            bool _blendable = false;
    };
} // namespace core
