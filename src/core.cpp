#include <chrono>
#include <cstdio>
#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <entt/entity/fwd.hpp>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <thread>
#include "src/ModuleManager/ModuleManager.hpp"
#include "src/PhysicsSync/PhysicsSync.hpp"
#include "src/RenderSync/RenderSync.hpp"
#include "types/World.hpp"


#include <types/types.hpp>
#include "core.hpp"

core::SimulationState core::Simulation::initializeCore(const std::string& filename) noexcept
{
    if (this->_moduleManager._loadEngines() != core::ModuleManagerError::SUCCESS)
        return core::SimulationState::SHARED_LOADER_ERROR;

    if (this->_moduleManager._loaderEngine->createScene(&this->_registry, filename) != common::LoaderStatus::SUCCESS)
        return core::SimulationState::INITIALIZATION_ERROR;

    this->_initPhysics();
    this->_initRender();

    return core::SimulationState::OK;
}

void core::Simulation::launchSimulation()
{
    std::thread physicsThread(&core::Simulation::_launchPhysics, this);
    std::thread rendererThread(&core::Simulation::_launchRenderer, this);

    physicsThread.detach();
    rendererThread.detach();

    auto prev = std::chrono::high_resolution_clock::now();
    while (this->is_running) {
        auto time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = time - prev;
        prev = time;
        this->physicsAccumulator += deltaTime.count();
        this->rendererAccumulator += deltaTime.count();
        this->_uiAccumulator += deltaTime.count();
    }
}

void core::Simulation::_initPhysics()
{
    core::PhysicsSync::gather(this->_registry, this->_specificDataPhysics);
    this->_moduleManager._physicsEngine->init(this->_specificDataPhysics);
}

void core::Simulation::_initRender()
{
    core::RenderSync::gather(this->_registry, this->_specificDataRender);
}

void core::Simulation::_launchPhysics()
{
    while (this->is_running) {
        if (this->physicsAccumulator >= this->physicsThreshold) {
            this->physicsAccumulator -= this->physicsThreshold;
            this->_stepPhysics();
        }
    }
    this->_moduleManager._physicsEngine->shutdown();
}

void core::Simulation::_stepPhysics()
{
    this->_syncPhysicsIn();
    this->_moduleManager._physicsEngine->update(core::PHYSICS_DEV_TIME_STEP);
    this->_syncPhysicsOut();
}

void core::Simulation::_syncPhysicsIn()
{
    //{
    //    std::scoped_lock lock(this->_registryMutex);
    //    core::PhysicsSync::gather(this->_registry, this->_specificDataPhysics);
    //}
    //this->_moduleManager._physicsEngine->syncIn(this->_specificDataPhysics);
}

void core::Simulation::_syncPhysicsOut()
{
    common::WorldState &worldState = this->_worldState.getWriter();
    worldState = this->_moduleManager._physicsEngine->publish();
    this->_worldState.publish();
}

void core::Simulation::_launchRenderer()
{
    this->_moduleManager._renderEngine->init(this->_specificDataRender);
    this->_moduleManager._uiEngine->init(this->_moduleManager._renderEngine->getWindowHandle());

    this->_renderInitCv.notify_all();

    while (this->is_running) {
        if (!this->_moduleManager._renderEngine->isRunning()) {
            this->is_running = false;
            break;
        }

        if (this->rendererAccumulator >= this->rendererThreshold) {
            this->rendererAccumulator = 0;
            {
                std::scoped_lock lock(this->_renderBufferMutex);
                if (this->_renderBufferQueue.empty() == false) {
                    auto renderBuffer = this->_renderBufferQueue.front();
                    this->_moduleManager._renderEngine->setVertexBuffer(renderBuffer);
                    this->_renderBufferQueue.pop();
                }
            }

            // this->_renderEngine->setVertexBuffer(this->_renderBuffer);
            if (this->_worldState.tryConsume()) {
                const common::WorldState& state = this->_worldState.getReader();
                this->_moduleManager._renderEngine->syncIn(state);
            }
            this->_moduleManager._renderEngine->update();
            this->_moduleManager._renderEngine->render([this]() { this->_moduleManager._uiEngine->render(); });
        }
    }
}
