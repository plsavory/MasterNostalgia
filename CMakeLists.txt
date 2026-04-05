cmake_minimum_required(VERSION 3.15)
project(MasterNostalgia)

set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_NAME "MasterNostalgia")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

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

if (NOT ${GIT_BRANCH} STREQUAL "master")
    set(DISPLAY_BRANCH ON)
endif()

find_package(SDL3 REQUIRED)

add_executable(${EXECUTABLE_NAME}
        src/VDPDisplayMode.cpp
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
        src/Emulator.cpp
        src/MasterSystemZ80IO.cpp
        src/MasterSystemController.cpp
        src/MasterSystemInput.cpp
        src/Config.cpp
        src/PlayerControlConfig.cpp
        src/InputInterface.cpp
        src/SDLKeyboardStringMapper.cpp
        src/GeneralControlConfig.cpp
        src/SoundConfig.cpp
        src/include/SMSSaveState.h
        src/SMSSaveState.cpp
        src/include/SMSSaveStateStructs.h)

target_include_directories(${EXECUTABLE_NAME} PRIVATE src/include lib)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
        CURRENT_COMMIT=${GIT_COMMIT_HASH}
        CURRENT_BRANCH=${GIT_BRANCH}
        JSON_CONFIG_FILE
        $<$<BOOL:${DISPLAY_BRANCH}>:DISPLAY_BRANCH_IN_VERSION>)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE SDL3::SDL3)
