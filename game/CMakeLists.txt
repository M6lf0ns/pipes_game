cmake_minimum_required(VERSION 3.18)

# set variables for source files
set(SOURCE_FILES src/main.cpp
        src/Game.cpp
        src/Game.h
        src/rendering/Shader.cpp
        src/rendering/Shader.h
        src/entities/Entity.cpp
        src/entities/Entity.h
        src/entities/Ship.cpp
        src/entities/Ship.h
        src/Assets.h
        src/Sprite.cpp
        src/Sprite.h
        src/rendering/Texture.cpp
        src/rendering/Texture.h
        src/EntityType.h
        src/entities/Floor.cpp
        src/entities/Floor.h
        src/entities/Box.cpp
        src/entities/Box.h
        src/entities/Switch.cpp
        src/entities/Switch.h
        src/entities/Door.cpp
        src/entities/Door.h
        src/entities/SecBot.cpp
        src/entities/SecBot.h
        src/PhysicsCategories.h
        src/entities/Stick.cpp
        src/entities/Stick.h
        src/entities/Dog.cpp
        src/entities/Dog.h
        src/levels/Scene.cpp
        src/levels/Scene.h
        src/entities/entities.h
        src/entities/ladder.cpp
        src/entities/ladder.h
        src/entities/Speaker.cpp
        src/entities/Speaker.h
        src/entities/SpeakerSwitch.cpp
        src/entities/SpeakerSwitch.h
        src/entities/Vent.cpp
        src/entities/Vent.h
        src/entities/Wall.cpp
        src/entities/Wall.h
        src/entities/LaserGuy.cpp
        src/entities/LaserGuy.h
        src/physics/PhysicsUtils.h
        src/physics/PhysicsUtils.cpp
        src/entities/Mirror.cpp
        src/entities/Mirror.h
        src/entities/MirrorCube.cpp
        src/entities/MirrorCube.h
        src/entities/PipeSwitch.cpp
        src/entities/PipeSwitch.h
        src/entities/GoalSensor.cpp
        src/entities/GoalSensor.h
        src/entities/TutorialSign.cpp
        src/entities/TutorialSign.h

)

include_directories(${CMAKE_SOURCE_DIR}/extern)
# set executable name
set(EXE_FILE Game)

# add the executable target
add_executable(${EXE_FILE} ${SOURCE_FILES})

# require C++ 17 compiler
target_compile_features(${EXE_FILE} PRIVATE cxx_std_17)

# link with dependencies
target_link_libraries(${EXE_FILE} PRIVATE glad glfw glm soloud box2d winmm)  # <-- now also links glfw. The library names are separated by spaces.


target_compile_definitions(${EXE_FILE} PRIVATE ASSET_ROOT=./assets/)

add_custom_command(TARGET ${EXE_FILE} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying assets..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${EXE_FILE}>/assets)