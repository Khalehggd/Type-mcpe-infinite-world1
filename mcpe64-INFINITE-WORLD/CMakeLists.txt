cmake_minimum_required(VERSION 3.21)
project(MinecraftPE)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# ─────────────────────────────────────────────
# Common sources shared by ALL platforms
# ─────────────────────────────────────────────
file(GLOB COMMON_SOURCES
    "src/client/*.cpp"
    "src/client/gamemode/*.cpp"
    "src/client/gui/*.cpp"
    "src/client/gui/components/*.cpp"
    "src/client/gui/screens/*.cpp"
    "src/client/gui/screens/crafting/*.cpp"
    "src/client/gui/screens/touch/*.cpp"
    "src/client/model/*.cpp"
    "src/client/model/geom/*.cpp"
    "src/client/particle/*.cpp"
    "src/client/player/*.cpp"
    "src/client/player/input/*.cpp"
    "src/client/player/input/touchscreen/*.cpp"
    "src/client/renderer/*.cpp"
    "src/client/renderer/culling/*.cpp"
    "src/client/renderer/entity/*.cpp"
    "src/client/renderer/ptexture/*.cpp"
    "src/client/renderer/tileentity/*.cpp"
    "src/client/sound/*.cpp"
    "src/locale/*.cpp"
    "src/nbt/*.cpp"
    "src/network/*.cpp"
    "src/network/command/*.cpp"
    "src/platform/*.cpp"
    "src/platform/input/*.cpp"
    "src/raknet/**.cpp"
    "src/server/**.cpp"
    "src/util/**.cpp"
    "src/world/*.cpp"
    "src/world/phys/*.cpp"
    "src/world/entity/*.cpp"
    "src/world/entity/ai/control/*.cpp"
    "src/world/entity/animal/*.cpp"
    "src/world/entity/item/*.cpp"
    "src/world/entity/monster/*.cpp"
    "src/world/entity/player/*.cpp"
    "src/world/entity/projectile/*.cpp"
    "src/world/food/*.cpp"
    "src/world/inventory/*.cpp"
    "src/world/item/*.cpp"
    "src/world/item/crafting/*.cpp"
    "src/world/level/*.cpp"
    "src/world/level/biome/*.cpp"
    "src/world/level/chunk/*.cpp"
    "src/world/level/dimension/*.cpp"
    "src/world/level/levelgen/*.cpp"
    "src/world/level/levelgen/feature/*.cpp"
    "src/world/level/levelgen/synth/*.cpp"
    "src/world/level/material/*.cpp"
    "src/world/level/pathfinder/*.cpp"
    "src/world/level/storage/*.cpp"
    "src/world/level/tile/*.cpp"
    "src/world/level/tile/entity/*.cpp"
    "src/SharedConstants.cpp"
    "src/NinecraftApp.cpp"
)

# Remove desktop-only platform files from the glob
list(REMOVE_ITEM COMMON_SOURCES
    "${CMAKE_SOURCE_DIR}/src/AppPlatform_glfw.cpp"
    "${CMAKE_SOURCE_DIR}/src/AppPlatform_win32.cpp"
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
)

# ═══════════════════════════════════════════════
#  ANDROID BUILD
# ═══════════════════════════════════════════════
if(ANDROID)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing -Wno-narrowing -Wno-invalid-source-encoding")
    endif()

    # Use OpenSL ES on Android, not OpenAL
    list(REMOVE_ITEM COMMON_SOURCES
        "${CMAKE_SOURCE_DIR}/src/platform/audio/SoundSystemAL.cpp"
    )

    # Android entry point (JNI bridge)
    list(APPEND ANDROID_SOURCES ${COMMON_SOURCES})
    list(APPEND ANDROID_SOURCES
        "src/main_android_java.cpp"
        "src/platform/audio/SoundSystemSL.cpp"
    )

    # Extra screens that were appended manually in original
    list(APPEND ANDROID_SOURCES
        "src/client/gui/screens/ConsoleScreen.cpp"
        "src/client/gui/screens/UsernameScreen.cpp"
        "src/client/gui/screens/CreditsScreen.cpp"
    )

    add_library(minecraftpe SHARED ${ANDROID_SOURCES})

    # Android system libraries (found via NDK)
    find_library(log-lib     log)
    find_library(android-lib android)
    find_library(gles1-lib   GLESv1_CM)
    find_library(egl-lib     EGL)
    find_library(opensl-lib  OpenSLES)
    find_library(z-lib       z)

    target_link_libraries(minecraftpe
        ${log-lib}
        ${android-lib}
        ${gles1-lib}
        ${egl-lib}
        ${opensl-lib}
        ${z-lib}
    )

    target_include_directories(minecraftpe PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
    )

    target_compile_definitions(minecraftpe PRIVATE
        "OPENGL_ES"
        "PLATFORM_ANDROID"
        "__ANDROID__"
    )

# ═══════════════════════════════════════════════
#  DESKTOP BUILD  (Windows / Linux / macOS)
# ═══════════════════════════════════════════════
else()

    include(cmake/CPM.cmake)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-Wno-c++11-narrowing -Wno-narrowing -Wno-invalid-source-encoding -Wno-reserved-user-defined-literal")
    endif()

    CPMAddPackage("gh:madler/zlib@1.3.2")
    CPMAddPackage(
        NAME "libpng"
        GIT_REPOSITORY "https://github.com/pnggroup/libpng.git"
        GIT_TAG "v1.6.55"
        OPTIONS
            "ZLIB_ROOT ${zlib_SOURCE_DIR}"
            "ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR}"
            "PNG_TOOLS OFF"
            "PNG_TESTS OFF"
            "BUILD_SHARED_LIBS ON"
    )

    CPMAddPackage(
        NAME "openal"
        GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
        GIT_TAG "1.25.1"
        OPTIONS
            "ALSOFT_EXAMPLES OFF"
            "ALSOFT_TESTS OFF"
            "ALSOFT_UTILS OFF"
            "BUILD_SHARED_LIBS ON"
    )

    CPMAddPackage(
        NAME "glfw"
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "3.4"
        OPTIONS
            "GLFW_BUILD_EXAMPLES OFF"
            "GLFW_BUILD_TESTS OFF"
            "GLFW_BUILD_DOCS OFF"
            "BUILD_SHARED_LIBS ON"
    )

    if(NOT DEFINED PLATFORM)
        set(PLATFORM "PLATFORM_GLFW")
    endif()

    set(DESKTOP_SOURCES ${COMMON_SOURCES})
    list(APPEND DESKTOP_SOURCES "src/main.cpp")

    if(PLATFORM STREQUAL "PLATFORM_WIN32")
        list(APPEND DESKTOP_SOURCES "src/AppPlatform_win32.cpp")
    endif()

    if(PLATFORM STREQUAL "PLATFORM_GLFW")
        list(APPEND DESKTOP_SOURCES "src/AppPlatform_glfw.cpp")
    endif()

    list(APPEND DESKTOP_SOURCES
        "src/client/gui/screens/ConsoleScreen.cpp"
        "src/client/gui/screens/UsernameScreen.cpp"
        "src/client/gui/screens/CreditsScreen.cpp"
    )

    add_executable(${PROJECT_NAME}
        ${DESKTOP_SOURCES}
        "glad/src/glad.c"
    )

    if(WIN32)
        set(EXTRA_LIBS "ws2_32")
        target_compile_definitions(${PROJECT_NAME} PUBLIC "_CRT_SECURE_NO_WARNINGS")
    endif()

    if(PLATFORM STREQUAL "PLATFORM_WIN32" OR PLATFORM STREQUAL "PLATFORM_GLFW")
        target_compile_definitions(${PROJECT_NAME} PUBLIC "PLATFORM_DESKTOP")
    endif()

    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_SOURCE_DIR}/glad/include/"
        "${CMAKE_SOURCE_DIR}/src"
        "${openal_SOURCE_DIR}/include"
        "${glfw_SOURCE_DIR}/include"
        "lib/include"
    )

    target_compile_definitions(${PROJECT_NAME} PUBLIC "OPENGL_ES" "NO_EGL" ${PLATFORM})
    target_link_libraries(${PROJECT_NAME} zlib png_shared alsoft.common OpenAL::OpenAL glfw ${EXTRA_LIBS})

    if(NOT UNIX)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
                    $<TARGET_FILE_DIR:${PROJECT_NAME}>
                COMMAND_EXPAND_LISTS
        )
    endif()

    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/data"
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
    )

endif()
