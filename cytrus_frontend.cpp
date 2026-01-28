#include <libretro.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Core/video_core/gpu.h"
#include "Core/video_core/renderer_base.h"
#include "Core/audio_core/audio_interface.h"
#include "Core/core/hle/service/hid/hid.h"
#include "Core/core/hle/service/ir/ir_user.h"
#include "Core/common/settings.h"

// libretro callbacks
extern retro_video_refresh_t video_cb;
extern retro_audio_sample_batch_t audio_sample_batch_cb;
extern retro_input_state_t input_state_cb;

// Frontend interface for Cytrus
class CytrusLibretroFrontend {
public:
    static CytrusLibretroFrontend& GetInstance() {
        static CytrusLibretroFrontend instance;
        return instance;
    }

    // Video output
    void PresentFrame(const uint8_t* data, unsigned width, unsigned height, size_t pitch) {
        if (video_cb && data) {
            // Convert to RGBA8888 if needed
            static uint32_t* rgba_buffer = nullptr;
            static size_t rgba_buffer_size = 0;
            
            size_t needed_size = width * height * 4;
            if (rgba_buffer_size < needed_size) {
                if (rgba_buffer) free(rgba_buffer);
                rgba_buffer = (uint32_t*)malloc(needed_size);
                rgba_buffer_size = needed_size;
            }
            
            // Convert RGB888 to RGBA8888
            const uint8_t* src = data;
            uint32_t* dst = rgba_buffer;
            
            for (unsigned y = 0; y < height; y++) {
                for (unsigned x = 0; x < width; x++) {
                    uint8_t r = src[(y * pitch + x) * 3 + 0];
                    uint8_t g = src[(y * pitch + x) * 3 + 1];
                    uint8_t b = src[(y * pitch + x) * 3 + 2];
                    dst[y * width + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
                }
            }
            
            video_cb(rgba_buffer, width, height, width * 4);
        }
    }

    // Audio output
    void OutputAudioSamples(const int16_t* samples, size_t count) {
        if (audio_sample_batch_cb && samples) {
            audio_sample_batch_cb(samples, count / 2); // count is frames, not samples
        }
    }

    // Input polling
    void PollInput() {
        // Input is handled by libretro, this is just a placeholder
    }

    // Touch screen input
    struct TouchState {
        bool pressed;
        float x;
        float y;
    };

    TouchState GetTouchState() {
        TouchState state = {false, 0.0f, 0.0f};
        
        if (input_state_cb) {
            // Check pointer/mouse input for touch screen
            bool pressed = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
            
            if (pressed) {
                int16_t pointer_x = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
                int16_t pointer_y = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
                
                // Convert to 3DS touch screen coordinates (320x240)
                state.x = ((pointer_x + 32767) * 320.0f) / 65534.0f;
                state.y = ((pointer_y + 32767) * 240.0f) / 65534.0f;
                state.pressed = true;
            }
        }
        
        return state;
    }

    // Button input
    uint32_t GetButtonState() {
        uint32_t buttons = 0;
        
        if (!input_state_cb)
            return buttons;
        
        // Map RetroPad to 3DS buttons
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))
            buttons |= 0x0001; // A
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))
            buttons |= 0x0002; // B
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y))
            buttons |= 0x0004; // X
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X))
            buttons |= 0x0008; // Y
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT))
            buttons |= 0x0010; // Select
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START))
            buttons |= 0x0020; // Start
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L))
            buttons |= 0x0040; // L
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R))
            buttons |= 0x0080; // R
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
            buttons |= 0x0100; // Up
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
            buttons |= 0x0200; // Down
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
            buttons |= 0x0400; // Left
        if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
            buttons |= 0x0800; // Right
        
        return buttons;
    }

    // Analog stick input
    struct AnalogState {
        float x, y;
    };

    AnalogState GetCirclePad() {
        AnalogState state = {0.0f, 0.0f};
        
        if (input_state_cb) {
            int16_t analog_x = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);
            int16_t analog_y = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);
            
            state.x = analog_x / 32767.0f;
            state.y = analog_y / 32767.0f;
            
            // Apply deadzone
            const float deadzone = 0.15f;
            float magnitude = sqrtf(state.x * state.x + state.y * state.y);
            if (magnitude < deadzone) {
                state.x = 0.0f;
                state.y = 0.0f;
            } else {
                float scale = (magnitude - deadzone) / (1.0f - deadzone);
                state.x = (state.x / magnitude) * scale;
                state.y = (state.y / magnitude) * scale;
            }
        }
        
        return state;
    }

    AnalogState GetCStick() {
        AnalogState state = {0.0f, 0.0f};
        
        if (input_state_cb) {
            int16_t analog_x = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X);
            int16_t analog_y = input_state_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);
            
            state.x = analog_x / 32767.0f;
            state.y = analog_y / 32767.0f;
            
            // Apply deadzone
            const float deadzone = 0.15f;
            float magnitude = sqrtf(state.x * state.x + state.y * state.y);
            if (magnitude < deadzone) {
                state.x = 0.0f;
                state.y = 0.0f;
            } else {
                float scale = (magnitude - deadzone) / (1.0f - deadzone);
                state.x = (state.x / magnitude) * scale;
                state.y = (state.y / magnitude) * scale;
            }
        }
        
        return state;
    }
};

// Interface functions that Cytrus can call
extern "C" {
    void cytrus_present_frame(const uint8_t* data, unsigned width, unsigned height, size_t pitch) {
        CytrusLibretroFrontend::GetInstance().PresentFrame(data, width, height, pitch);
    }

    void cytrus_output_audio(const int16_t* samples, size_t count) {
        CytrusLibretroFrontend::GetInstance().OutputAudioSamples(samples, count);
    }

    void cytrus_poll_input() {
        CytrusLibretroFrontend::GetInstance().PollInput();
    }

    uint32_t cytrus_get_button_state() {
        return CytrusLibretroFrontend::GetInstance().GetButtonState();
    }

    void cytrus_get_touch_state(bool* pressed, float* x, float* y) {
        auto state = CytrusLibretroFrontend::GetInstance().GetTouchState();
        if (pressed) *pressed = state.pressed;
        if (x) *x = state.x;
        if (y) *y = state.y;
    }

    void cytrus_get_circle_pad(float* x, float* y) {
        auto state = CytrusLibretroFrontend::GetInstance().GetCirclePad();
        if (x) *x = state.x;
        if (y) *y = state.y;
    }

    void cytrus_get_c_stick(float* x, float* y) {
        auto state = CytrusLibretroFrontend::GetInstance().GetCStick();
        if (x) *x = state.x;
        if (y) *y = state.y;
    }
}
