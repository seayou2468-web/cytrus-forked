// Copyright 2024 Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <libretro.h>
#include "core/core.h"
#include "core/libretro_bridge.h"
#include "video_core/gpu.h"
#include "video_core/rasterizer_interface.h"

extern "C" {
    void cytrus_present_frame(const uint8_t* data, unsigned width, unsigned height, size_t pitch);
    void cytrus_output_audio(const int16_t* samples, size_t count);
}

namespace Libretro {

class NullRasterizer : public VideoCore::RasterizerInterface {
public:
    void AddTriangle(const Pica::OutputVertex& v0, const Pica::OutputVertex& v1,
                     const Pica::OutputVertex& v2) override {}
    void DrawTriangles() override {}
    void FlushAll() override {}
    void FlushRegion(PAddr addr, u32 size) override {}
    void InvalidateRegion(PAddr addr, u32 size) override {}
    void FlushAndInvalidateRegion(PAddr addr, u32 size) override {}
    void ClearAll(bool flush) override {}
};

static NullRasterizer null_rasterizer;

LibretroRenderer::LibretroRenderer(Core::System& system, Frontend::EmuWindow& window,
                                   Frontend::EmuWindow* secondary_window)
    : RendererBase(system, window, secondary_window) {}

LibretroRenderer::~LibretroRenderer() = default;

VideoCore::RasterizerInterface* LibretroRenderer::Rasterizer() {
    return &null_rasterizer;
}

void LibretroRenderer::SwapBuffers() {
    // In a real Citra core, we would pull from the GPU/Rasterizer.
    // For this bridge, we assume the frontend provides a way to get the data.
    // This is currently a stub that allows the core to 'run'.
}

void LibretroRenderer::TryPresent(int timeout_ms, bool is_secondary) {}

void LibretroRenderer::NotifySurfaceChanged(bool is_secondary) {}

LibretroAudioSink::LibretroAudioSink(std::string_view device_id) {}
LibretroAudioSink::~LibretroAudioSink() = default;

unsigned int LibretroAudioSink::GetCallbackSize() const {
    return 1024;
}

void LibretroAudioSink::Callback(std::span<s16> samples) {
    cytrus_output_audio(samples.data(), samples.size());
}

std::unique_ptr<AudioCore::Sink> CreateLibretroAudioSink(std::string_view device_id) {
    return std::make_unique<LibretroAudioSink>(device_id);
}

std::vector<std::string> ListLibretroAudioSinkDevices() {
    return {"Libretro"};
}

} // namespace Libretro
