#include <libretro.h>
#include <stdlib.h>
#include <string.h>

#include "input_common/main.h"
#include "core/hle/service/hid/hid.h"

extern retro_input_poll_t input_poll_cb;
extern retro_input_state_t input_state_cb;

// Input state for all players
static uint16_t button_state[4] = {0};
static float analog_state[4][2][2] = {{{0}}}; // [player][stick][axis]

// Button mappings from RetroPad to 3DS
static const struct {
    unsigned retro_id;
    unsigned cytrus_button;
} button_mappings[] = {
    { RETRO_DEVICE_ID_JOYPAD_B,      700 }, // 3DS A button
    { RETRO_DEVICE_ID_JOYPAD_A,      701 }, // 3DS B button  
    { RETRO_DEVICE_ID_JOYPAD_Y,      702 }, // 3DS X button
    { RETRO_DEVICE_ID_JOYPAD_X,      703 }, // 3DS Y button
    { RETRO_DEVICE_ID_JOYPAD_SELECT, 705 }, // 3DS Select button
    { RETRO_DEVICE_ID_JOYPAD_START,  704 }, // 3DS Start button
    { RETRO_DEVICE_ID_JOYPAD_L,      707 }, // 3DS ZL button
    { RETRO_DEVICE_ID_JOYPAD_R,      708 }, // 3DS ZR button
    { RETRO_DEVICE_ID_JOYPAD_L2,     773 }, // 3DS L button
    { RETRO_DEVICE_ID_JOYPAD_R2,     774 }, // 3DS R button
    { RETRO_DEVICE_ID_JOYPAD_UP,     709 }, // 3DS D-Pad Up
    { RETRO_DEVICE_ID_JOYPAD_DOWN,   710 }, // 3DS D-Pad Down
    { RETRO_DEVICE_ID_JOYPAD_LEFT,   711 }, // 3DS D-Pad Left
    { RETRO_DEVICE_ID_JOYPAD_RIGHT,  712 }, // 3DS D-Pad Right
    { RETRO_DEVICE_ID_JOYPAD_L3,     781 }, // 3DS Debug button
};

// Analog stick mappings
static const struct {
    unsigned retro_index;
    unsigned cytrus_stick;
} analog_mappings[] = {
    { RETRO_DEVICE_INDEX_ANALOG_LEFT,  713 }, // Circle Pad
    { RETRO_DEVICE_INDEX_ANALOG_RIGHT, 718 }, // C-Stick
};

// Touch screen state
static struct {
    bool active;
    float x;
    float y;
} touch_state = {false, 0.0f, 0.0f};

// Initialize input subsystem
bool cytrus_input_init(void) {
    // Clear input state
    memset(button_state, 0, sizeof(button_state));
    memset(analog_state, 0, sizeof(analog_state));
    touch_state.active = false;
    touch_state.x = 0.0f;
    touch_state.y = 0.0f;
    
    return true;
}

void cytrus_input_deinit(void) {
    // Nothing to clean up
}

// Poll input state
void cytrus_input_poll(void) {
    if (!input_poll_cb)
        return;
    
    input_poll_cb();
    
    // Update button states for all players
    for (int player = 0; player < 4; player++) {
        uint16_t new_state = 0;
        
        // Check each button
        for (size_t i = 0; i < sizeof(button_mappings) / sizeof(button_mappings[0]); i++) {
            if (input_state_cb(player, RETRO_DEVICE_JOYPAD, 0, button_mappings[i].retro_id)) {
                new_state |= (1 << (button_mappings[i].cytrus_button - 700));
            }
        }
        
        button_state[player] = new_state;
        
        // Update analog sticks
        for (size_t i = 0; i < sizeof(analog_mappings) / sizeof(analog_mappings[0]); i++) {
            int16_t analog_x = input_state_cb(player, RETRO_DEVICE_ANALOG, 
                                            analog_mappings[i].retro_index, 
                                            RETRO_DEVICE_ID_ANALOG_X);
            int16_t analog_y = input_state_cb(player, RETRO_DEVICE_ANALOG, 
                                            analog_mappings[i].retro_index, 
                                            RETRO_DEVICE_ID_ANALOG_Y);
            
            // Convert to float (-1.0 to 1.0)
            float x = analog_x / 32767.0f;
            float y = analog_y / 32767.0f;
            
            // Apply deadzone
            const float deadzone = 0.15f;
            float magnitude = sqrtf(x * x + y * y);
            if (magnitude < deadzone) {
                x = 0.0f;
                y = 0.0f;
            } else {
                // Rescale to avoid jump at deadzone edge
                float scale = (magnitude - deadzone) / (1.0f - deadzone);
                x = (x / magnitude) * scale;
                y = (y / magnitude) * scale;
            }
            
            analog_state[player][i][0] = x;
            analog_state[player][i][1] = y;
        }
        
        // Handle touch screen (only for player 0)
        if (player == 0) {
            // Check for touch input using mouse or pointer
            bool touch_pressed = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_PRESSED);
            
            if (touch_pressed) {
                int16_t pointer_x = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_X);
                int16_t pointer_y = input_state_cb(0, RETRO_DEVICE_POINTER, 0, RETRO_DEVICE_ID_POINTER_Y);
                
                // Convert pointer coordinates to touch screen coordinates
                // 3DS touch screen is 320x240
                touch_state.x = ((pointer_x + 32767) * 320) / 65534.0f;
                touch_state.y = ((pointer_y + 32767) * 240) / 65534.0f;
                touch_state.active = true;
            } else {
                touch_state.active = false;
            }
        }
    }
}

// Get button state for a player
uint16_t cytrus_input_get_buttons(int player) {
    if (player < 0 || player >= 4)
        return 0;
    
    return button_state[player];
}

// Get analog stick state for a player
void cytrus_input_get_analog(int player, int stick, float* x, float* y) {
    if (player < 0 || player >= 4 || stick < 0 || stick >= 2) {
        if (x) *x = 0.0f;
        if (y) *y = 0.0f;
        return;
    }
    
    if (x) *x = analog_state[player][stick][0];
    if (y) *y = analog_state[player][stick][1];
}

// Get touch screen state
void cytrus_input_get_touch(bool* active, float* x, float* y) {
    if (active) *active = touch_state.active;
    if (x) *x = touch_state.x;
    if (y) *y = touch_state.y;
}

// Check if a specific button is pressed
bool cytrus_input_button_pressed(int player, unsigned button_id) {
    if (player < 0 || player >= 4)
        return false;
    
    return (button_state[player] & (1 << (button_id - 700))) != 0;
}

// Convert input state to Citra's format
void cytrus_input_update_citra(void) {
    // This would connect to Citra's input system
    // For now, we'll just update our internal state
    
    // Update HID service with current input state
    // This would need to be connected to Citra's Service::HID implementation
    
    // Update circle pad
    float circle_x, circle_y;
    cytrus_input_get_analog(0, 0, &circle_x, &circle_y);
    
    // Update C-stick  
    float cstick_x, cstick_y;
    cytrus_input_get_analog(0, 1, &cstick_x, &cstick_y);
    
    // Update touch screen
    bool touch_active;
    float touch_x, touch_y;
    cytrus_input_get_touch(&touch_active, &touch_x, &touch_y);
    
    // These values would be passed to Citra's input system
}

// Set rumble effect (if supported)
void cytrus_input_set_rumble(int player, float strength, uint32_t duration_ms) {
    // 3DS doesn't have rumble, but we implement this for completeness
    // Could be used for future extensions or compatibility
}

// Get controller info for a player
void cytrus_input_get_controller_info(int player, char* name, size_t name_size) {
    if (player < 0 || player >= 4 || !name || name_size == 0)
        return;
    
    snprintf(name, name_size, "RetroPad Player %d", player + 1);
}

// Check if input system is initialized
bool cytrus_input_is_initialized(void) {
    return input_poll_cb != NULL && input_state_cb != NULL;
}
