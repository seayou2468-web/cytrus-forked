# Cytrus libretro core Makefile

# Core name
CORE_NAME = cytrus

# Platform detection
UNAME = $(shell uname -s)
ifeq ($(OS),Windows_NT)
    PLATFORM = win
else ifeq ($(UNAME),Darwin)
    PLATFORM = osx
else
    PLATFORM = unix
endif

# Compiler and flags
CXX = g++
CC = gcc

# Common flags
COMMONFLAGS = -O2 -g -Wall -Wextra -fno-strict-aliasing -fno-exceptions -fvisibility=hidden

# Platform-specific flags
ifeq ($(PLATFORM),win)
    COMMONFLAGS += -D__WIN32__ -static-libgcc -static-libstdc++
    LDFLAGS = -shared -Wl,--version-script=libretro/link.T -Wl,--no-undefined
    LIBS = -lws2_32 -lwinmm -lgdi32 -lopengl32
    SOEXT = .dll
else ifeq ($(PLATFORM),osx)
    COMMONFLAGS += -D__APPLE__ -stdlib=libc++
    LDFLAGS = -shared -dynamiclib
    LIBS = -framework OpenGL -framework Foundation -framework AppKit
    SOEXT = .dylib
else
    COMMONFLAGS += -D__linux__ -fPIC
    LDFLAGS = -shared -Wl,--version-script=libretro/link.T -Wl,--no-undefined
    LIBS = -lGL -lpthread -ldl
    SOEXT = .so
endif

# Include directories
INCLUDES = -I. \
           -Ilibretro-common/include \
           -ICore \
           -ICore/audio_core \
           -ICore/common \
           -ICore/core \
           -ICore/core/arm \
           -ICore/core/hle \
           -ICore/core/hle/service \
           -ICore/frontend \
           -ICore/input_common \
           -ICore/network \
           -ICore/video_core

# Source files
CORE_SOURCES = cytrus_libretro_core.cpp cytrus_frontend.cpp

# Cytrus core sources
CITRA_SOURCES = \
    Core/core/libretro_interface.cpp \
    cytrus_video.cpp \
    cytrus_audio.cpp \
    cytrus_input.cpp \
    cytrus_memory.cpp

# Additional Citra core files (simplified - would need full list)
CITRA_CORE_SOURCES = \
    Core/core/core.cpp \
    Core/core/loader/loader.cpp \
    Core/core/hle/service/cfg/cfg.cpp \
    Core/core/hle/service/am/am.cpp \
    Core/core/frontend/applets/default_applets.cpp \
    Core/input_common/main.cpp \
    Core/network/network.cpp \
    Core/audio_core/audio_interface.cpp \
    Core/video_core/gpu.cpp \
    Core/video_core/renderer_base.cpp \
    Core/common/file_util.cpp \
    Core/common/settings.cpp

# Find all source files automatically
SOURCES = $(CORE_SOURCES) $(CITRA_SOURCES) $(CITRA_CORE_SOURCES)
OBJECTS = $(SOURCES:.cpp=.o)

# Target
TARGET = $(CORE_NAME)_libretro$(SOEXT)

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(COMMONFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CC) $(COMMONFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

# Install (optional)
install: $(TARGET)
	cp $(TARGET) /usr/local/lib/libretro/

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the libretro core"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install the core to system directory"
	@echo "  help     - Show this help message"
