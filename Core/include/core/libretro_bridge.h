// Copyright 2024 Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <memory>
#include <string_view>
#include <vector>
#include "audio_core/sink.h"
#include "video_core/renderer_base.h"

namespace Core {
class System;
}

namespace Frontend {
class EmuWindow;
}

namespace Libretro {

class LibretroRenderer final : public VideoCore::RendererBase {
public:
    explicit LibretroRenderer(Core::System& system, Frontend::EmuWindow& window,
                             Frontend::EmuWindow* secondary_window);
    ~LibretroRenderer() override;

    VideoCore::RasterizerInterface* Rasterizer() override;
    void SwapBuffers() override;
    void TryPresent(int timeout_ms, bool is_secondary) override;
    void NotifySurfaceChanged(bool is_secondary) override;
};

class LibretroAudioSink final : public AudioCore::Sink {
public:
    explicit LibretroAudioSink(std::string_view device_id);
    ~LibretroAudioSink() override;

    unsigned int GetCallbackSize() const override;
    void Callback(std::span<s16> samples) override;
    std::string GetId() const override { return "libretro"; }
};

std::unique_ptr<AudioCore::Sink> CreateLibretroAudioSink(std::string_view device_id);
std::vector<std::string> ListLibretroAudioSinkDevices();

} // namespace Libretro
