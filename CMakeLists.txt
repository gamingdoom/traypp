cmake_minimum_required(VERSION 3.10)

project(tray VERSION 0.2.2 DESCRIPTION "A cross-platform C++ system tray library")

include(GNUInstallDirs)

file(GLOB_RECURSE src
    "tray/src/*.cpp"
)

add_library(tray ${src})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
target_compile_features(tray PUBLIC cxx_std_17)
set_target_properties(tray PROPERTIES 
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      CXX_STANDARD_REQUIRED ON)

# Platform-specific configuration
if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(APPINDICATOR REQUIRED ayatana-appindicator3-0.1)

    message(STATUS "GTK3_INCLUDE_DIRS: ${GTK3_INCLUDE_DIRS}")
    message(STATUS "APPINDICATOR_INCLUDE_DIRS: ${APPINDICATOR_INCLUDE_DIRS}")

    target_link_libraries(tray PUBLIC ${GTK3_LIBRARIES} ${APPINDICATOR_LIBRARIES})
    target_include_directories(tray PUBLIC ${GTK3_INCLUDE_DIRS} ${APPINDICATOR_INCLUDE_DIRS})

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(tray PRIVATE -Wall -Wextra -Werror -pedantic -Wno-ignored-qualifiers -Wno-deprecated-declarations)
    endif()

elseif(WIN32)
    target_link_libraries(tray PUBLIC shell32 user32)
endif()

# Add include directories for build and install
# (headers are in tray/include)
target_include_directories(tray PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tray/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Install the library and its headers
install(TARGETS tray
        EXPORT tray_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

file(GLOB_RECURSE TRAY_HEADERS "tray/include/*.hpp")
install(FILES ${TRAY_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export targets for find_package
install(EXPORT tray_targets
        FILE tray-targets.cmake
        NAMESPACE tray::
        DESTINATION share/tray)

# Generate the config file in the binary dir
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tray-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"# Add find_dependency() for any required dependencies here\n"
"include(\"${CMAKE_CURRENT_LIST_DIR}/tray-targets.cmake\")\n"
)

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tray-config.cmake"
        DESTINATION share/tray)

set_target_properties(tray PROPERTIES VERSION ${PROJECT_VERSION})