#include <chrono>
#include <components/acceleration.hpp>
#include <components/mass.hpp>
#include <components/position.hpp>
#include <components/velocity.hpp>
#include <entt/entity/fwd.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <thread>
#include "SharedLoader/SharedLoader.hpp"

#include <types/types.hpp>
#include "core.hpp"

core::SimulationState core::Simulation::initializeCore(const std::string& filename) noexcept
{
    this->_loader.load<common::LoaderStatus(void*, const std::string&)>("plugins/Loader/liborbital_loader",
                                                                        "createScene", "createScene");
    auto createScene = this->_loader.get<common::LoaderStatus(void*, const std::string&)>("createScene");
    if (createScene(&this->_registry, filename) != common::LoaderStatus::SUCCESS)
        return core::SimulationState::INITIALIZATION_ERROR;

    this->_loadEngines();
    return core::SimulationState::OK;
}

core::SimulationState core::Simulation::_loadEngines() noexcept
{
    this->_initPhysics();

    // this->_loader.load<std::unique_ptr<common::IUIEngine>()>("plugins/UI/liborbital_ui", "get_engine",
    // "get_ui_engine"); auto uiFactory = this->_loader.get<std::unique_ptr<common::IUIEngine>()>("get_ui_engine");
    // this->_uiEngine = uiFactory();

    return core::SimulationState::OK;
}

void core::Simulation::_initPhysics()
{
    this->_loader.load<void(void*, void*, double)>("plugins/Physics/liborbital_physics", "physicsUpdate",
                                                   "physicsUpdate");
    this->_loader.load<std::string()>("plugins/Physics/liborbital_physics", "getName", "getName");
    this->_loader.load<void(void*, void*)>("plugins/Physics/liborbital_physics", "physicsInit", "physicsInit");
    this->_loader.load<void(void*)>("plugins/Physics/liborbital_physics", "physicsSyncIn", "physicsSyncIn");
    this->_loader.load<void(void*)>("plugins/Physics/liborbital_physics", "physicsSyncOut", "physicsSyncOut");

    auto physicsInit = this->_loader.get<void(void*, void*)>("physicsInit");
    physicsInit(&this->_registry, &this->_dispatcher);
}

void core::Simulation::launchSimulation()
{
    std::thread physicsThread(&core::Simulation::_launchPhysics, this);
    std::thread rendererThread(&core::Simulation::_launchRenderer, this);
    // std::thread uiThread(&core::Simulation::_launchUI, this);

    physicsThread.detach();
    rendererThread.detach();
    // uiThread.detach();

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
    auto getName = this->_loader.get<std::string()>("getName");
    auto syncIn = this->_loader.get<void(void*)>("physicsSyncIn");
    auto syncOut = this->_loader.get<void(void*)>("physicsSyncOut");
    auto updatePhysics = this->_loader.get<void(void*, void*, double)>("physicsUpdate");
    auto accumulator = 0.f;

    while (this->is_running) {
        if (this->physicsAccumulator >= this->physicsThreshold) {
            accumulator = physicsAccumulator;
            syncIn(&this->_registry);
            updatePhysics(&this->_registry, &this->_dispatcher, 7200);
            syncOut(&this->_registry);
            std::cout << std::format("Physics elapsed time: {} ms", (this->physicsAccumulator - accumulator) * 1000)
                      << std::endl;
            this->physicsAccumulator = 0;
        }
    }
}

void core::Simulation::_launchRenderer()
{
    this->_loader.load<std::unique_ptr<common::IRenderEngine>()>("plugins/Renderer/liborbital_render", "get_engine",
                                                                 "get_render_engine");
    this->_loader.load<std::unique_ptr<common::IUIEngine>()>("plugins/Renderer/liborbital_render", "get_ui_engine",
                                                             "get_render_ui_engine");

    auto renderFactory = this->_loader.get<std::unique_ptr<common::IRenderEngine>()>("get_render_engine");
    auto renderUiFactory = this->_loader.get<std::unique_ptr<common::IUIEngine>()>("get_render_ui_engine");

    this->_renderEngine = renderFactory();
    this->_uiEngine = renderUiFactory();
    this->_renderEngine->init();
    this->_uiEngine->init(this->_renderEngine->getWindowHandle());

    this->_renderInitCv.notify_all();

    while (this->is_running) {
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
            this->_renderEngine->syncIn(this->_registry);
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
