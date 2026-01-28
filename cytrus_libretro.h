#ifndef CYTRUS_LIBRETRO_H
#define CYTRUS_LIBRETRO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screen layout options
typedef enum {
    CYTRUS_LAYOUT_TOP_BOTTOM,
    CYTRUS_LAYOUT_SIDE_BY_SIDE,
    CYTRUS_LAYOUT_TOP_ONLY,
    CYTRUS_LAYOUT_BOTTOM_ONLY
} cytrus_layout_t;

// Video functions
bool cytrus_video_init(unsigned width, unsigned height, unsigned scale);
void cytrus_video_deinit(void);
void cytrus_video_set_layout(cytrus_layout_t layout);
void cytrus_video_render_frame(void);
void cytrus_video_get_dimensions(unsigned* width, unsigned* height);
void cytrus_video_set_resolution_scale(unsigned scale);
cytrus_layout_t cytrus_video_get_layout(void);

// Audio functions
bool cytrus_audio_init(void);
void cytrus_audio_deinit(void);
void cytrus_audio_set_volume(double volume);
void cytrus_audio_set_muted(bool muted);
void cytrus_audio_process_samples(const int16_t* samples, size_t sample_count);
void cytrus_audio_generate_silence(size_t frame_count);
void cytrus_audio_flush(void);
void cytrus_audio_process_float_samples(const float* samples, size_t sample_count);
void cytrus_audio_generate_test_tone(double frequency, size_t frame_count);
bool cytrus_audio_is_initialized(void);
double cytrus_audio_get_volume(void);
bool cytrus_audio_is_muted(void);

// Input functions
bool cytrus_input_init(void);
void cytrus_input_deinit(void);
void cytrus_input_poll(void);
uint16_t cytrus_input_get_buttons(int player);
void cytrus_input_get_analog(int player, int stick, float* x, float* y);
void cytrus_input_get_touch(bool* active, float* x, float* y);
bool cytrus_input_button_pressed(int player, unsigned button_id);
void cytrus_input_update_citra(void);
void cytrus_input_set_rumble(int player, float strength, uint32_t duration_ms);
void cytrus_input_get_controller_info(int player, char* name, size_t name_size);
bool cytrus_input_is_initialized(void);

// Memory functions
bool cytrus_memory_init(void);
void cytrus_memory_deinit(void);
void* cytrus_memory_get_data(unsigned id);
size_t cytrus_memory_get_size(unsigned id);
size_t cytrus_memory_serialize_size(void);
bool cytrus_memory_serialize(void* data, size_t size);
bool cytrus_memory_unserialize(const void* data, size_t size);
bool cytrus_memory_create_snapshot(void);
bool cytrus_memory_restore_snapshot(void);
void cytrus_memory_get_region_info(unsigned id, const char** name, size_t* size, void** data);
void cytrus_memory_update_regions(void);
void cytrus_memory_dump_region(unsigned id, const char* filename);
bool cytrus_memory_validate(void);

// Constants
#define CYTRUS_TOP_SCREEN_WIDTH    400
#define CYTRUS_TOP_SCREEN_HEIGHT   240
#define CYTRUS_BOTTOM_SCREEN_WIDTH  320
#define CYTRUS_BOTTOM_SCREEN_HEIGHT 240
#define CYTRUS_MAX_PLAYERS         4
#define CYTRUS_MAX_RESOLUTION_SCALE 8

#ifdef __cplusplus
}
#endif

#endif // CYTRUS_LIBRETRO_H
