#include <engine/gx_render/RenderDevice.h>
#include <engine/GraphicEngine.hpp>
#include <engine/EngineCamera.hpp>
#include <engine/EngineMap.hpp>
#include <engine/EngineSky.hpp>
#include <network/NetworkClient.hpp>
#include <audio/AudioManager.hpp>
#include <rendering/UIRenderer.hpp>
#include <rendering/ParticleRenderer.hpp>
#include <ecs/systems/ParticleSystem.hpp>
#include <ecs/systems/SpawnSystem.hpp>
#include <ecs/systems/GameDataDB.hpp>
#include <ui/GameState.hpp>
#include <ui/ScreenManager.hpp>
#include <ui/screens/LoginScreen.hpp>
#include <ui/screens/CharSelectScreen.hpp>
#include <ui/screens/LoadingScreen.hpp>
#include <ui/screens/GameScreen.hpp>
#include <rendering/CharacterRenderer.hpp>
#include <ui/Localization.hpp>
#include <gameobjects/ClassAdvancement.hpp>
#include <gameobjects/PersistenceManager.hpp>
#include <gameobjects/AssetPreloader.hpp>
#include <gameobjects/PatchSystem.hpp>
#include <engine/CollisionSystem.hpp>
#include <effects/WeatherSystem.hpp>
#include <audio/AmbientSystem.hpp>
#include <input/InputSystem.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <input/UserInput.hpp>
#include <input/MouseCursor.hpp>
#include <config/ConfigManager.hpp>
#include <config/KeyBindings.hpp>
#include <ui/skin/UiSkinManager.hpp>
#include <ui/UiFunctionRegistry.hpp>
#include <game/network/PacketDispatcher.hpp>
#include <spdlog/spdlog.h>
#include <csignal>
#include <chrono>
#include <flatbuffers/flatbuffers.h>
#include <Chat_generated.h>
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>

#include <game/Log.h>
#include <sys/stat.h>

static bool g_running = true;
void signal_handler(int) { g_running = false; }

static NetworkClient g_network;
static GameState g_state;
AudioManager* g_audio = nullptr;
static double g_last_mx = 0, g_last_my = 0;
static bool g_dragging = false;
static ScreenManager* g_screen_mgr = nullptr;
UIRenderer* g_ui = nullptr;

int main() {
    srand((unsigned int)time(nullptr));
    mkdir("logs", 0777);

    // 1. Logger / ConfigManager
    Luna::InitLog("client", "client.log");
    spdlog::info("LUNA Plus Reborn — v1.1.0");
    ConfigManager::Init();

    // 2. Input systems (before audio)
    Localization::Init();
    Keyboard::Init();
    Mouse::Init();
    MouseCursor::Init();
    UserInput::Init();
    UiSkinManager::Init();

    // --- NEW: Register Legacy UI Functions ---
    auto& ui_reg = Luna::UiFunctionRegistry::Get();
    ui_reg.Register("IN_DlgFunc", [](Widget* w, const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("UI Event: Inventory callback");
    });
    ui_reg.Register("CI_DlgFunc", [](Widget* w, const UIEvent& e) {
        if (e.type == UIEvent::Click) spdlog::info("UI Event: Character Info callback");
    });

    signal(SIGINT, signal_handler); signal(SIGTERM, signal_handler);

    // 3. AudioManager (before render device so BGM can start early)
    AudioManager audio;
    g_audio = &audio;
    if (!audio.Initialize()) {
        spdlog::warn("AudioManager: init failed, continuing without audio");
    }

    // 4. RenderDevice
    RenderDeviceConfig config{};
    config.width = 1280; config.height = 720;
    config.title = "LUNA Plus Reborn";
    config.vsync = false;
    RenderDevice device;
    if (!device.Init(config)) return 1;
    spdlog::info("Window Size: {}x{} | Framebuffer: {}x{}", config.width, config.height, device.GetWidth(), device.GetHeight());

    // 5. SceneRenderer (via GraphicEngine)
    GraphicEngine gfx; gfx.Init();
    gfx.GetScene()->width = (float)device.GetWidth();
    gfx.GetScene()->height = (float)device.GetHeight();
    EngineCamera cam; cam.Init(config.width, config.height);

    // 6. TerrainRenderer / PropRenderer
    TerrainRenderer terrain;
    PropRenderer props; props.Init();
    props.width = (float)device.GetWidth();
    props.height = (float)device.GetHeight();

    // 7. EngineMap (with Audio and DB integration)
    SpawnSystem spawn_sys;
    entt::registry registry;
    EngineMap map;
    map.SetTerrain(&terrain);
    map.SetProps(&props);
    map.SetAudio(&audio);
    map.SetSpawnSystem(&spawn_sys);
    map.SetRegistry(&registry);
    // GameDataDB — init before map load so EngineMap uses external DB
    GameDataDB gamedb;
    if (gamedb.Open("assets/data/game_data.db")) {
        gamedb.LoadMonsterTemplates();
        gamedb.LoadNPCTemplates();
        gamedb.LoadMapData();
        map.SetGameDataDB(&gamedb);
        spdlog::info("GameDataDB: initialized");
    } else {
        spdlog::warn("GameDataDB: failed to open data/game_data.db, using internal fallback");
    }
    // Initial map load from state
    if (g_state.map_id != 0) {
        map.Load(std::to_string(g_state.map_id));
    } else {
        map.Load("51"); // Default to Alker Plains
    }
    map.LoadFarmProps("assets/models/farm");

    // 8. EngineSky
    EngineSky sky;
    sky.Init();

    // 9. CharacterRenderer
    CharRenderer_Init();
    CharRenderer_SetFBSize((uint16_t)device.GetWidth(), (uint16_t)device.GetHeight());

    // 10. UIRenderer
    UIRenderer ui; ui.Init();
    g_ui = &ui;
    ui.width = (float)device.GetWidth();
    ui.height = (float)device.GetHeight();
    ui.logicalWidth = (float)device.GetLogicalWidth();
    ui.logicalHeight = (float)device.GetLogicalHeight();

    terrain.width = (float)device.GetWidth();
    terrain.height = (float)device.GetHeight();
    props.width = (float)device.GetWidth();
    props.height = (float)device.GetHeight();
    gfx.GetScene()->width = (float)device.GetWidth();
    gfx.GetScene()->height = (float)device.GetHeight();
    ParticleRenderer particles; particles.Init();

    if (g_audio) {
        std::string bgm_path = ASSETS_PATH + std::string("audio/BGM/BGM_Login.mp3");
        struct stat bgm_stat;
        if (::stat(bgm_path.c_str(), &bgm_stat) == 0) {
            audio.PlayBGM("BGM_Login");
        } else {
            spdlog::warn("BGM: login BGM not found at {}", bgm_path);
        }
    }

    AmbientSystem ambient;
    ambient.Init();

    // Persistence
    PersistenceManager persistence;
    std::string db_path = "character.db";
    if (persistence.Init(db_path)) {
        if (persistence.LoadGameState(g_state)) {
            spdlog::info("Main: loaded saved game (Lv.{}, {})", g_state.level, g_state.name);
        } else {
            spdlog::info("Main: no saved game found, starting fresh");
        }
    }

    // Asset Preloader
    AssetPreloader preloader;
    preloader.Init(&ui, &audio);
    preloader.ScanAssets();
    preloader.PreloadTextures();
    preloader.PreloadInterface();
    preloader.PreloadAudio();
    spdlog::info("AssetPreloader: {} textures, {} interface loaded", 
        preloader.GetTexturesLoaded(), preloader.GetInterfaceLoaded());

    // Patch System
    PatchSystem patcher;
    patcher.Init();
    if (patcher.IsUpdateAvailable()) {
        spdlog::info("PatchSystem: update v{} available (current: v{})", 
            patcher.GetLatestVersion().ToString(), patcher.GetCurrentVersion().ToString());
    }

    // Setup Screen Manager
    ScreenManager screenManager;
    g_screen_mgr = &screenManager;
    screenManager.Init(&g_state, &g_network);

    auto loginScreen = std::make_unique<LoginScreen>();
    loginScreen->SetSceneRenderer(gfx.GetScene());
    loginScreen->SetSceneClearer([&gfx](uint32_t c) { gfx.GetScene()->SetClearColor(c); });
    screenManager.Register("login", std::move(loginScreen));

    auto charSelectScreen = std::make_unique<CharSelectScreen>();
    charSelectScreen->SetSceneClearer([&gfx](uint32_t c) { gfx.GetScene()->SetClearColor(c); });
    screenManager.Register("charselect", std::move(charSelectScreen));

    screenManager.Register("loading", std::make_unique<LoadingScreen>());

    auto gameScreen = std::make_unique<GameScreen>();
    gameScreen->SetTerrain(&terrain);
    gameScreen->SetProps(&props);
    gameScreen->SetParticles(&particles);
    gameScreen->SetAudio(&audio);
    gameScreen->SetMap(&map);
    gameScreen->SetUI(&ui);
    screenManager.Register("game", std::move(gameScreen));

    // 12. Enter Login Screen
    screenManager.SwitchTo("login");

    static Luna::PacketDispatcher client_dispatcher;

    // --- Register Client Handlers ---
    client_dispatcher.Register(luna::protocol::PacketType_MP_ITEM_APPEARANCE_ADD, [&](uint32_t, uint16_t type, const std::vector<uint8_t>&) {
        spdlog::info("Equipment Update Triggered (type=0x{:04X})", type);
        g_state.equipment_dirty = true;
    });

    client_dispatcher.Register(luna::protocol::PacketType_MP_USERCONN_CHARACTER_TOTALINFO, [&](uint32_t, uint16_t type, const std::vector<uint8_t>&) {
        spdlog::info("Character Total Info Received (type=0x{:04X})", type);
        g_state.equipment_dirty = true;
    });

    // Network packet handler
    g_network.SetPacketHandler([&](uint16_t type, const std::vector<uint8_t>& payload) {
        spdlog::debug("Packet Received: type=0x{:04X} ({} bytes)", type, payload.size());

        client_dispatcher.Dispatch(0, type, payload);
        screenManager.HandlePacket(type, payload);

        if (type == luna::protocol::PacketType_MP_USERCONN_LOGIN_ACK && g_state.current_state == ClientState::Title) {
            g_state.current_state = ClientState::CharSelect;
            screenManager.SwitchTo("loading");
        }
    });
    g_state.current_state = ClientState::Title;
    screenManager.SwitchTo("login");

    auto* window = device.GetWindow();
    InputSystem input_sys;
    input_sys.Init(window);

    // Register GLFW callbacks via InputSystem
    glfwSetKeyCallback(window, InputSystem::GlfwKeyCallback);
    glfwSetCharCallback(window, InputSystem::GlfwCharCallback);
    glfwSetMouseButtonCallback(window, InputSystem::GlfwMouseButtonCallback);
    glfwSetCursorPosCallback(window, InputSystem::GlfwCursorPosCallback);
    glfwSetScrollCallback(window, InputSystem::GlfwScrollCallback);

    // Input event handlers
    input_sys.SetKeyCallback([&](const KeyEvent& e) {
        if (e.action != 1 && e.key != 259) return; // PRESS only, allow BACKSPACE repeat
        if (g_state.chat_open) {
            if (e.key == 257) {
                g_state.chat_open = false;
                if (!g_state.chat_input.empty() && g_network.IsConnected()) {
                    flatbuffers::FlatBufferBuilder fbb;
                    auto chatMsg = luna::protocol::CreateChatMessageDirect(fbb, 0, g_state.name.c_str(), g_state.chat_input.c_str(), luna::protocol::ChatChannel_All, 0);
                    fbb.Finish(chatMsg);
                    g_network.SendPacket(luna::protocol::PacketType_MP_CHAT_ALL_SYN, fbb.GetBufferPointer(), fbb.GetSize());
                }
                g_state.chat_input.clear();
                return;
            }
            if (e.key == 259 && !g_state.chat_input.empty()) { g_state.chat_input.pop_back(); return; }
            if (e.key == 256) { g_state.chat_open = false; return; }
            return;
        }
        if (g_screen_mgr) g_screen_mgr->HandleKey(e.key, e.scancode, e.action, e.mods);
    });

    input_sys.SetMouseCallback([&](const MouseEvent& e) {
        if (e.button == 0 && e.action == 1) {
            g_state.player_x = (float)(e.x - 640) / 12.0f;
            g_state.player_z = (float)(e.y - 360) / 12.0f;
        }
        if (e.button == 1 && e.action == 1) { g_dragging = true; g_last_mx = e.x; g_last_my = e.y; }
        if (e.button == 1 && e.action == 0) g_dragging = false;
    });

    using clock = std::chrono::steady_clock;
    auto last_time = clock::now();
    int frame = 0;
    float fps = 0, time = 0;

    while (!device.ShouldClose() && g_running) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - last_time).count();
        last_time = now; time += dt; frame++;

        device.BeginFrame();

        input_sys.Update(dt);
        Mouse::Update();

        {
            bool is_login = (screenManager.CurrentName() == "login" || 
                            screenManager.CurrentName() == "charselect");
            if (is_login) {
                cam.SetTarget(glm::vec3(0, 0, 0));
                cam.SetDistance(80.0f);
                cam.SetYaw(-45.0f);
                cam.SetPitch(-30.0f);
            } else {
                cam.SetTarget(glm::vec3(g_state.player_x, 0, g_state.player_z));
                cam.SetDistance(g_state.cam_dist);
                cam.SetYaw(g_state.cam_yaw);
                cam.SetPitch(g_state.cam_pitch);
            }
        }
        cam.SetShake(g_state.shake_x, g_state.shake_y);
        cam.Update(dt);
        
        // --- PROCESS NETWORK EVENTS (thread-safe) ---
        g_network.ProcessEvents();

        // --- GAME STATE UPDATE BEFORE RENDER ---
        screenManager.Update(dt);

        // Periodic auto-save
        persistence.Update(dt);
        if (frame % 1800 == 0) { // every ~30 seconds at 60fps
            persistence.SaveGameState(g_state);
        }

        // --- RENDERING ---
        {
            gfx.BeginFrame(cam.GetViewMatrix(), cam.GetProjectionMatrix(), sky.GetLightDirection());
            ambient.Update(dt, 51, 0.5f, g_state.player_x, 0, g_state.player_z);
            gfx.Render(&terrain, &props, nullptr, cam.GetViewMatrix(), cam.GetProjectionMatrix());
            gfx.RenderCharacters(time, cam.GetViewMatrix(), cam.GetProjectionMatrix());
        }
        ui.BeginFrame();
        screenManager.Render(ui, cam.GetViewMatrix(), cam.GetProjectionMatrix());

        if (frame % 30 == 0) fps = 1.0f / dt;
        // Hanya tampil fps di game screen, bukan login/charselect
        if (screenManager.CurrentName() == "game") {
            char fps_buf[32]; snprintf(fps_buf, sizeof(fps_buf), "FPS: %.0f", fps);
            ui.DrawText(1200, 2, 0xff888888, "%s", fps_buf);
        }

        device.EndFrame();
    }

    // Save game state on exit
    persistence.SaveGameState(g_state);
    persistence.Shutdown();

    g_network.Disconnect();
    g_audio = nullptr;
    ambient.Shutdown();
    audio.Shutdown(); CharRenderer_Shutdown();
    map.Unload(); props.Shutdown(); terrain.Shutdown(); gfx.Shutdown();
    ui.Shutdown(); particles.Shutdown(); device.Shutdown();
    spdlog::info("Shutdown");
    return 0;
}
