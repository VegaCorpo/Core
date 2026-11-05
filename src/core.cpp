#include <chrono>
#include <cstddef>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <queue>
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
    if (this->_moduleManager.loadPluginsVersion() != core::ModuleManagerError::SUCCESS) {
        return core::SimulationState::SHARED_LOADER_ERROR;
    }

    if (this->_moduleManager.loadEngines() != core::ModuleManagerError::SUCCESS)
        return core::SimulationState::SHARED_LOADER_ERROR;

    const auto scene = this->_moduleManager.get_Module<common::ILoaderEngine>()->createScene(filename);

    if (!scene)
        return core::SimulationState::INITIALIZATION_ERROR;

    this->_initPhysics(*scene);
    this->_initRender(*scene);
    this->_initUI(*scene);

    return core::SimulationState::OK;
}

void core::Simulation::launchSimulation()
{
    std::thread physicsThread(&core::Simulation::_launchPhysics, this);
    std::thread rendererThread(&core::Simulation::_launchRenderer, this);

    rendererThread.join();
    physicsThread.join();
}

void core::Simulation::_initPhysics(const common::SceneDescription& scene)
{
    core::PhysicsSync::gather(scene, this->_specificDataPhysics);
    this->_moduleManager.get_Module<common::IPhysicsEngine>()->init(this->_specificDataPhysics);
}

void core::Simulation::_initRender(const common::SceneDescription& scene)
{
    core::RenderSync::gather(scene, this->_specificDataRender);
}

void core::Simulation::_initUI(const common::SceneDescription& scene)
{
    core::UISync::gather(scene, this->_specificDataUI);
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
    auto* inputsEngine = this->_moduleManager.get_Module<common::IInputsEngine>();

    if (!this->_initRenderingModules(*renderEngine, *uiEngine, *inputsEngine)) {
        this->is_running = false;
        return;
    }

    TickPacer pacer(RENDER_FRAME_PERIOD);

    while (this->is_running && renderEngine->isRunning()) {
        pacer.waitNextTick();
        this->_renderFrame(*renderEngine, *uiEngine, *inputsEngine);
    }
    this->is_running = false;
}

bool core::Simulation::_initRenderingModules(common::IRenderEngine& renderEngine, common::IUIEngine& uiEngine,
                                             common::IInputsEngine& inputsEngine)
{
    if (!renderEngine.init(this->_specificDataRender))
        return false;

    uiEngine.init(renderEngine.getWindowHandle(), this->_specificDataUI);
    return inputsEngine.init(renderEngine.getWindowHandle());
}

void core::Simulation::_renderFrame(common::IRenderEngine& renderEngine, common::IUIEngine& uiEngine,
                                    common::IInputsEngine& inputsEngine)
{
    this->_consumeSnapshot(uiEngine);
    this->_syncRenderedWorld(renderEngine);
    core::Simulation::_dispatchActions(inputsEngine, renderEngine);
    renderEngine.update();
    renderEngine.render([&uiEngine]() { uiEngine.render(); });
}

void core::Simulation::_dispatchActions(common::IInputsEngine& inputsEngine, common::IRenderEngine& renderEngine)
{
    std::queue<common::InputAction> actions = inputsEngine.poll();

    renderEngine.handleActions(actions);
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
