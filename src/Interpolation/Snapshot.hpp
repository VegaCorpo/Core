#pragma once

#include <types/World.hpp>
#include "src/Timing/TickPacer.hpp"

namespace core {
    struct Snapshot {
            common::WorldState state;
            SteadyClock::time_point publishedAt;
    };
} // namespace core
