cmake_minimum_required(VERSION 3.15)
project(MasterNostalgia)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_NAME "MasterNostalgia")

#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE Release)
#endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DCURRENT_COMMIT=${GIT_COMMIT_HASH}")
add_definitions("-DCURRENT_BRANCH=${GIT_BRANCH}")

if (NOT ${GIT_BRANCH} STREQUAL "master")
    add_definitions("-DDISPLAY_BRANCH_IN_VERSION")
endif()

add_definitions("-DJSON_CONFIG_FILE")

find_package(SDL3 REQUIRED)
include_directories(src/include lib ${SDL3_INCLUDE_DIRS})

add_executable(${EXECUTABLE_NAME}
        src/include/SDLKeyboardStringMapper.h
        src/include/Cartridge.h
        src/include/CPUZ80.h
        src/include/MasterSystem.h
        src/include/Memory.h
        src/include/ProjectInfo.h
        src/include/PSGChannel.h
        src/include/PSG.h
        src/include/Utils.h
        src/include/VDPDisplayMode.h
        src/VDPDisplayMode.cpp
        src/include/VDP.h
        src/Cartridge.cpp
        src/CPUInstructionHelpers.cpp
        src/CPUZ80.cpp
        src/CPUZ80StandardOpcodeHandlers.cpp
        src/CPUZ80ExtendedOpcodeHandlers.cpp
        src/CPUZ80BitOpcodeHandlers.cpp
        src/CPUZ80IndexOpcodeHandlers.cpp
        src/CPUZ80IndexBitOpcodeHandlers.cpp
        src/EntryPoint.cpp
        src/MasterSystem.cpp
        src/Memory.cpp
        src/PSGChannel.cpp
        src/PSG.cpp
        src/Utils.cpp
        src/VDP.cpp
        src/Z80InstructionNames.cpp
        src/include/Z80InstructionNames.h
        src/include/Exceptions.h
        src/include/Console.h
        src/include/Emulator.h
        src/Emulator.cpp
        src/include/Z80IO.h
        src/include/MasterSystemZ80IO.h
        src/MasterSystemZ80IO.cpp
        src/include/MasterSystemController.h
        src/MasterSystemController.cpp
        src/include/MasterSystemInput.h
        src/MasterSystemInput.cpp
        src/Config.cpp
        src/PlayerControlConfig.cpp
        src/InputInterface.cpp
        src/SDLKeyboardStringMapper.cpp
        src/GeneralControlConfig.cpp
        src/SoundConfig.cpp)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE SDL3::SDL3)