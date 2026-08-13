#include <chrono>
#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <entt/entity/fwd.hpp>
#include <expected>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <thread>
#include "SharedLoader/SharedLoader.hpp"

#include <types/types.hpp>
#include "core.hpp"

core::SimulationState core::Simulation::initializeCore(const std::string& filename) noexcept
{
    auto loader = this->_loader.load<common::LoaderStatus(void*, const std::string&)>(
        "plugins/Loader/liborbital_loader", "createScene", "createScene");
    if (this->reportLoaderError(loader) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    auto createScene = this->_loader.get<common::LoaderStatus(void*, const std::string&)>("createScene");
    if (this->reportLoaderError(createScene) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    if (createScene.value()(&this->_registry, filename) != common::LoaderStatus::SUCCESS)
        return core::SimulationState::INITIALIZATION_ERROR;

    if (this->_loadEngines() != core::SimulationState::OK)
        return core::SimulationState::SHARED_LOADER_ERROR;
    return core::SimulationState::OK;
}

core::SimulationState core::Simulation::_loadEngines() noexcept
{
    auto physics = this->_loader.load<std::unique_ptr<common::IPhysicsEngine>()>("plugins/Physics/liborbital_physics",
                                                                                 "get_engine", "get_physics_engine");

    auto render = this->_loader.load<std::unique_ptr<common::IRenderEngine>()>("plugins/Renderer/liborbital_render",
                                                                               "get_engine", "get_render_engine");

    auto ui = this->_loader.load<std::unique_ptr<common::IUIEngine>()>("plugins/Renderer/liborbital_render",
                                                                       "get_ui_engine", "get_render_ui_engine");

    if (this->reportLoaderError(physics) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    if (this->reportLoaderError(render) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    if (this->reportLoaderError(ui) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    auto physicsFactory = this->_loader.get<std::unique_ptr<common::IPhysicsEngine>()>("get_physics_engine");
    if (this->reportLoaderError(physicsFactory) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    this->_physicsEngine = physicsFactory.value()();
    this->_physicsEngine->init(this->_registry, this->_dispatcher);

    auto renderFactory = this->_loader.get<std::unique_ptr<common::IRenderEngine>()>("get_render_engine");
    auto renderUiFactory = this->_loader.get<std::unique_ptr<common::IUIEngine>()>("get_render_ui_engine");

    if (this->reportLoaderError(renderFactory) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    if (this->reportLoaderError(renderUiFactory) == core::SimulationState::SHARED_LOADER_ERROR)
        return core::SimulationState::SHARED_LOADER_ERROR;

    this->_renderEngine = renderFactory.value()();
    this->_uiEngine = renderUiFactory.value()();
    return core::SimulationState::OK;
}

void core::Simulation::launchSimulation()
{
    std::thread physicsThread(&core::Simulation::_launchPhysics, this);
    std::thread rendererThread(&core::Simulation::_launchRenderer, this);
    std::thread uiThread(&core::Simulation::_launchUI, this);

    physicsThread.detach();
    rendererThread.detach();
    uiThread.detach();

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

void core::Simulation::_launchPhysics()
{
    auto accumulator = 0.f;

    while (this->is_running) {
        if (this->physicsAccumulator >= this->physicsThreshold) {
            accumulator = physicsAccumulator;
            this->_physicsEngine->syncIn(this->_registry);
            this->_physicsEngine->update(this->_registry, this->_dispatcher, 7200);
            {
                std::scoped_lock lock(this->_registryMutex);
                this->_physicsEngine->syncOut(this->_registry);
            }
            // std::cout << std::format("Physics elapsed time: {} ms", (this->physicsAccumulator - accumulator) * 1000)
            //           << std::endl;
            this->physicsAccumulator -= this->physicsThreshold;
        }
    }
    this->_physicsEngine->shutdown(this->_registry);
}

void core::Simulation::_launchRenderer()
{
    this->_renderEngine->init();
    this->_uiEngine->init(this->_renderEngine->getWindowHandle());

    this->_renderInitCv.notify_all();

    while (this->is_running) {
        if (!this->_renderEngine->isRunning()) {
            this->is_running = false;
            break;
        }

        if (this->rendererAccumulator >= this->rendererThreshold) {
            this->rendererAccumulator = 0;
            {
                std::scoped_lock lock(this->_renderBufferMutex);
                if (this->_renderBufferQueue.empty() == false) {
                    auto renderBuffer = this->_renderBufferQueue.front();
                    this->_renderEngine->setVertexBuffer(renderBuffer);
                    this->_renderBufferQueue.pop();
                }
            }

            // this->_renderEngine->setVertexBuffer(this->_renderBuffer);
            {
                std::scoped_lock lock(this->_registryMutex);
                this->_renderEngine->syncIn(this->_registry);
            }
            this->_renderEngine->update();
            this->_renderEngine->render([this]() { this->_uiEngine->render(); });
        }
    }
}

void core::Simulation::_launchUI()
{
    std::unique_lock<std::mutex> lock(this->_initMutex);
    this->_renderInitCv.wait(lock);
    lock.unlock();

    while (this->is_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
