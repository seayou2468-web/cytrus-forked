# Cytrus libretro Core

A libretro core for Nintendo 3DS emulation based on Citra.

## Features

- Full Nintendo 3DS emulation
- Support for .3ds, .3dsx, .cia, and .elf files
- Hardware-accelerated rendering
- Audio output
- Input support for RetroPad
- Save states
- Core options for configuration

## Requirements

- System files and firmware for Nintendo 3DS
- Modern GPU with OpenGL/Vulkan support
- 4GB+ RAM recommended

## Core Options

- **CPU JIT**: Enable/disable Just-In-Time compilation for better performance
- **New 3DS Mode**: Enable New 3DS hardware features
- **Hardware Shaders**: Enable hardware-accelerated shaders
- **Resolution Scale**: Internal resolution scaling (1x-8x)
- **Screen Layout**: Arrangement of top and bottom screens

## Building

```bash
make
```

## Installation

Copy the built core file to your RetroArch cores directory:
- Windows: `cores/cytrus_libretro.dll`
- Linux: `cores/cytrus_libretro.so`
- macOS: `cores/cytrus_libretro.dylib`

## Usage

1. Install Nintendo 3DS system files
2. Load 3DS games through RetroArch
3. Configure core options as needed

## Input Mapping

| RetroPad Button | 3DS Button |
|----------------|------------|
| B | A |
| A | B |
| Y | X |
| X | Y |
| Select | Select |
| Start | Start |
| L | ZL |
| R | ZR |
| L2 | L |
| R2 | R |
| D-Pad | D-Pad |
| Left Analog | Circle Pad |
| Right Analog | C-Stick |

## Known Issues

- Some games may have compatibility issues
- Performance varies depending on hardware
- Certain system features may not be fully implemented

## License

GPLv2 - See LICENSE file for details
