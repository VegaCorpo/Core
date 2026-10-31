#include <chrono>
#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <cstddef>
#include <entt/entity/fwd.hpp>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <thread>
#include "interfaces/ILoaderEngine.hpp"
#include "src/ModuleManager/ModuleManager.hpp"
#include "src/PhysicsSync/PhysicsSync.hpp"
#include "src/RenderSync/RenderSync.hpp"
#include "src/UISync/UISync.hpp"
#include "types/World.hpp"

#include <types/types.hpp>
#include "core.hpp"

core::SimulationState core::Simulation::initializeCore(const std::string& filename) noexcept
{
    if (this->_moduleManager.loadEngines() != core::ModuleManagerError::SUCCESS)
        return core::SimulationState::SHARED_LOADER_ERROR;

    if (this->_moduleManager.get_Module<common::ILoaderEngine>()->createScene(&this->_registry, filename) !=
        common::LoaderStatus::SUCCESS)
        return core::SimulationState::INITIALIZATION_ERROR;

    this->_initPhysics();
    this->_initRender();
    this->_initUI();

    return core::SimulationState::OK;
}

void core::Simulation::launchSimulation()
{
    std::thread physicsThread(&core::Simulation::_launchPhysics, this);
    std::thread rendererThread(&core::Simulation::_launchRenderer, this);

    rendererThread.join();
    physicsThread.join();
}

void core::Simulation::_initPhysics()
{
    core::PhysicsSync::gather(this->_registry, this->_specificDataPhysics);
    this->_moduleManager.get_Module<common::IPhysicsEngine>()->init(this->_specificDataPhysics);
}

void core::Simulation::_initRender()
{
    core::RenderSync::gather(this->_registry, this->_specificDataRender);
}

void core::Simulation::_initUI()
{
    core::UISync::gather(this->_registry, this->_specificDataUI);
}

void core::Simulation::_launchPhysics()
{
    auto* physicsEngine = this->_moduleManager.get_Module<common::IPhysicsEngine>();
    TickPacer pacer(PHYSICS_TICK_PERIOD);

    while (this->is_running) {
        pacer.waitNextTick();
        this->_stepPhysics(*physicsEngine, pacer.lastInterval());
    }
    physicsEngine->shutdown();
}

void core::Simulation::_stepPhysics(common::IPhysicsEngine& physicsEngine, Seconds elapsed)
{
    const double stepDuration = physicsEngine.stepDuration();
    const std::size_t steps = this->_simulationClock.stepsDue(elapsed, stepDuration);

    if (steps == 0)
        return;

    const SteadyClock::time_point start = SteadyClock::now();

    physicsEngine.update(stepDuration * static_cast<double>(steps));
    this->_simulationClock.recordCost(SteadyClock::now() - start, steps);
    this->_syncPhysicsOut(physicsEngine);
}

void core::Simulation::_syncPhysicsOut(common::IPhysicsEngine& physicsEngine)
{
    Snapshot& snapshot = this->_snapshots.getWriter();

    snapshot.state = physicsEngine.publish();
    snapshot.publishedAt = SteadyClock::now();
    this->_snapshots.publish();
}

void core::Simulation::_launchRenderer()
{
    auto* renderEngine = this->_moduleManager.get_Module<common::IRenderEngine>();
    auto* uiEngine = this->_moduleManager.get_Module<common::IUIEngine>();

    if (!renderEngine->init(this->_specificDataRender)) {
        this->is_running = false;
        return;
    }

    uiEngine->init(renderEngine->getWindowHandle(), this->_specificDataUI);

    this->_renderInitCv.notify_all();

    TickPacer pacer(RENDER_FRAME_PERIOD);

    while (this->is_running && renderEngine->isRunning()) {
        pacer.waitNextTick();
        this->_renderFrame(*renderEngine, *uiEngine);
    }
    this->is_running = false;
}

void core::Simulation::_renderFrame(common::IRenderEngine& renderEngine, common::IUIEngine& uiEngine)
{
    this->_applyPendingRenderBuffer(renderEngine);
    this->_consumeSnapshot(uiEngine);
    this->_syncRenderedWorld(renderEngine);
    renderEngine.update();
    renderEngine.render([&uiEngine]() { uiEngine.render(); });
}

void core::Simulation::_applyPendingRenderBuffer(common::IRenderEngine& renderEngine)
{
    const std::scoped_lock lock(this->_renderBufferMutex);

    if (this->_renderBufferQueue.empty())
        return;

    auto renderBuffer = this->_renderBufferQueue.front();

    renderEngine.setVertexBuffer(renderBuffer);
    this->_renderBufferQueue.pop();
}

void core::Simulation::_consumeSnapshot(common::IUIEngine& uiEngine)
{
    if (!this->_snapshots.tryConsume())
        return;

    const Snapshot& snapshot = this->_snapshots.getReader();

    this->_interpolator.push(snapshot);
    uiEngine.update(snapshot.state);
}

void core::Simulation::_syncRenderedWorld(common::IRenderEngine& renderEngine)
{
    if (!this->_interpolator.ready())
        return;
    renderEngine.syncIn(this->_interpolator.get_snapshot(SteadyClock::now()));
}
