add_executable(LunaPlusClient
    main.cpp
    rendering/SceneRenderer.cpp
    rendering/WorldRenderer.cpp
    rendering/TerrainRenderer.cpp
    rendering/PropRenderer.cpp
    rendering/CharacterRenderer.cpp
    rendering/CameraSystem.cpp
    rendering/UIRenderer.cpp
    rendering/FontManager.cpp
    rendering/ParticleRenderer.cpp
    rendering/PostProcessor.cpp
    rendering/RenderQueue.cpp
    network/NetworkClient.cpp
    audio/AudioManager.cpp
    ui/ScreenManager.cpp
    ui/ClientFlow.cpp
    ui/Window.cpp
    ui/WindowManager.cpp
    ui/widgets/Button.cpp
    ui/widgets/Label.cpp
    ui/widgets/InputField.cpp
    ui/widgets/ProgressBar.cpp
    ui/widgets/Grid.cpp
    ui/widgets/ListBox.cpp
    ui/widgets/TabPanel.cpp
    ui/widgets/ComboBox.cpp
    ui/widgets/CheckBox.cpp
    ui/widgets/Icon.cpp
    ui/widgets/ToggleButton.cpp
    ui/widgets/GaugeBar.cpp
    ui/widgets/ScriptSprite.cpp
    ui/UiScriptWidgetBuilder.cpp
    ui/UiCaptureMode.cpp
    ui/LegacyHudOverlay.cpp
    ui/ScriptDialogPanel.cpp
    ui/NoticeView.cpp
    ui/UiTabSheetController.cpp
    ui/UiTooltip.cpp
    util/ScreenshotCapture.cpp
    ui/screens/LoginScreen.cpp
    ui/screens/CharSelectScreen.cpp
    ui/screens/LoadingScreen.cpp
    ui/screens/GameScreen.cpp
    ui/dialogs/InventoryDialog.cpp
    ui/dialogs/SkillDialog.cpp
    ui/dialogs/QuestDialog.cpp
    ui/dialogs/NPCDialog.cpp
    ui/dialogs/ChatPanel.cpp
    ui/dialogs/PartyDialog.cpp
    ui/dialogs/GuildDialog.cpp
    ui/dialogs/GuildWarehouseDialog.cpp
    ui/dialogs/FriendDialog.cpp
    effects/EffectManager.cpp
    effects/WeatherSystem.cpp
    gameobjects/Hero.cpp
    gameobjects/Monster.cpp
    gameobjects/Pet.cpp
    gameobjects/NPC.cpp
    gameobjects/ObjectBalloon.cpp
    gameobjects/NpcDialogSystem.cpp
    gameobjects/BuffSystem.cpp
    gameobjects/ProjectileSystem.cpp
    gameobjects/NavMeshSystem.cpp
    gameobjects/ConsignmentSystem.cpp
    gameobjects/DurabilitySystem.cpp
    gameobjects/FarmSystem.cpp
    gameobjects/FamilySystem.cpp
    gameobjects/DungeonSystem.cpp
    gameobjects/SiegeSystem.cpp
    gameobjects/TournamentSystem.cpp
    gameobjects/CostumeSystem.cpp
    gameobjects/CashShopSystem.cpp
    gameobjects/TradingSystem.cpp
    gameobjects/TelemetrySystem.cpp
    gameobjects/PatchSystem.cpp
    gameobjects/PersistenceManager.cpp
    gameobjects/AssetPreloader.cpp
    gameobjects/EconomyMonitor.cpp
    gameobjects/OperationsSystem.cpp
    gameobjects/TriggerSystem.cpp
    effects/EftParser.cpp
    engine/EngineSky.cpp
    engine/ServerAuthMovement.cpp
    audio/AnimationSfxSync.cpp
    audio/AmbientSystem.cpp
    network/Launcher.cpp
    network/PacketCrypto.cpp
    ui/dialogs/TradeDialog.cpp
    ui/dialogs/ConsignmentDialog.cpp
    ui/dialogs/MailDialog.cpp
    ui/dialogs/KeyBindDialog.cpp
    ui/dialogs/CharacterDialog.cpp
    ui/dialogs/PetDialog.cpp
    ui/dialogs/CookingDialog.cpp
    ui/dialogs/MountDialog.cpp
    ui/dialogs/TournamentDialog.cpp
    ui/dialogs/AvatarDialog.cpp
    ui/dialogs/UpgradeDialog.cpp
    ui/dialogs/MacroDialog.cpp
    ui/dialogs/ChatRoomDialog.cpp
    ui/dialogs/DateMatchingDialog.cpp
    ui/dialogs/FamilyDialog.cpp
    ui/dialogs/FarmGetDialog.cpp
    ui/dialogs/HousingDialog.cpp
    ui/dialogs/DungeonDialog.cpp
    ui/dialogs/CostumeDialog.cpp
    ui/dialogs/CashShopDialog.cpp
    ui/dialogs/FishingDialog.cpp
    ui/dialogs/CharMakeDlg.cpp
    ui/dialogs/WorldMapDlg.cpp
    ui/dialogs/MiniMapDlg.cpp
    ui/dialogs/HelperDlg.cpp
    ui/dialogs/FadeDlg.cpp
    ui/dialogs/GameOutDialog.cpp
    ui/dialogs/PKManagerDlg.cpp
    ui/dialogs/WeatherDialog.cpp
    ui/dialogs/ApplyOptionDialog.cpp
    ui/dialogs/AutoAnswerDlg.cpp
    ui/dialogs/ChallengeZoneClearNo1Dlg.cpp
    ui/dialogs/ChatRoomCreateDlg.cpp
    ui/dialogs/ChatRoomJoinDlg.cpp
    ui/dialogs/ChatRoomOptionDlg.cpp
    ui/dialogs/FarmAnimalCageDlg.cpp
    ui/dialogs/GuildLevelUp.cpp
    ui/dialogs/GuildRank.cpp
    ui/dialogs/HouseNameDlg.cpp
    ui/dialogs/ItemPopupDlg.cpp
    ui/dialogs/OptionsDialog.cpp
    ui/dialogs/PartyMatchingSetting.cpp
    ui/dialogs/PartySeekMemberDlg.cpp
    ui/dialogs/ReinforceGuideDialog.cpp
    ui/dialogs/StorageDialog.cpp
    ui/dialogs/SystemMsgDialog.cpp
    ui/dialogs/ShoutDialog.cpp
    ui/dialogs/BillingDialog.cpp
    ui/dialogs/HousingWebDialog.cpp
    ui/dialogs/TutorialBtnDlg.cpp
    engine/GraphicEngine.cpp
    engine/EngineCamera.cpp
    engine/EngineMap.cpp
    engine/SceneLoader.cpp
    input/InputSystem.cpp
    input/Keyboard.cpp
    input/Mouse.cpp
    input/UserInput.cpp
    input/MouseCursor.cpp
    config/ConfigManager.cpp
    config/Paths.cpp
    config/ModelResolver.cpp
    config/KeyBindings.cpp
    ui/skin/UiSkinManager.cpp
    ui/UiScriptParser.cpp
    ui/UiAtlasRegistry.cpp
    ui/UiStringTable.cpp
    ui/UiSoundIndex.cpp
)

target_include_directories(LunaPlusClient PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include/external
    ${CMAKE_SOURCE_DIR}/third_party/bsdiff
    ${CMAKE_BINARY_DIR}/game/fbs
)

target_link_libraries(LunaPlusClient PRIVATE
    game_core
    gx_render
    gx_geom
    gx_physics
    soundlib
    glfw
    bgfx::bgfx
    glm::glm
    spdlog::spdlog
    fmt::fmt
    EnTT::EnTT
    flatbuffers::flatbuffers
    assimp::assimp
    SQLite::SQLite3
    BZip2::BZip2
    luna_bspatch
)

if(APPLE)
    # Frameworks are linked via bgfx::bgfx and Jolt::Jolt INTERFACE_LINK_OPTIONS
endif()

target_compile_definitions(LunaPlusClient PRIVATE
    ASSETS_PATH="${CMAKE_SOURCE_DIR}/assets/"
    GAME_DATA_PATH="${CMAKE_SOURCE_DIR}/assets/data/game_data.db"
)

# ── Asset deployment ─────────────────────────────────────────────────────────
# Create a symlink from bin/assets → <source>/assets so the client can find
# assets via relative "./assets/" regardless of which dir it was launched from.
# On Windows a symlink needs elevated permissions; use a directory junction
# instead (cmake -E create_symlink on Windows creates a junction for dirs).
set(ASSET_SRC  "${CMAKE_SOURCE_DIR}/assets")
set(ASSET_LINK "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

if(NOT EXISTS "${ASSET_LINK}")
    add_custom_command(TARGET LunaPlusClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${ASSET_SRC}" "${ASSET_LINK}"
        COMMENT "Linking assets → ${ASSET_LINK}"
        VERBATIM
    )
endif()

# Likewise for shaders: bin/shaders is already handled by ShaderCompile.cmake,
# but ensure source shaders dir is also reachable for platforms where shaderc
# isn't run at build time (the .bin files are pre-compiled in source tree).
set(SHADER_SRC  "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_LINK "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders_src")
if(NOT EXISTS "${SHADER_LINK}")
    add_custom_command(TARGET LunaPlusClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink "${SHADER_SRC}" "${SHADER_LINK}"
        COMMENT "Linking source shaders → ${SHADER_LINK}"
        VERBATIM
    )
endif()

# ── macOS app bundle resources ───────────────────────────────────────────────
# When building with Xcode or packaging as a .app, add assets as Resources.
if(APPLE AND CMAKE_GENERATOR STREQUAL "Xcode")
    set_target_properties(LunaPlusClient PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in"
    )
    # Copy the assets directory into the .app bundle Resources/
    add_custom_command(TARGET LunaPlusClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${ASSET_SRC}" "$<TARGET_BUNDLE_CONTENT_DIR:LunaPlusClient>/Resources/assets"
        COMMENT "Copying assets into .app bundle"
        VERBATIM
    )
endif()
