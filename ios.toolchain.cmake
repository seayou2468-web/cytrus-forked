# iOS CMake toolchain file for libretro core compilation

set(CMAKE_SYSTEM_NAME iOS)
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0")
set(CMAKE_IOS_ARCHITECTURES "arm64;x86_64")

# Set compilers
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Set compiler flags
set(CMAKE_C_FLAGS_INIT "-fPIC -O3")
set(CMAKE_CXX_FLAGS_INIT "-fPIC -O3 -std=c++17")

# Set shared library flags
set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-shared")
set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-shared")

# Set install prefix
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

# Skip trying to compile test programs
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

# iOS specific settings
set(CMAKE_SYSTEM_PROCESSOR "aarch64")

# Find programs
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Set iOS SDK path
set(CMAKE_OSX_SYSROOT iphoneos)

# Link flags
set(CMAKE_EXE_LINKER_FLAGS "-fPIE")
set(CMAKE_SHARED_LINKER_FLAGS "-shared -fPIC")
