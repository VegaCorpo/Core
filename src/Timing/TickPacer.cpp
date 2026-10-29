#include "TickPacer.hpp"
#include <algorithm>
#include <thread>

core::TickPacer::TickPacer(SteadyClock::duration period) :
    _period(period), _nextTick(SteadyClock::now()), _previousTick(_nextTick)
{}

void core::TickPacer::waitNextTick()
{
    std::this_thread::sleep_until(this->_nextTick);

    const SteadyClock::time_point now = SteadyClock::now();

    this->_lastInterval = now - this->_previousTick;
    this->_previousTick = now;
    this->_nextTick = std::max(this->_nextTick + this->_period, now);
}
