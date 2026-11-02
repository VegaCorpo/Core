#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <interfaces/IInputsEngine.hpp>
#include <interfaces/IPhysicsEngine.hpp>
#include <interfaces/IRenderEngine.hpp>
#include <interfaces/IUIEngine.hpp>
#include <mutex>
#include <queue>
#include <types/RenderDataBuffer.hpp>
#include "src/Buffers/buffers.hpp"
#include "src/Interpolation/Snapshot.hpp"
#include "src/Interpolation/SnapshotInterpolator.hpp"
#include "src/ModuleManager/ModuleManager.hpp"
#include "src/Timing/SimulationClock.hpp"
#include "src/Timing/TickPacer.hpp"
#include "types/World.hpp"

namespace core {
    inline constexpr double SECONDS_PER_DAY = 86400.0;
    inline constexpr double DEFAULT_SIMULATED_DAYS_PER_SECOND = 365.0;
    inline constexpr double DEFAULT_TIME_SCALE = DEFAULT_SIMULATED_DAYS_PER_SECOND * SECONDS_PER_DAY;
    inline constexpr std::chrono::milliseconds PHYSICS_TICK_PERIOD{16};
    inline constexpr std::chrono::milliseconds PHYSICS_TICK_BUDGET{12};
    inline constexpr std::chrono::milliseconds RENDER_FRAME_PERIOD{16};

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
            void _initUI();

            void _stepPhysics(common::IPhysicsEngine& physicsEngine, Seconds elapsed);
            void _syncPhysicsOut(common::IPhysicsEngine& physicsEngine);

            [[nodiscard]] bool _initRenderingModules(common::IRenderEngine& renderEngine, common::IUIEngine& uiEngine,
                                                     common::IInputsEngine& inputsEngine);

            void _renderFrame(common::IRenderEngine& renderEngine, common::IUIEngine& uiEngine,
                              common::IInputsEngine& inputsEngine);
            static void _dispatchActions(common::IInputsEngine& inputsEngine, common::IRenderEngine& renderEngine);
            void _applyPendingRenderBuffer(common::IRenderEngine& renderEngine);
            void _consumeSnapshot(common::IUIEngine& uiEngine);
            void _syncRenderedWorld(common::IRenderEngine& renderEngine);

            ModuleManager _moduleManager;
            entt::registry _registry;
            entt::dispatcher _dispatcher;

            std::atomic<bool> _rendererInit = false;

            std::condition_variable _renderInitCv;
            std::mutex _renderBufferMutex;
            std::mutex _registryMutex;
            std::mutex _initMutex;
            std::queue<common::RenderDataBuffer> _renderBufferQueue;

            std::atomic<bool> is_running = true;

            common::SpecificDataPhysics _specificDataPhysics;
            common::SpecificDataRender _specificDataRender;
            common::SpecificDataUI _specificDataUI;
            TripleBuffering<Snapshot> _snapshots;
            SimulationClock _simulationClock{DEFAULT_TIME_SCALE, PHYSICS_TICK_BUDGET};
            SnapshotInterpolator _interpolator;
    };
} // namespace core
