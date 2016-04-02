cmake_minimum_required(VERSION 2.8.7)
project(SeriousEngine)

# Set @rpath for Mac OS X shared library install names.
cmake_policy(SET CMP0042 NEW)

# Needed to find the SDL1.2 library files
find_package(SDL REQUIRED)

# Set up some sanity stuff...
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(LINUX TRUE)
endif()
if(APPLE)
    SET(MACOSX TRUE)
endif()
if(MSVC)
    SET(WINDOWS TRUE)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "None Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif()
SET(DEBUG FALSE)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(DEBUG TRUE)
endif()

add_definitions(-D_REENTRANT=1)
add_definitions(-D_MT=1)

if(DEBUG)
    add_definitions(-DDEBUG=1)
    add_definitions(-D_DEBUG=1)
    set(DEBUGSUFFIX "D")
else()
    add_definitions(-DNDEBUG=1)
    add_definitions(-D_NDEBUG=1)
    set(DEBUGSUFFIX "")
endif()

if(WINDOWS)
    add_definitions(-DPLATFORM_WIN32=1)
    add_definitions(-DPRAGMA_ONCE=1)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE=1)
endif()

if(LINUX)
    set(CMAKE_SKIP_RPATH ON CACHE BOOL "Skip RPATH" FORCE)
    add_definitions(-DPLATFORM_UNIX=1)
    add_definitions(-D_FILE_OFFSET_BITS=64)
    add_definitions(-D_LARGEFILE_SOURCE=1)
    add_definitions(-DPRAGMA_ONCE=1)
endif()

if(MACOSX)
    add_definitions(-DPLATFORM_UNIX=1)
    add_definitions(-DPLATFORM_MACOSX=1)
    add_definitions(-DPRAGMA_ONCE=1)
endif()

if(LINUX OR MACOSX)
    if(DEBUG)
        add_definitions(-O0 -ggdb3)
    endif(DEBUG)
    add_definitions(-pthread)
    add_definitions(-pipe -fsigned-char)
    #CXXFLAGS += -fexceptions -frtti
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # !!! FIXME: turn a bunch of these off, this is just for now. I hope.  --ryan.
    add_definitions(-Wno-unused-value)
    add_definitions(-Wno-switch)
    add_definitions(-Wno-tautological-undefined-compare)
    add_definitions(-Wno-c++11-compat-deprecated-writable-strings)
endif()

# !!! FIXME: you currently need this, but I'd like to flip this to not use
# !!! FIXME:  assembly language. And maybe delete the asm code too.
set(USE_I386_ASM TRUE)

if (USE_I386_ASM)
    # You need the Netwide Assembler (NASM) to build this on Intel systems.
    #   http://nasm.sf.net/
    add_definitions(-DUSE_I386_ASM=1)
    if (MACOSX)
        set(ASMOBJFMT "macho")
        list(APPEND ASMFLAGS --prefix _)
    elseif(WINDOWS)
        set(ASMOBJFMT "win32")
    else()
        set(ASMOBJFMT "elf")
    endif()
else()
    add_definitions(-DUSE_PORTABLE_C=1)
endif()


# !!! FIXME: I currently force this, but you shouldn't _have_ to.
add_definitions(-DSINGLE_THREADED=1)


include_directories(
    .
    External/SDL12
)

# We build ECC, then use it to generate C++ code for the game entities...
macro(add_parser_and_scanner _PARSER _SCANNER)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_SCANNER}.cpp"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_SCANNER}.l"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND flex
        ARGS -o${_SCANNER}.cpp ${_SCANNER}.l
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_PARSER}.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${_PARSER}.hpp"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_PARSER}.y"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND bison
        ARGS -o${_PARSER}.cpp ${_PARSER}.y -d
    )

    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_PARSER}.h"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_PARSER}.hpp"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${_PARSER}.hpp ${_PARSER}.h
    )
endmacro()

add_parser_and_scanner("Ecc/Parser" "Ecc/Scanner")
add_executable(ecc Ecc/Main.cpp Ecc/Parser.cpp Ecc/Parser.h Ecc/Scanner.cpp)

macro(entity _NAME)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}.h" "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}_tables.h"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}.es"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMAND ecc
        ARGS "${_NAME}.es"
    )
    list(APPEND ENTITIES_CPP "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}.cpp")
    list(APPEND ENTITIES_H "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}.h")
    list(APPEND ENTITIES_H "${CMAKE_CURRENT_SOURCE_DIR}/${_NAME}_tables.h")
endmacro()

set(ENTITIES_CPP "")
set(ENTITIES_H "")
entity(Engine/Classes/BaseEvents)
entity(Engine/Classes/MovableBrushEntity)
entity(Engine/Classes/MovableEntity)
entity(Engine/Classes/MovableModelEntity)
entity(Engine/Classes/PlayerEntity)
set(ENGINE_ENTITIES_CPP ${ENTITIES_CPP})

set(ENTITIES_CPP "")
entity(EntitiesMP/AirElemental)
entity(EntitiesMP/AirShockwave)
entity(EntitiesMP/AmmoItem)
entity(EntitiesMP/AmmoPack)
entity(EntitiesMP/AnimationChanger)
entity(EntitiesMP/AnimationHub)
entity(EntitiesMP/AreaMarker)
entity(EntitiesMP/ArmorItem)
entity(EntitiesMP/BackgroundViewer)
entity(EntitiesMP/BasicEffects)
entity(EntitiesMP/Beast)
entity(EntitiesMP/BigHead)
entity(EntitiesMP/BlendController)
entity(EntitiesMP/BloodSpray)
entity(EntitiesMP/Boneman)
entity(EntitiesMP/Bouncer)
entity(EntitiesMP/Bullet)
entity(EntitiesMP/Camera)
entity(EntitiesMP/CameraMarker)
entity(EntitiesMP/CannonBall)
entity(EntitiesMP/CannonRotating)
entity(EntitiesMP/CannonStatic)
entity(EntitiesMP/ChainsawFreak)
entity(EntitiesMP/Copier)
entity(EntitiesMP/Counter)
entity(EntitiesMP/CrateBus)
entity(EntitiesMP/CrateRider)
entity(EntitiesMP/CreditsHolder)
entity(EntitiesMP/Damager)
entity(EntitiesMP/Debris)
entity(EntitiesMP/DebugEntityStatesDisplay)
entity(EntitiesMP/Demon)
entity(EntitiesMP/DestroyableArchitecture)
entity(EntitiesMP/Devil)
entity(EntitiesMP/DevilMarker)
entity(EntitiesMP/DevilProjectile)
entity(EntitiesMP/DoorController)
entity(EntitiesMP/Dragonman)
entity(EntitiesMP/EffectMarker)
entity(EntitiesMP/Effector)
entity(EntitiesMP/Elemental)
entity(EntitiesMP/EnemyBase)
entity(EntitiesMP/EnemyCounter)
entity(EntitiesMP/EnemyDive)
entity(EntitiesMP/EnemyFly)
entity(EntitiesMP/EnemyMarker)
entity(EntitiesMP/EnemyRunInto)
entity(EntitiesMP/EnemySpawner)
entity(EntitiesMP/EnvironmentBase)
entity(EntitiesMP/EnvironmentMarker)
entity(EntitiesMP/EnvironmentParticlesHolder)
entity(EntitiesMP/Eruptor)
entity(EntitiesMP/ExotechLarva)
entity(EntitiesMP/ExotechLarvaBattery)
entity(EntitiesMP/ExotechLarvaCharger)
entity(EntitiesMP/Eyeman)
entity(EntitiesMP/Fireworks)
entity(EntitiesMP/Fish)
entity(EntitiesMP/Flame)
entity(EntitiesMP/FogMarker)
entity(EntitiesMP/Gizmo)
entity(EntitiesMP/Global)
entity(EntitiesMP/GradientMarker)
entity(EntitiesMP/GravityMarker)
entity(EntitiesMP/GravityRouter)
entity(EntitiesMP/Grunt)
entity(EntitiesMP/GruntSka)
entity(EntitiesMP/Guffy)
entity(EntitiesMP/HazeMarker)
entity(EntitiesMP/Headman)
entity(EntitiesMP/HealthItem)
entity(EntitiesMP/HudPicHolder)
entity(EntitiesMP/Item)
entity(EntitiesMP/KeyItem)
entity(EntitiesMP/LarvaOffspring)
entity(EntitiesMP/Light)
entity(EntitiesMP/Lightning)
entity(EntitiesMP/Marker)
entity(EntitiesMP/MessageHolder)
entity(EntitiesMP/MessageItem)
entity(EntitiesMP/MeteorShower)
entity(EntitiesMP/MirrorMarker)
entity(EntitiesMP/ModelDestruction)
entity(EntitiesMP/ModelHolder)
entity(EntitiesMP/ModelHolder2)
entity(EntitiesMP/ModelHolder3)
entity(EntitiesMP/MovingBrush)
entity(EntitiesMP/MovingBrushMarker)
entity(EntitiesMP/MusicChanger)
entity(EntitiesMP/MusicHolder)
entity(EntitiesMP/NavigationMarker)
entity(EntitiesMP/ParticlesHolder)
entity(EntitiesMP/Pendulum)
entity(EntitiesMP/PhotoAlbum)
entity(EntitiesMP/Pipebomb)
entity(EntitiesMP/Player)
entity(EntitiesMP/PlayerActionMarker)
entity(EntitiesMP/PlayerAnimator)
entity(EntitiesMP/PlayerMarker)
entity(EntitiesMP/PlayerView)
entity(EntitiesMP/PlayerWeapons)
entity(EntitiesMP/PlayerWeaponsEffects)
entity(EntitiesMP/PowerUpItem)
entity(EntitiesMP/Projectile)
entity(EntitiesMP/PyramidSpaceShip)
entity(EntitiesMP/PyramidSpaceShipMarker)
entity(EntitiesMP/Reminder)
entity(EntitiesMP/RollingStone)
entity(EntitiesMP/Santa)
entity(EntitiesMP/Scorpman)
entity(EntitiesMP/ScrollHolder)
entity(EntitiesMP/SeriousBomb)
entity(EntitiesMP/Ship)
entity(EntitiesMP/ShipMarker)
entity(EntitiesMP/Shooter)
entity(EntitiesMP/SoundHolder)
entity(EntitiesMP/SpawnerProjectile)
entity(EntitiesMP/Spinner)
entity(EntitiesMP/StormController)
entity(EntitiesMP/Summoner)
entity(EntitiesMP/SummonerMarker)
entity(EntitiesMP/Switch)
entity(EntitiesMP/TacticsChanger)
entity(EntitiesMP/TacticsHolder)
entity(EntitiesMP/Teleport)
entity(EntitiesMP/Terrain)
entity(EntitiesMP/TextFXHolder)
entity(EntitiesMP/TimeController)
entity(EntitiesMP/TouchField)
entity(EntitiesMP/Trigger)
entity(EntitiesMP/Twister)
entity(EntitiesMP/VoiceHolder)
entity(EntitiesMP/Walker)
entity(EntitiesMP/WatchPlayers)
entity(EntitiesMP/Watcher)
entity(EntitiesMP/Water)
entity(EntitiesMP/WeaponItem)
entity(EntitiesMP/Werebull)
entity(EntitiesMP/Woman)
entity(EntitiesMP/WorldBase)
entity(EntitiesMP/WorldLink)
entity(EntitiesMP/WorldSettingsController)

add_custom_target(ParseAllEntities DEPENDS ${ENTITIES_H})

set(ENTITIESMPLIB "EntitiesMP${DEBUGSUFFIX}")
add_library(${ENTITIESMPLIB} SHARED
    ${ENTITIES_CPP}
    EntitiesMP/Common/Common.cpp
	EntitiesMP/Common/Particles.cpp
	EntitiesMP/Common/EmanatingParticles.cpp
	EntitiesMP/Common/PathFinding.cpp
	EntitiesMP/Common/HUD.cpp
)
if(MACOSX)
    target_link_libraries(${ENTITIESMPLIB} "-undefined dynamic_lookup")
endif()
add_dependencies(${ENTITIESMPLIB} ParseAllEntities)

set(GAMEMPLIB "GameMP${DEBUGSUFFIX}")
add_library(${GAMEMPLIB} SHARED
    GameMP/Camera.cpp
    GameMP/CompMessage.cpp
    GameMP/CompModels.cpp
    GameMP/Computer.cpp
    GameMP/Console.cpp
    GameMP/Controls.cpp
    GameMP/Game.cpp
    GameMP/LCDDrawing.cpp
    GameMP/LoadingHook.cpp
    GameMP/Map.cpp
    GameMP/SessionProperties.cpp
    GameMP/WEDInterface.cpp
)
if(MACOSX)
    target_link_libraries(${GAMEMPLIB} "-undefined dynamic_lookup")
endif()
add_dependencies(${GAMEMPLIB} ParseAllEntities)

set(SHADERSLIB "Shaders${DEBUGSUFFIX}")
add_library(${SHADERSLIB} SHARED
    Shaders/AddShader.cpp
    Shaders/AddShaderDS.cpp
    Shaders/BaseShader.cpp
    Shaders/BaseShaderDS.cpp
    Shaders/BaseTransparent.cpp
    Shaders/BaseTransparentDS.cpp
    Shaders/ColorShader.cpp
    Shaders/Common.cpp
    Shaders/DetailShader.cpp
    Shaders/DisplaceShader.cpp
    Shaders/InvisibleShader.cpp
    Shaders/MultiLayerShader.cpp
    Shaders/Reflection.cpp
    Shaders/ReflectionDS.cpp
    Shaders/ReftectionAndSpecular.cpp
    Shaders/ReftectionAndSpecularDS.cpp
    Shaders/Specular.cpp
    Shaders/SpecularDS.cpp
    Shaders/StdH.cpp
    Shaders/Translucent.cpp
)
if(MACOSX)
    target_link_libraries(${SHADERSLIB} "-undefined dynamic_lookup")
endif()
add_dependencies(${SHADERSLIB} ParseAllEntities)


add_parser_and_scanner("Engine/Base/Parser" "Engine/Base/Scanner")
add_parser_and_scanner("Engine/Ska/smcPars" "Engine/Ska/smcScan")

if (USE_I386_ASM)
    add_custom_command(
        OUTPUT "SoundMixer386.o"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Sound/SoundMixer386.asm"
        COMMAND nasm
        ARGS ${ASMFLAGS} -f ${ASMOBJFMT} -o SoundMixer386.o ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Sound/SoundMixer386.asm
    )
    list(APPEND ADDITIONAL_ENGINE_SRCS SoundMixer386.o)
endif()

add_library(SeriousEngine STATIC
    ${ENGINE_ENTITIES_CPP}
    Engine/Engine.cpp
    Engine/Base/Anim.cpp
    Engine/Base/CRC.cpp
    Engine/Base/CRCTable.cpp
    Engine/Base/Changeable.cpp
    Engine/Base/Console.cpp
    Engine/Base/Directory.cpp
    Engine/Base/ErrorReporting.cpp
    Engine/Base/FileName.cpp
    Engine/Base/Input.cpp
    Engine/Base/Lists.cpp
    Engine/Base/Memory.cpp
    Engine/Base/Profiling.cpp
    Engine/Base/ProgressHook.cpp
    Engine/Base/Protection.cpp
    Engine/Base/Relations.cpp
    Engine/Base/ReplaceFile.cpp
    Engine/Base/Serial.cpp
    Engine/Base/Shell.cpp
    Engine/Base/ShellTypes.cpp
    Engine/Base/Statistics.cpp
    Engine/Base/Stream.cpp
    Engine/Base/Timer.cpp
    Engine/Base/Translation.cpp
    Engine/Base/Unzip.cpp
    Engine/Base/Updateable.cpp
    Engine/Base/CTString.cpp
    Engine/Base/Scanner.cpp
    Engine/Base/Parser.cpp
    Engine/Base/Parser.h
    Engine/Base/IFeel.cpp
    Engine/Base/Unix/UnixFileSystem.cpp
    Engine/Base/Unix/UnixDynamicLoader.cpp
    Engine/Base/SDL/SDLTimer.cpp
    Engine/Base/SDL/SDLInput.cpp
    Engine/Base/SDL/SDLEvents.cpp
    Engine/Base/NullSynchronization.cpp  # single threaded.
    #Engine/Base/Unix/UnixSynchronization.cpp  # multithreaded Unix.
    #Engine/Base/SDL/SDLThreadLocalStorage.cpp  # multithreaded Unix.
    #Engine/Base/Registry.cpp  # Windows only.
    #Engine/Base/StackDump.cpp  # Windows only.
    #Engine/Base/Win32/Win32Synchronization.cpp # Windows only.
    Engine/Brushes/Brush.cpp
    Engine/Brushes/BrushIO.cpp
    Engine/Brushes/BrushShadows.cpp
    Engine/Brushes/BrushTriangularize.cpp
    Engine/Brushes/BrushArchive.cpp
    Engine/Brushes/BrushImport.cpp
    Engine/Brushes/BrushMip.cpp
    Engine/Brushes/BrushPolygon.cpp
    Engine/Brushes/BrushExport.cpp
    Engine/Brushes/BrushSector.cpp
    Engine/Entities/Entity.cpp
    Engine/Entities/NearestPolygon.cpp
    Engine/Entities/EntityProperties.cpp
    Engine/Entities/PlayerCharacter.cpp
    Engine/Entities/EntityClass.cpp
    Engine/Entities/FieldBSPTesting.cpp
    Engine/Entities/EntityCollision.cpp
    Engine/Entities/EntityCopying.cpp
    Engine/Entities/LastPositions.cpp
    Engine/Math/Projection_Isometric.cpp
    Engine/Math/Object3D.cpp
    Engine/Math/Projection_Parallel.cpp
    Engine/Math/Projection_Perspective.cpp
    Engine/Math/Float.cpp
    Engine/Math/Object3D_CSG.cpp
    Engine/Math/Projection_Simple.cpp
    Engine/Math/Projection_Simple_DOUBLE.cpp
    Engine/Math/Functions.cpp
    Engine/Math/ObjectSector.cpp
    Engine/Math/Placement.cpp
    Engine/Math/TextureMapping.cpp
    Engine/Math/Geometry.cpp
    Engine/Math/Projection.cpp
    Engine/Math/Geometry_DOUBLE.cpp
    #Engine/Math/Object3D_IO.cpp  # Exploration 3D support.
    #Engine/Models/EditModel.cpp
    Engine/Models/Model.cpp
    Engine/Models/RenderModel_View.cpp
    Engine/Models/Normals.cpp
    Engine/Models/VertexGetting.cpp
    Engine/Models/RenderModel.cpp
    Engine/Models/MipMaker.cpp
    Engine/Models/ModelProfile.cpp
    Engine/Models/RenderModel_Mask.cpp
    Engine/Light/LayerMaker.cpp
    Engine/Light/LayerMixer.cpp
    Engine/Light/LightSource.cpp
    Engine/Graphics/Adapter.cpp
    Engine/Graphics/Raster.cpp
    Engine/Graphics/GfxLibrary.cpp
    Engine/Graphics/Benchmark.cpp
    Engine/Graphics/GfxProfile.cpp
    Engine/Graphics/Color.cpp
    Engine/Graphics/ShadowMap.cpp
    Engine/Graphics/DepthCheck.cpp
    Engine/Graphics/Texture.cpp
    Engine/Graphics/DisplayMode.cpp
    Engine/Graphics/Gfx_OpenGL.cpp
    Engine/Graphics/Gfx_OpenGL_Textures.cpp
    Engine/Graphics/TextureEffects.cpp
    Engine/Graphics/DrawPort.cpp
    Engine/Graphics/Gfx_wrapper.cpp
    Engine/Graphics/DrawPort_Particles.cpp
    Engine/Graphics/Graphics.cpp
    Engine/Graphics/ViewPort.cpp
    Engine/Graphics/DrawPort_RenderScene.cpp
    Engine/Graphics/ImageInfo.cpp
    Engine/Graphics/Fog.cpp
    Engine/Graphics/MultiMonitor.cpp
    Engine/Graphics/Font.cpp
    Engine/Graphics/Shader.cpp
    Engine/Graphics/Stereo.cpp
    Engine/Graphics/SDL/SDLOpenGL.cpp
    Engine/Graphics/SDL/SDLAdapter.cpp
    Engine/Network/ActionBuffer.cpp
    Engine/Network/NetworkMessage.cpp
    Engine/Network/Server.cpp
    Engine/Network/Buffer.cpp
    Engine/Network/NetworkProfile.cpp
    Engine/Network/SessionState.cpp
    Engine/Network/PlayerBuffer.cpp
    Engine/Network/MessageDispatcher.cpp
    Engine/Network/PlayerSource.cpp
    Engine/Network/Compression.cpp
    Engine/Network/Network.cpp
    Engine/Network/PlayerTarget.cpp
    Engine/Network/CPacket.cpp
    Engine/Network/ClientInterface.cpp
    Engine/Network/CommunicationInterface.cpp
    Engine/Network/Diff.cpp
    Engine/GameAgent/GameAgent.cpp
    Engine/Terrain/ArrayHolder.cpp
    Engine/Terrain/Terrain.cpp
    Engine/Terrain/TerrainArchive.cpp
    Engine/Terrain/TerrainEditing.cpp
    Engine/Terrain/TerrainLayer.cpp
    Engine/Terrain/TerrainMisc.cpp
    Engine/Terrain/TerrainRayCasting.cpp
    Engine/Terrain/TerrainRender.cpp
    Engine/Terrain/TerrainTile.cpp
    Engine/Rendering/Render.cpp
    Engine/Rendering/RenderProfile.cpp
    Engine/Rendering/SelectOnRender.cpp
    Engine/Ska/AnimSet.cpp
    Engine/Ska/RMRender.cpp
    Engine/Ska/Skeleton.cpp
    Engine/Ska/ModelInstance.cpp
    Engine/Ska/StringTable.cpp
    Engine/Ska/Mesh.cpp
    Engine/Ska/RMRenderMask.cpp
    Engine/Ska/smcPars.cpp
    Engine/Ska/smcPars.h
    Engine/Ska/smcScan.cpp
    Engine/Sound/SoundDecoder.cpp
    Engine/Sound/SoundObject.cpp
    Engine/Sound/SoundLibrary.cpp
    Engine/Sound/SoundProfile.cpp
    Engine/Sound/SoundData.cpp
    Engine/Sound/Wave.cpp
    Engine/Sound/SoundMixer.cpp
    Engine/Templates/Stock_CAnimData.cpp
    Engine/Templates/Stock_CAnimSet.cpp
    Engine/Templates/Stock_CEntityClass.cpp
    Engine/Templates/Stock_CMesh.cpp
    Engine/Templates/Stock_CModelData.cpp
    Engine/Templates/Stock_CSkeleton.cpp
    Engine/Templates/Stock_CSoundData.cpp
    Engine/Templates/Stock_CTextureData.cpp
    Engine/Templates/Stock_CShader.cpp
    Engine/Templates/NameTable_CTFileName.cpp
    Engine/Templates/NameTable_CTranslationPair.cpp
    Engine/Templates/BSP.cpp
    Engine/World/WorldCSG.cpp
    Engine/World/PhysicsProfile.cpp
    Engine/World/WorldCollision.cpp
    Engine/World/WorldIO.cpp
    Engine/World/WorldRayCasting.cpp
    Engine/World/World.cpp
    Engine/World/WorldCollisionGrid.cpp
    Engine/World/WorldEditingProfile.cpp
    Engine/zlib/inflate.c
    Engine/zlib/adler32.c
    Engine/zlib/infblock.c
    Engine/zlib/inffast.c
    Engine/zlib/inftrees.c
    Engine/zlib/trees.c
    Engine/zlib/compress.c
    Engine/zlib/zutil.c
    Engine/zlib/deflate.c
    Engine/zlib/infcodes.c
    Engine/zlib/infutil.c
    Engine/zlib/uncompr.c
    ${ADDITIONAL_ENGINE_SRCS}
)
add_dependencies(SeriousEngine ParseAllEntities)

add_executable(SeriousSam
    SeriousSam/LevelInfo.cpp
    SeriousSam/CmdLine.cpp
    SeriousSam/SeriousSam.cpp
    SeriousSam/VarList.cpp
    SeriousSam/Credits.cpp
    SeriousSam/Menu.cpp
    SeriousSam/GLSettings.cpp
    SeriousSam/MenuGadgets.cpp
    SeriousSam/LCDDrawing.cpp
    SeriousSam/MenuPrinting.cpp
    SeriousSam/SplashScreen.cpp
    SeriousSam/MainWindow.cpp
)
target_link_libraries(SeriousSam ${CMAKE_BINARY_DIR}/libSeriousEngine.a ${SDL_LIBRARY})
add_dependencies(SeriousSam ParseAllEntities)

add_executable(SeriousSamDedicated
    DedicatedServer/DedicatedServer.cpp
)
target_link_libraries(SeriousSamDedicated ${CMAKE_BINARY_DIR}/libSeriousEngine.a ${SDL_LIBRARY})
add_dependencies(SeriousSamDedicated ParseAllEntities)

if(MACOSX)
    find_library(COCOA_FRAMEWORK Cocoa)
    target_link_libraries(SeriousSam "${COCOA_FRAMEWORK}")
    target_link_libraries(SeriousSam "${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx/libSDL-1.2.0.dylib")
    target_link_libraries(SeriousSam "${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx/libSDLmain.a")
    target_link_libraries(SeriousSamDedicated "${COCOA_FRAMEWORK}")
    target_link_libraries(SeriousSamDedicated "${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx/libSDL-1.2.0.dylib")
    target_link_libraries(SeriousSamDedicated "${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx/libSDLmain.a")
endif()

if(LINUX)
    set_target_properties(SeriousSam PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN")
    target_link_libraries(SeriousSam "m")
    target_link_libraries(SeriousSam "dl")
    target_link_libraries(SeriousSam "pthread")

    set_target_properties(SeriousSamDedicated PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN")
    target_link_libraries(SeriousSamDedicated "m")
    target_link_libraries(SeriousSamDedicated "dl")
    target_link_libraries(SeriousSamDedicated "pthread")
endif()

# end of CMakeLists.txt ...

