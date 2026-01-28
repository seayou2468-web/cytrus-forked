# Cytrus libretro Core - Build Instructions

## Overview
This document explains how to build the Cytrus libretro core from the existing Cytrus source code.

## Prerequisites

### Required Dependencies
- **C++17 compatible compiler** (GCC 7+, Clang 6+, MSVC 2017+)
- **CMake** 3.12 or higher
- **Git** for source management

### Platform-Specific Dependencies

#### Windows
- Visual Studio 2017 or later
- Windows SDK 10.0
- Git for Windows

#### Linux
- GCC 7+ or Clang 6+
- Development packages:
  ```bash
  sudo apt-get install build-essential cmake git libgl1-mesa-dev
  ```

#### macOS
- Xcode 10.0 or later
- Command Line Tools
- Homebrew (recommended)

## Build Process

### Step 1: Prepare the Source
```bash
# Clone the repository (if not already done)
git clone <repository-url> Cytrus-forked
cd Cytrus-forked

# Initialize submodules
git submodule update --init --recursive
```

### Step 2: Build the libretro Core

#### Method 1: Using Make (Recommended)
```bash
# Build the core
make

# The output will be:
# - Windows: cytrus_libretro.dll
# - Linux: cytrus_libretro.so  
# - macOS: cytrus_libretro.dylib
```

#### Method 2: Using CMake
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# The core will be in build/lib/
```

### Step 3: Install the Core

#### RetroArch Installation
```bash
# Copy to RetroArch cores directory
# Linux
cp cytrus_libretro.so ~/.config/retroarch/cores/

# Windows
copy cytrus_libretro.dll %APPDATA%\retroarch\cores\

# macOS
cp cytrus_libretro.dylib ~/Library/Application\ Support/RetroArch/cores/
```

## Build Options

### Debug Build
```bash
make DEBUG=1
```

### Static Linking
```bash
make STATIC=1
```

### Custom Compiler
```bash
make CXX=clang++ CC=clang
```

## Troubleshooting

### Common Issues

#### 1. Missing Dependencies
**Error**: `fatal error: 'libretro.h' file not found`
**Solution**: Ensure libretro-common submodule is initialized:
```bash
git submodule update --init --recursive
```

#### 2. Compilation Errors
**Error**: Various C++ compilation errors
**Solution**: Check that you have a C++17 compatible compiler:
```bash
g++ --version  # Should be 7.0 or higher
```

#### 3. Linking Errors
**Error**: Undefined reference to functions
**Solution**: Ensure all required libraries are installed and all source files are included in Makefile.

#### 4. Runtime Issues
**Error**: Core loads but games don't start
**Solution**: 
- Check that 3DS system files are properly installed
- Verify game files are valid
- Check RetroArch log for error messages

## Advanced Configuration

### Custom Build Flags
You can modify the Makefile to add custom flags:

```makefile
# Add to Makefile
CUSTOMFLAGS = -DCUSTOM_FLAG -O3
COMMONFLAGS += $(CUSTOMFLAGS)
```

### Cross-Compilation
For cross-compiling, set the target architecture:

```bash
# Cross-compile for Windows from Linux
make CROSS_COMPILE=x86_64-w64-mingw32-

# Cross-compile for ARM64
make CROSS_COMPILE=aarch64-linux-gnu-
```

## Testing the Core

### Basic Test
1. Install the core in RetroArch
2. Load a 3DS game file (.3ds, .3dsx, .cia)
3. Check if the game starts

### Debug Testing
Enable debug logging:
```bash
# In RetroArch, set log level to Debug
# Check the log file for detailed information
```

## Performance Optimization

### Recommended Settings
- **CPU JIT**: Enabled (for better performance)
- **Hardware Shaders**: Enabled (if supported)
- **Resolution Scale**: 2x or 3x (balance quality and performance)

### System Requirements
- **Minimum**: 4GB RAM, OpenGL 3.3 support
- **Recommended**: 8GB RAM, OpenGL 4.5+ or Vulkan support

## Contributing

### Code Style
Follow the existing code style in the project:
- 4-space indentation
- CamelCase for functions/classes
- UPPER_CASE for constants

### Submitting Changes
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review existing GitHub issues
3. Create a new issue with detailed information
4. Include system information and error logs

## License

This project is licensed under GPLv2. See the LICENSE file for details.
