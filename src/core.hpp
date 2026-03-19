#pragma once

#include <condition_variable>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <exception>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <queue>
#include <types/RenderDataBuffer.hpp>
#include "src/SharedLoader/SharedLoader.hpp"

namespace core {
    enum class SimulationState {
        OK,
        INITIALIZATION_ERROR,
        SIMULATION_ERROR,
    };

    class Simulation {
        public:
            core::SimulationState initializeCore(const std::string& filename) noexcept;
            void launchSimulation();

        private:
            void _initPhysics();
            void _launchPhysics();
            void _launchRenderer();
            void _launchUI();

            SimulationState _loadEngines() noexcept;
            std::unique_ptr<common::IUIEngine> _uiEngine = nullptr;
            std::unique_ptr<common::IRenderEngine> _renderEngine = nullptr;

            utils::SharedLoader _loader;
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
            std::mutex _initMutex;
            std::queue<common::RenderDataBuffer> _renderBufferQueue;

            bool is_running = true;
    };
} // namespace core
