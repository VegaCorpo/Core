#pragma once

#include <atomic>
#include <condition_variable>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <queue>
#include <types/RenderDataBuffer.hpp>
#include "src/Buffers/buffers.hpp"
#include "src/ModuleManager/ModuleManager.hpp"
#include "types/World.hpp"

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

            void _initPhysics();
            void _initRender();

            void _stepPhysics();

            void _syncPhysicsOut();

            ModuleManager _moduleManager;
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

            common::SpecificDataPhysics _specificDataPhysics;
            common::SpecificDataRender _specificDataRender;
            TripleBuffering<common::WorldState> _worldState;

    };
} // namespace core
