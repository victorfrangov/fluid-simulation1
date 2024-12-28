cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Download and install vcpkg
include(ExternalProject)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/vcpkg")
    execute_process(
        COMMAND git clone https://github.com/Microsoft/vcpkg.git
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    if(WIN32)
        execute_process(
            COMMAND ./vcpkg/bootstrap-vcpkg.bat
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    else()
        execute_process(
            COMMAND ./vcpkg/bootstrap-vcpkg.sh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endif()

# Install SDL3 packages based on platform
if(APPLE)
    execute_process(
        COMMAND ./vcpkg/vcpkg install sdl3:arm64-osx
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
elseif(WIN32)
    execute_process(
        COMMAND ./vcpkg/vcpkg install sdl3:x64-windows
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    execute_process(
        COMMAND ./vcpkg/vcpkg install sdl3:x64-linux
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

project ("FluidSimulation")
set(CMAKE_CXX_STANDARD 20)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add executable with conditional resource file
if(WIN32)
    add_executable(${PROJECT_NAME} ${SOURCES} res/resource.rc)
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# Add vcpkg toolchain file
include("vcpkg/scripts/buildsystems/vcpkg.cmake")

target_include_directories(${PROJECT_NAME} PRIVATE "include")

# Find SDL3 package
find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

# FetchContent for SDL3_image
include(FetchContent)
FetchContent_Declare(
  SDL3_image
  GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
  GIT_TAG main
)
FetchContent_MakeAvailable(SDL3_image)

# Add include directories for SDL3_image
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_image_SOURCE_DIR}/include)

# Link SDL3 and SDL3_image to your project
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_image::SDL3_image)

# Copy DLLs only on Windows
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:SDL3::SDL3>
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:SDL3_image::SDL3_image>
      $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )

endif()