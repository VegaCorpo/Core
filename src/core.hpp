#pragma once

#include <atomic>
#include <condition_variable>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <expected>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <types/RenderDataBuffer.hpp>
#include "src/PhysicsSync/PhysicsSync.hpp"
#include "src/SharedLoader/SharedLoader.hpp"

namespace core {
    inline constexpr double PHYSICS_DEV_TIME_STEP = 7200.0;

    enum class SimulationState {
        OK,
        INITIALIZATION_ERROR,
        SHARED_LOADER_ERROR,
        SIMULATION_ERROR,
    };

    class Simulation {
        public:
            core::SimulationState initializeCore(const std::string& filename) noexcept;
            void launchSimulation();

        private:
            void _launchPhysics();
            void _launchRenderer();
            void _launchUI();

            void _stepPhysics();

            void _syncPhysicsIn();

            void _syncPhysicsOut();

            template <typename T, typename E>
            [[nodiscard]] core::SimulationState reportLoaderError(std::expected<T, E> sharedLib)
            {
                if (!sharedLib) {
                    std::cerr << sharedLib.error() << std::endl;
                    return core::SimulationState::SHARED_LOADER_ERROR;
                }
                return core::SimulationState::OK;
            }

            SimulationState _loadEngines() noexcept;

            utils::SharedLoader _loader;

            std::unique_ptr<common::IPhysicsEngine> _physicsEngine = nullptr;
            std::unique_ptr<common::IUIEngine> _uiEngine = nullptr;
            std::unique_ptr<common::IRenderEngine> _renderEngine = nullptr;

            entt::registry _registry;
            entt::dispatcher _dispatcher;

            float physicsThreshold = 0.016;
            std::atomic<float> physicsAccumulator = 0.0;

            float rendererThreshold = 0.016;
            std::atomic<bool> _rendererInit = false;
            std::atomic<float> rendererAccumulator = 0.0;

            float _uiThreashold = 0.16;
            std::atomic<float> _uiAccumulator = 0.0;

            std::condition_variable _renderInitCv;
            std::mutex _renderBufferMutex;
            std::mutex _registryMutex;
            std::mutex _initMutex;
            std::queue<common::RenderDataBuffer> _renderBufferQueue;

            std::atomic<bool> is_running = true;

            common::WorldState _world_state;
    };
} // namespace core
