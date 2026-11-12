# Core architecture

```mermaid
flowchart TB
    classDef plugin fill:#dbeafe,stroke:#1d4ed8,color:#0f172a
    classDef core fill:#f1f5f9,stroke:#475569,color:#0f172a
    classDef thread fill:#dcfce7,stroke:#15803d,color:#0f172a
    classDef buffer fill:#fef3c7,stroke:#b45309,color:#0f172a
    classDef unwired fill:#ffffff,stroke:#9ca3af,stroke-dasharray:5 5,color:#4b5563

    MM["ModuleManager<br/>get_module_info() · get_engine()"]:::core
    LOADER["Loader"]:::plugin
    REG{"Registry"}:::core
    SDR(("Specific<br/>Render Data")):::core
    SDP(("Specific<br/>Physics Data")):::core
    SDU(("Specific<br/>UI Data")):::core

    PLOOP(["Physics thread<br/>TickPacer 16 ms<br/>SimulationClock, 12 ms budget"]):::thread
    PHYSICS["Physics"]:::plugin

    subgraph BUF["TripleBuffering#lt;Snapshot#gt;"]
        direction RL
        WS_W(("World State<br/>(write)")):::buffer
        WS_RDY(("World State<br/>(ready)")):::buffer
        WS_R(("World State<br/>(read)")):::buffer
        WS_W <-->|"publish()"| WS_RDY
        WS_RDY <-->|"tryConsume()"| WS_R
    end

    RLOOP(["Render thread<br/>TickPacer 16 ms"]):::thread
    INTERP["SnapshotInterpolator<br/>lerp positions"]:::core
    UI["UI"]:::plugin
    INPUTS["Inputs"]:::plugin
    RENDERER["Renderer"]:::plugin

    TODO["Planned but never called<br/>PhysicsSync::scatter · physics syncIn()<br/>publishDataUi()<br/>physicsPause() · physicsResume()"]:::unwired

    MM -->|"① loads all plugins"| LOADER
    LOADER -->|"② createScene()"| REG
    REG -->|"③ RenderSync::gather"| SDR
    REG -->|"③ PhysicsSync::gather"| SDP
    REG -->|"③ UISync::gather"| SDU
    SDP -->|"④ init()"| PHYSICS
    SDR -->|"⑤ init()"| RENDERER
    SDU -->|"⑤ init()"| UI

    PLOOP -->|"update(steps × stepDuration)"| PHYSICS
    PHYSICS -->|"publish()"| BUF
    BUF -->|"push()"| INTERP
    BUF -->|"update(raw state)"| UI
    INTERP -->|"syncIn(interpolated)"| RENDERER
    INPUTS -->|"poll() → handleActions()"| RENDERER
    UI -->|"drawn in render()"| RENDERER
    RLOOP -->|"update() · render()"| RENDERER

    UI ~~~ TODO

    SDR ~~~ PHYSICS
    SDU ~~~ PHYSICS
```

- **Blue**: plugins loaded from `plugins/` by `ModuleManager`.
- **Grey**: Core classes and data.
- **Green**: the two Core threads, each paced by a `TickPacer`. The render thread sets `is_running = false` when the window closes, which stops the physics thread.
- **Yellow**: the triple buffer shared by the two threads. Only the `ready` index is atomic, with a fresh bit.
- **Steps ① to ⑤**: initialization order. ① to ④ run on the main thread in `initializeCore()`, ⑤ at the start of the render thread.
- **Dashed box**: methods that exist in Core or in the interfaces but are never called. The registry is never updated after loading, and UI settings never reach Core.
