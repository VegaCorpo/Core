#include "SnapshotInterpolator.hpp"
#include <algorithm>
#include <utility>
#include <utils/interpolation.hpp>

void core::SnapshotInterpolator::push(const Snapshot& snapshot)
{
    this->_previous = std::exchange(this->_current, snapshot);
    this->_frame = snapshot.state;
    this->_blendable = this->_sameBodies();
}

const common::WorldState& core::SnapshotInterpolator::at(SteadyClock::time_point now)
{
    if (this->_blendable)
        this->_blendPositions(this->_progressAt(now));
    return this->_frame;
}

bool core::SnapshotInterpolator::_sameBodies() const
{
    if (!this->_previous.has_value() || !this->_current.has_value())
        return false;

    const common::WorldState& previous = this->_previous->state;
    const common::WorldState& current = this->_current->state;

    return previous.entitiesId == current.entitiesId && previous.positions.size() == current.positions.size();
}

double core::SnapshotInterpolator::_progressAt(SteadyClock::time_point now) const
{
    const Seconds interval = this->_current->publishedAt - this->_previous->publishedAt;

    if (interval <= Seconds::zero())
        return FULL_PROGRESS;

    const Seconds sinceCurrent = now - this->_current->publishedAt;

    return std::clamp(sinceCurrent / interval, NO_PROGRESS, FULL_PROGRESS);
}

void core::SnapshotInterpolator::_blendPositions(double progress)
{
    const auto blend = [progress](const common::components::Position& from, const common::components::Position& to)
    { return common::interpolation::lerp(from, to, progress); };

    std::ranges::transform(this->_previous->state.positions, this->_current->state.positions,
                           this->_frame.positions.begin(), blend);
}
