#include <libretro.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "video_core/gpu.h"
#include "video_core/renderer_base.h"
#include "video_core/rasterizer_interface.h"

extern retro_video_refresh_t video_cb;

// Video output buffer
static uint32_t* video_buffer = nullptr;
static unsigned video_width = 800;
static unsigned video_height = 480;
static unsigned video_pitch = 0;

// Framebuffer layout for 3DS dual screen
enum class ScreenLayout {
    TopBottom,
    SideBySide,
    TopOnly,
    BottomOnly
};

static ScreenLayout current_layout = ScreenLayout::TopBottom;

// 3DS screen dimensions
static const unsigned TOP_SCREEN_WIDTH = 400;
static const unsigned TOP_SCREEN_HEIGHT = 240;
static const unsigned BOTTOM_SCREEN_WIDTH = 320;
static const unsigned BOTTOM_SCREEN_HEIGHT = 240;

// Resolution scaling
static unsigned resolution_scale = 1;

// Initialize video subsystem
bool cytrus_video_init(unsigned /*width*/, unsigned /*height*/, unsigned scale) {
    resolution_scale = scale;
    
    // Calculate actual dimensions based on layout and scale
    switch (current_layout) {
        case ScreenLayout::TopBottom:
            video_width = TOP_SCREEN_WIDTH * scale;
            video_height = (TOP_SCREEN_HEIGHT + BOTTOM_SCREEN_HEIGHT) * scale;
            break;
        case ScreenLayout::SideBySide:
            video_width = (TOP_SCREEN_WIDTH + BOTTOM_SCREEN_WIDTH) * scale;
            video_height = TOP_SCREEN_HEIGHT * scale;
            break;
        case ScreenLayout::TopOnly:
            video_width = TOP_SCREEN_WIDTH * scale;
            video_height = TOP_SCREEN_HEIGHT * scale;
            break;
        case ScreenLayout::BottomOnly:
            video_width = BOTTOM_SCREEN_WIDTH * scale;
            video_height = BOTTOM_SCREEN_HEIGHT * scale;
            break;
    }
    
    video_pitch = video_width * sizeof(uint32_t);
    
    // Allocate video buffer
    if (video_buffer) {
        free(video_buffer);
    }
    
    video_buffer = (uint32_t*)malloc(video_width * video_height * sizeof(uint32_t));
    if (!video_buffer) {
        return false;
    }
    
    // Clear buffer to black
    memset(video_buffer, 0, video_width * video_height * sizeof(uint32_t));
    
    return true;
}

void cytrus_video_deinit(void) {
    if (video_buffer) {
        free(video_buffer);
        video_buffer = nullptr;
    }
}

void cytrus_video_set_layout(ScreenLayout layout) {
    current_layout = layout;
    // Reinitialize with new layout
    cytrus_video_init(video_width, video_height, resolution_scale);
}

// Convert 3DS framebuffer format to RGBA8888
static void convert_framebuffer_to_rgba(const uint8_t* src, uint32_t* dst, 
                                       unsigned src_width, unsigned src_height,
                                       unsigned dst_x, unsigned dst_y, unsigned dst_pitch) {
    for (unsigned y = 0; y < src_height; y++) {
        uint32_t* dst_line = (uint32_t*)((uint8_t*)dst + (dst_y + y) * dst_pitch) + dst_x;
        
        for (unsigned x = 0; x < src_width; x++) {
            // 3DS uses RGB888 format
            uint8_t r = src[(y * src_width + x) * 3 + 0];
            uint8_t g = src[(y * src_width + x) * 3 + 1];
            uint8_t b = src[(y * src_width + x) * 3 + 2];
            
            // Convert to RGBA8888
            dst_line[x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
        }
    }
}

// Render frame from Citra's GPU output
void cytrus_video_render_frame(void) {
    if (!video_buffer || !video_cb)
        return;
    
    // In a functional core, these would be pointers to the actual 3DS VRAM or linear heap
    // For now, we keep them as placeholders but remove the test pattern generation
    // to prepare for real data bridging.
    static uint8_t top_screen_data[TOP_SCREEN_WIDTH * TOP_SCREEN_HEIGHT * 3] = {0};
    static uint8_t bottom_screen_data[BOTTOM_SCREEN_WIDTH * BOTTOM_SCREEN_HEIGHT * 3] = {0};
    
    // Clear the entire buffer first
    memset(video_buffer, 0, video_width * video_height * sizeof(uint32_t));
    
    // Render based on current layout
    switch (current_layout) {
        case ScreenLayout::TopBottom:
            {
                // Top screen
                convert_framebuffer_to_rgba(top_screen_data, video_buffer,
                                          TOP_SCREEN_WIDTH, TOP_SCREEN_HEIGHT,
                                          0, 0, video_pitch);
                
                // Bottom screen (centered)
                unsigned bottom_x = (video_width - BOTTOM_SCREEN_WIDTH * resolution_scale) / 2;
                unsigned bottom_y = TOP_SCREEN_HEIGHT * resolution_scale;
                convert_framebuffer_to_rgba(bottom_screen_data, video_buffer,
                                          BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT,
                                          bottom_x, bottom_y, video_pitch);
            }
            break;
            
        case ScreenLayout::SideBySide:
            {
                // Top screen
                convert_framebuffer_to_rgba(top_screen_data, video_buffer,
                                          TOP_SCREEN_WIDTH, TOP_SCREEN_HEIGHT,
                                          0, 0, video_pitch);
                
                // Bottom screen
                convert_framebuffer_to_rgba(bottom_screen_data, video_buffer,
                                          BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT,
                                          TOP_SCREEN_WIDTH * resolution_scale, 0, video_pitch);
            }
            break;
            
        case ScreenLayout::TopOnly:
            {
                // Only top screen
                convert_framebuffer_to_rgba(top_screen_data, video_buffer,
                                          TOP_SCREEN_WIDTH, TOP_SCREEN_HEIGHT,
                                          0, 0, video_pitch);
            }
            break;
            
        case ScreenLayout::BottomOnly:
            {
                // Only bottom screen
                convert_framebuffer_to_rgba(bottom_screen_data, video_buffer,
                                          BOTTOM_SCREEN_WIDTH, BOTTOM_SCREEN_HEIGHT,
                                          0, 0, video_pitch);
            }
            break;
    }
    
    // Apply scaling if needed (simple nearest neighbor scaling)
    if (resolution_scale > 1) {
        // This is a placeholder - proper scaling would need more sophisticated implementation
        // For now, we'll just send the unscaled buffer
    }
    
    // Send frame to frontend
    video_cb(video_buffer, video_width, video_height, video_pitch);
}

// Get video dimensions
void cytrus_video_get_dimensions(unsigned* width, unsigned* height) {
    if (width) *width = video_width;
    if (height) *height = video_height;
}

// Set resolution scale
void cytrus_video_set_resolution_scale(unsigned scale) {
    if (scale >= 1 && scale <= 8) {
        resolution_scale = scale;
        cytrus_video_init(video_width, video_height, scale);
    }
}

// Get current layout
ScreenLayout cytrus_video_get_layout(void) {
    return current_layout;
}
