# GitHub Actions iOS Build Guide

This document explains how to use the GitHub Actions workflows to build the Cytrus libretro core for iOS.

## Overview

The repository includes two GitHub Actions workflows for building iOS libretro cores:

1. **`build-ios.yml`** - CMake-based build method
2. **`build-ios-makefile.yml`** - Traditional Makefile build method (recommended)

## Prerequisites

### Repository Structure
```
Cytrus-forked/
├── .github/workflows/
│   ├── build-ios.yml
│   └── build-ios-makefile.yml
├── Core/                    # Cytrus core source code
├── cytrus_*.cpp            # libretro interface files
├── cytrus_*.h              # Header files
├── Makefile                # Build configuration
└── ios.toolchain.cmake     # CMake toolchain file
```

### Required Secrets (Optional)
**Note: Code signing has been disabled. The build now creates unsigned dylib files.**

For future code signing needs, you would need:
1. **`CODESIGN_CERTIFICATE`** - Base64 encoded .p12 certificate
2. **`CODESIGN_PASSWORD`** - Certificate password

To create these secrets:

```bash
# Convert certificate to base64
base64 -i certificate.p12 | pbcopy

# Add to GitHub repository secrets:
# Settings → Secrets and variables → Actions → New repository secret
```

## Workflow Triggers

### Automatic Triggers
- **Push** to `main` or `develop` branches
- **Pull requests** to `main` branch
- Changes to relevant files (Core/, cytrus_*, Makefile, workflows)

### Manual Triggers
- **Workflow dispatch** - Can be triggered manually from GitHub Actions tab

## Build Methods

### Method 1: Makefile Build (Recommended)

This method uses the traditional libretro Makefile approach and is more reliable.

**Features:**
- Multi-architecture builds (arm64, x86_64)
- Universal binary creation
- iOS 13.0+ and 14.0+ support
- Better compatibility with libretro standards

**Workflow:** `.github/workflows/build-ios-makefile.yml`

### Method 2: CMake Build

This method uses CMake with iOS toolchain files.

**Features:**
- Modern CMake configuration
- Cross-platform compatibility
- Easier dependency management

**Workflow:** `.github/workflows/build-ios.yml`

## Build Process

### Step-by-Step Overview

1. **Environment Setup**
   - Uses `macos-latest` runner
   - Installs Xcode 26.1
   - Configures build environment

2. **Source Checkout**
   - Recursive checkout with submodules
   - Full history for proper versioning

3. **Build Configuration**
   - Sets iOS deployment target (13.0+)
   - Configures architecture-specific flags
   - Sets up compiler flags for iOS

4. **Compilation**
   - Builds for arm64 (primary)
   - Optionally builds for x86_64
   - Creates universal binaries

5. **Artifact Creation**
   - Creates core info file
   - Packages build artifacts
   - Uploads to GitHub Actions

## Build Matrix

### Architecture Support
| Architecture | iOS Minimum | Notes |
|-------------|-------------|-------|
| arm64 | 13.0, 14.0 | Primary iOS devices |
| x86_64 | 13.0 | iOS Simulator (deprecated in newer iOS) |

### Build Combinations
- `arm64 + iOS 13.0` ✅ (Recommended)
- `arm64 + iOS 14.0` ✅ (Modern devices only)
- `x86_64 + iOS 13.0` ✅ (Simulator support)

## Artifacts

### Generated Files
- `cytrus_libretro.dylib` - Main core library
- `cytrus_libretro.info` - Core information file
- Build logs and diagnostics

### Artifact Names
- `cytrus-ios-arm64-13.0` - ARM64 build for iOS 13.0+
- `cytrus-ios-arm64-14.0` - ARM64 build for iOS 14.0+
- `cytrus-ios-universal` - Universal binary (if available)

## Local Development

### Prerequisites
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install dependencies
brew install cmake
```

### Local Build Commands

#### Makefile Method
```bash
# Set iOS build environment
export CFLAGS="-arch arm64 -mios-version-min=13.0 -O3 -fPIC"
export CXXFLAGS="-arch arm64 -mios-version-min=13.0 -O3 -fPIC -std=c++17"
export LDFLAGS="-arch arm64 -mios-version-min=13.0 -shared"
export CC=clang
export CXX=clang++

# Build
make clean
make -j$(sysctl -n hw.ncpu)
```

#### CMake Method
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake .. \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
  -DCMAKE_IOS_ARCHITECTURES=arm64 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake

# Build
make -j$(sysctl -n hw.ncpu)
```

## Troubleshooting

### Common Issues

#### 1. Build Failures
**Symptoms:** Compilation errors, missing headers
**Solutions:**
- Check Xcode installation
- Verify iOS SDK availability
- Update submodules: `git submodule update --init --recursive`

#### 2. Code Signing Issues
**Symptoms:** Invalid signature, certificate errors
**Solutions:**
- Verify certificate validity
- Check certificate password
- Ensure proper certificate export format (.p12)

#### 3. Architecture Issues
**Symptoms:** Wrong architecture, compatibility warnings
**Solutions:**
- Verify target iOS version
- Check architecture flags
- Use proper iOS SDK

### Debug Information

Enable verbose logging:
```bash
# Makefile
make VERBOSE=1

# CMake
make VERBOSE=1
```

Check library information:
```bash
# File type and architecture
file cytrus_libretro.dylib

# Dependencies
otool -L cytrus_libretro.dylib

# Architecture info (for universal binaries)
lipo -info cytrus_libretro.dylib
```

## Integration with RetroArch

### Installing the Core

1. **Download artifacts** from GitHub Actions
2. **Code sign** the dylib (if not already signed)
3. **Copy to RetroArch modules directory:**
   ```bash
   # For iOS
   cp cytrus_libretro.dylib /path/to/RetroArch/pkg/apple/iOS/modules/
   
   # For tvOS
   cp cytrus_libretro.dylib /path/to/RetroArch/pkg/apple/tvOS/modules/
   ```

4. **Build RetroArch** with the new core:
   ```bash
   cd /path/to/RetroArch
   xcodebuild -target RetroArchiOS11 -configuration Release
   ```

### Testing

1. **Install RetroArch** on device
2. **Verify core appears** in core list
3. **Test with 3DS game files**
4. **Check performance** and compatibility

## Advanced Configuration

### Custom Build Flags
Modify the workflow to add custom flags:

```yaml
- name: Build with custom flags
  run: |
    export CFLAGS="$CFLAGS -custom-flag"
    export CXXFLAGS="$CXXFLAGS -custom-flag"
    make -j$(sysctl -n hw.ncpu)
```

### Multiple iOS Versions
Extend the build matrix for additional iOS versions:

```yaml
strategy:
  matrix:
    ios_min:
      - "12.0"
      - "13.0" 
      - "14.0"
      - "15.0"
    include:
      - ios_min: "12.0"
        exclude_arch: "x86_64"
```

### Automated Releases
Create releases automatically on tags:

```yaml
- name: Create Release
  if: startsWith(github.ref, 'refs/tags/')
  uses: softprops/action-gh-release@v1
  with:
    files: dist/ios-arm64/*.dylib
    draft: false
    prerelease: ${{ contains(github.ref, 'beta') }}
```

## Performance Optimization

### Build Speed
- Use build caching
- Parallel compilation (`-j$(sysctl -n hw.ncpu)`)
- Incremental builds

### Binary Size
- Strip debug symbols: `strip -x cytrus_libretro.dylib`
- Optimize compiler flags: `-Os` instead of `-O3`
- Remove unused code

## Security Considerations

### Certificate Management
- Store certificates in GitHub secrets
- Use short-lived certificates
- Rotate certificates regularly

### Code Signing
- Verify signatures before distribution
- Use proper entitlements
- Test on target iOS versions

## Support

For issues with the GitHub Actions workflows:

1. **Check the Actions tab** in GitHub repository
2. **Review build logs** for error details
3. **Consult this documentation**
4. **Create an issue** with detailed information

Include in issue reports:
- Workflow name and run ID
- Error messages and logs
- Environment details (iOS version, Xcode version)
- Steps to reproduce
