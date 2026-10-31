#include "SimulationClock.hpp"
#include <algorithm>
#include <cmath>

core::SimulationClock::SimulationClock(double timeScale, Seconds budget) : _timeScale(timeScale), _budget(budget)
{}

std::size_t core::SimulationClock::stepsDue(Seconds elapsed, double stepDuration)
{
    if (stepDuration <= 0.0 || !std::isfinite(stepDuration))
        return 0;
    this->_debt += elapsed.count() * this->_timeScale;

    const double owedSteps = std::floor(this->_debt / stepDuration);
    const auto steps = static_cast<std::size_t>(std::min(owedSteps, static_cast<double>(this->_affordableSteps())));

    this->_debt = std::min(this->_debt - static_cast<double>(steps) * stepDuration, stepDuration);
    return steps;
}

void core::SimulationClock::recordCost(Seconds duration, std::size_t steps)
{
    if (steps == 0)
        return;
    this->_stepCost = duration / static_cast<double>(steps);
}

std::size_t core::SimulationClock::_affordableSteps() const
{
    if (this->_stepCost <= Seconds::zero())
        return MINIMUM_STEPS;
    return std::max(MINIMUM_STEPS, static_cast<std::size_t>(this->_budget / this->_stepCost));
}
