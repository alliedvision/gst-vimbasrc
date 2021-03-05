cmake_minimum_required(VERSION 3.12)

project(
    gstvimbasrc
    VERSION 0.1.0
    LANGUAGES C
    DESCRIPTION "GStreamer source plugin for Vimba cameras"
    # TODO: Should this URL instead point to the Github Repo we plan to use for this project?
    HOMEPAGE_URL "https://alliedvision.com/"
)

# add local cmake modules to simplify detection of dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

add_library(${PROJECT_NAME} SHARED
    src/gstvimbasrc.c
    src/vimba_helpers.c
    src/pixelformats.c
)

# Defines used in gstplugin.c
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        HAVE_CONFIG_H
)

# fill configuration file with defined values
configure_file(
    ${PROJECT_SOURCE_DIR}/src/config.h.in config.h
)

find_package(GStreamer REQUIRED COMPONENTS base)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_BINARY_DIR}
        ${GSTREAMER_INCLUDE_DIR}
        ${GLIB2_INCLUDE_DIR}
        # TODO: If possible find a better way to include Vimba into CMake
        ${VIMBA_HOME}
)

# Make linking work on Windows and Linux
# TODO: find a better way to include Vimba into CMake
if(WIN32)
    set(VIMBAC_LIBRARY ${VIMBA_HOME}/VimbaC/Lib/Win64/VimbaC.lib)
elseif(UNIX)
    if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(VIMBAC_LIBRARY ${VIMBA_HOME}/VimbaC/DynamicLib/x86_64bit/libVimbaC.so)
    elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        set(VIMBAC_LIBRARY ${VIMBA_HOME}/VimbaC/DynamicLib/arm_64bit/libVimbaC.so)
    endif()
endif()

target_link_libraries (${PROJECT_NAME}
    ${GLIB2_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GSTREAMER_LIBRARY}
    ${GSTREAMER_BASE_LIBRARY}
    # TODO: If possible find a better way to include Vimba into CMake
    ${VIMBAC_LIBRARY}
)