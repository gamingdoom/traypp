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
    target_include_directories(tray PUBLIC 
        ${GTK3_INCLUDE_DIRS} 
        ${APPINDICATOR_INCLUDE_DIRS}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tray/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(tray PRIVATE -Wall -Wextra -Werror -pedantic -Wno-ignored-qualifiers -Wno-deprecated-declarations)
    endif()

elseif(WIN32)
    target_link_libraries(tray PUBLIC shell32 user32)
    target_include_directories(tray PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tray/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
endif()

# Install the library and its headers
install(TARGETS tray
        EXPORT tray_targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

# Install headers while preserving directory structure
install(DIRECTORY "tray/include/" 
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

# Export targets for find_package
install(EXPORT tray_targets
        FILE tray-targets.cmake
        NAMESPACE tray::
        DESTINATION share/tray)

# Generate the config file in the binary dir
if(UNIX)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tray-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(PkgConfig REQUIRED)\n"
"pkg_check_modules(GTK3 REQUIRED gtk+-3.0)\n"
"pkg_check_modules(APPINDICATOR REQUIRED ayatana-appindicator3-0.1)\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/tray-targets.cmake\")\n"
)
elseif(WIN32)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tray-config.cmake"
"include(CMakeFindDependencyMacro)\n"
"# Windows dependencies are system libraries\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/tray-targets.cmake\")\n"
)
endif()

# Install the generated config file
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tray-config.cmake"
        DESTINATION share/tray)

set_target_properties(tray PROPERTIES VERSION ${PROJECT_VERSION})