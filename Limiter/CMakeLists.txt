cmake_minimum_required(VERSION 3.20)
project(Limiter VERSION 0.1.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Use -std=c++23 not -std=gnu++23

# Export compile commands for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Library Target
# ============================================================================
add_library(Limiter_lib
    src/limiter.cpp
)

# Include directories
target_include_directories(Limiter_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Compiler warnings (portable across GCC/Clang/MSVC)
target_compile_options(Limiter_lib
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
            -Wall -Wextra -Wpedantic -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4 /WX>
)

# ============================================================================
# Executable Target (Demo App)
# ============================================================================
add_executable(Limiter
    apps/main.cpp
)

target_link_libraries(Limiter
    PRIVATE Limiter_lib
)

# ============================================================================
# Install Targets (for library reuse)
# ============================================================================
include(GNUInstallDirs)

install(TARGETS Limiter_lib
    EXPORT LimiterTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/limiter
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT LimiterTargets
    FILE LimiterTargets.cmake
    NAMESPACE Limiter::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Limiter
)

# ============================================================================
# Testing (uncomment when ready to add tests)
# ============================================================================
# enable_testing()
# add_subdirectory(tests)