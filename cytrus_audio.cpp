#include <libretro.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "audio_core/dsp_interface.h"
#include "audio_core/sink.h"

extern retro_audio_sample_t audio_sample_cb;
extern retro_audio_sample_batch_t audio_sample_batch_cb;

// Audio settings
static const double SAMPLE_RATE = 44100.0;
static const size_t BUFFER_SIZE = 1024;
static const int CHANNELS = 2; // Stereo

// Audio buffer
static int16_t audio_buffer[BUFFER_SIZE * CHANNELS];
static size_t buffer_pos = 0;
static bool audio_initialized = false;

// Audio processing parameters
static double volume = 1.0;
static bool audio_muted = false;

// Initialize audio subsystem
bool cytrus_audio_init(void) {
    if (audio_initialized)
        return true;
    
    // Clear audio buffer
    memset(audio_buffer, 0, sizeof(audio_buffer));
    buffer_pos = 0;
    
    audio_initialized = true;
    return true;
}

void cytrus_audio_deinit(void) {
    audio_initialized = false;
    buffer_pos = 0;
}

// Set audio volume (0.0 to 1.0)
void cytrus_audio_set_volume(double vol) {
    volume = fmax(0.0, fmin(1.0, vol));
}

// Mute/unmute audio
void cytrus_audio_set_muted(bool muted) {
    audio_muted = muted;
}

// Process audio samples from Citra
void cytrus_audio_process_samples(const int16_t* samples, size_t sample_count) {
    if (!audio_initialized || audio_muted || !audio_sample_batch_cb)
        return;
    
    // Process samples in chunks
    while (sample_count > 0) {
        size_t space_left = BUFFER_SIZE - buffer_pos;
        size_t to_process = (sample_count < space_left) ? sample_count : space_left;
        
        // Copy and apply volume
        for (size_t i = 0; i < to_process; i++) {
            int16_t sample = samples[i];
            
            // Apply volume
            if (volume != 1.0) {
                sample = (int16_t)(sample * volume);
            }
            
            audio_buffer[buffer_pos++] = sample;
        }
        
        samples += to_process;
        sample_count -= to_process;
        
        // If buffer is full, send to frontend
        if (buffer_pos >= BUFFER_SIZE) {
            audio_sample_batch_cb(audio_buffer, BUFFER_SIZE / CHANNELS);
            buffer_pos = 0;
        }
    }
}

// Generate silence
void cytrus_audio_generate_silence(size_t frame_count) {
    if (!audio_initialized || !audio_sample_batch_cb)
        return;
    
    size_t samples_needed = frame_count * CHANNELS;
    
    while (samples_needed > 0) {
        size_t space_left = BUFFER_SIZE - buffer_pos;
        size_t to_generate = (samples_needed < space_left) ? samples_needed : space_left;
        
        // Fill with silence
        memset(&audio_buffer[buffer_pos], 0, to_generate * sizeof(int16_t));
        buffer_pos += to_generate;
        samples_needed -= to_generate;
        
        // If buffer is full, send to frontend
        if (buffer_pos >= BUFFER_SIZE) {
            audio_sample_batch_cb(audio_buffer, BUFFER_SIZE / CHANNELS);
            buffer_pos = 0;
        }
    }
}

// Flush remaining audio samples
void cytrus_audio_flush(void) {
    if (!audio_initialized || buffer_pos == 0 || !audio_sample_batch_cb)
        return;
    
    // Send remaining samples
    audio_sample_batch_cb(audio_buffer, buffer_pos / CHANNELS);
    buffer_pos = 0;
}

// Convert float samples to int16
static int16_t float_to_int16(float sample) {
    sample = fmax(-1.0f, fmin(1.0f, sample));
    return (int16_t)(sample * 32767.0f);
}

// Process audio samples from float format
void cytrus_audio_process_float_samples(const float* samples, size_t sample_count) {
    if (!audio_initialized || audio_muted || !audio_sample_batch_cb)
        return;
    
    // Convert float samples to int16 and process
    int16_t int_samples[BUFFER_SIZE * CHANNELS];
    size_t processed = 0;
    
    while (sample_count > 0) {
        size_t space_left = BUFFER_SIZE - buffer_pos;
        size_t to_process = (sample_count < space_left) ? sample_count : space_left;
        
        // Convert and apply volume
        for (size_t i = 0; i < to_process; i++) {
            int16_t sample = float_to_int16(samples[i]);
            
            // Apply volume
            if (volume != 1.0) {
                sample = (int16_t)(sample * volume);
            }
            
            audio_buffer[buffer_pos++] = sample;
        }
        
        samples += to_process;
        sample_count -= to_process;
        processed += to_process;
        
        // If buffer is full, send to frontend
        if (buffer_pos >= BUFFER_SIZE) {
            audio_sample_batch_cb(audio_buffer, BUFFER_SIZE / CHANNELS);
            buffer_pos = 0;
        }
    }
}

// Generate test tone (for debugging)
void cytrus_audio_generate_test_tone(double frequency, size_t frame_count) {
    if (!audio_initialized || !audio_sample_batch_cb)
        return;
    
    static double phase = 0.0;
    const double phase_increment = (2.0 * M_PI * frequency) / SAMPLE_RATE;
    
    for (size_t frame = 0; frame < frame_count; frame++) {
        // Generate sine wave
        float sample = sin(phase) * 0.3f; // 30% volume
        phase += phase_increment;
        
        if (phase >= 2.0 * M_PI) {
            phase -= 2.0 * M_PI;
        }
        
        // Convert to int16 and store for both channels
        int16_t int_sample = float_to_int16(sample);
        
        audio_buffer[buffer_pos++] = int_sample; // Left channel
        audio_buffer[buffer_pos++] = int_sample; // Right channel
        
        // Send buffer if full
        if (buffer_pos >= BUFFER_SIZE) {
            audio_sample_batch_cb(audio_buffer, BUFFER_SIZE / CHANNELS);
            buffer_pos = 0;
        }
    }
}

// Get audio status
bool cytrus_audio_is_initialized(void) {
    return audio_initialized;
}

double cytrus_audio_get_volume(void) {
    return volume;
}

bool cytrus_audio_is_muted(void) {
    return audio_muted;
}
