#include <ti/getcsc.h>  // Can be removed if all input switches to keypadc
#include <graphx.h>
#include <sys/timers.h>
#include <sys/rtc.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <keypadc.h>     // <--- ADD THIS INCLUDE

#include "game_constants.h"
#include "game_logic.h"
#include "game_drawing.h"

#define TARGET_FRAMERATE_MAIN 30.0f
#define FRAME_TIME_TARGET_MAIN (1.0f / TARGET_FRAMERATE_MAIN)
#define FRAME_LIMITING_ENABLED true

// Define which key will be used for jumping.
// For example, using the [enter] key.
// kb_Data[6] contains Enter, Clear, math ops. kb_Enter is the mask.
#define JUMP_KEY_GROUP 6
#define JUMP_KEY_MASK  kb_Enter
// For exiting, kb_Clear is also in group 6
#define CLEAR_KEY_GROUP 6
#define CLEAR_KEY_MASK  kb_Clear


int main(void) {
    srand(rtc_Time());
    game_draw_init();
    init_game_state();

    float last_loop_time_sec = (float)clock() / CLOCKS_PER_SEC;
    float accumulated_delta_time = 0.0f;
    bool game_is_running = true;
    // uint8_t key_code; // Replaced by specific key checks

    while (game_is_running) {
        float current_loop_time_sec = (float)clock() / CLOCKS_PER_SEC;
        float elapsed_iteration_time = current_loop_time_sec - last_loop_time_sec;
        last_loop_time_sec = current_loop_time_sec;

        if (elapsed_iteration_time > 0.25f) {
            elapsed_iteration_time = 0.25f;
        }
        if (elapsed_iteration_time < 0.0f) {
            elapsed_iteration_time = 0.0f;
        }
        
        accumulated_delta_time += elapsed_iteration_time;
        bool needs_render = false;

        // --- Key Input Handling using keypadc ---
        kb_Scan(); // Update the keypad state

        if (kb_Data[CLEAR_KEY_GROUP] & CLEAR_KEY_MASK) { // Check if Clear key is pressed
            game_is_running = false;
        }
        // Check if the defined jump key is pressed
        bool jump_key_active = (kb_Data[JUMP_KEY_GROUP] & JUMP_KEY_MASK);
        // --- End Key Input Handling ---

        if (FRAME_LIMITING_ENABLED) {
            while (accumulated_delta_time >= FRAME_TIME_TARGET_MAIN && game_is_running) {
                // Pass the state of the specific jump key
                update_game_logic(jump_key_active, FRAME_TIME_TARGET_MAIN);
                accumulated_delta_time -= FRAME_TIME_TARGET_MAIN;
                needs_render = true;
                 if (kb_Data[CLEAR_KEY_GROUP] & CLEAR_KEY_MASK) { // Re-check clear key inside the loop if it can be held
                    game_is_running = false;
                }
            }
        } else {
            if (game_is_running) {
                update_game_logic(jump_key_active, accumulated_delta_time);
            }
            accumulated_delta_time = 0.0f;
            needs_render = true;
        }
        
        if (!game_is_running) {
            break;
        }

        if (needs_render) {
            draw_game_elements();
            gfx_SwapDraw();
        }
    }

    gfx_End();
    return 0;
}