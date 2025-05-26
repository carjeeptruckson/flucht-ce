#include "sprites/gfx.h" // For sprite pointers and palette
#include "game_logic.h"
#include "game_drawing.h" // For drawing functions
#include "game_entities.h"// For Particle, BackgroundElement, etc.
#include <stdlib.h>      // For rand(), srand()
#include <math.h>        // For fmaxf, roundf, fabsf, sinf
#include <string.h>      // For memset
#include <fileioc.h>     // For AppVar R/W
#include <ti/vars.h>     // For OS_TYPE_APPVAR
#include <keypadc.h>     // For kb_Data, kb_Scan
#include <sys/timers.h>  // For delay
#include <sys/rtc.h>     // For rtc_Time()

// --- Global Variable DEFINITIONS ---
GameState current_state;
float time_since_last_state_change;
float f_camera_bottom_y;
float player_pos[2];
float player_vel[2];
uint8_t player_size[2] = {PLAYER_WIDTH, PLAYER_HEIGHT}; 
int player_rect[4]; 
float key_pressed_start_time;
float jump_start_time;
float grab_start_time;
uint8_t full_jump_chain;
bool is_boosting;
float last_boost_particle_gen_time;
float game_over_time;
float time_since_game_start;
uint24_t last_score;
float last_blink_start;
bool is_grounded;
float time_since_grounded;
int last_generation_height;

BackgroundElement background_elements[MAX_BACKGROUND_ELEMENTS];
uint8_t num_background_elements;
BackgroundElement wall_elements_left[MAX_WALL_ELEMENTS];
uint8_t num_wall_elements_left;
BackgroundElement wall_elements_right[MAX_WALL_ELEMENTS];
uint8_t num_wall_elements_right;

int hazards_left[MAX_HAZARDS][2]; 
uint8_t num_hazards_left;
int hazards_right[MAX_HAZARDS][2];
uint8_t num_hazards_right;

Particle particles[MAX_PARTICLES];
uint8_t num_particles;
uint24_t high_score_appvar; 


// --- Array Definitions ---
const gfx_sprite_t* gfx_sprite_pointers[NUM_SPRITE_IDS] = {
    SPRITE_TITLE, SPRITE_SCORE, SPRITE_BEST,
    SPRITE_DIGIT_0, SPRITE_DIGIT_1, SPRITE_DIGIT_2, SPRITE_DIGIT_3, SPRITE_DIGIT_4, SPRITE_DIGIT_5, SPRITE_DIGIT_6, SPRITE_DIGIT_7, SPRITE_DIGIT_8, SPRITE_DIGIT_9,
    SPRITE_DIGIT_SMALL_0, SPRITE_DIGIT_SMALL_1, SPRITE_DIGIT_SMALL_2, SPRITE_DIGIT_SMALL_3, SPRITE_DIGIT_SMALL_4, SPRITE_DIGIT_SMALL_5, SPRITE_DIGIT_SMALL_6, SPRITE_DIGIT_SMALL_7, SPRITE_DIGIT_SMALL_8, SPRITE_DIGIT_SMALL_9,
    SPRITE_WALL_LEFT, SPRITE_WALL_RIGHT, SPRITE_SPIKE_LEFT, SPRITE_SPIKE_RIGHT,
    SPRITE_WEEDS_1_LEFT, SPRITE_WEEDS_1_RIGHT, SPRITE_WEEDS_2_LEFT, SPRITE_WEEDS_2_RIGHT, SPRITE_WEEDS_3_LEFT, SPRITE_WEEDS_3_RIGHT,
    SPRITE_BRICKS_1_LEFT, SPRITE_BRICKS_1_RIGHT, SPRITE_BRICKS_2_LEFT, SPRITE_BRICKS_2_RIGHT, SPRITE_BRICKS_3_LEFT, SPRITE_BRICKS_3_RIGHT
};

const LogicalSpriteSize all_logical_sprite_sizes[NUM_SPRITE_IDS] = {
    {SPRITE_TITLE_width, SPRITE_TITLE_height}, {SPRITE_SCORE_width, SPRITE_SCORE_height}, {SPRITE_BEST_width, SPRITE_BEST_height},
    {SPRITE_DIGIT_0_width, SPRITE_DIGIT_0_height}, {SPRITE_DIGIT_1_width, SPRITE_DIGIT_1_height}, {SPRITE_DIGIT_2_width, SPRITE_DIGIT_2_height}, {SPRITE_DIGIT_3_width, SPRITE_DIGIT_3_height}, {SPRITE_DIGIT_4_width, SPRITE_DIGIT_4_height}, {SPRITE_DIGIT_5_width, SPRITE_DIGIT_5_height}, {SPRITE_DIGIT_6_width, SPRITE_DIGIT_6_height}, {SPRITE_DIGIT_7_width, SPRITE_DIGIT_7_height}, {SPRITE_DIGIT_8_width, SPRITE_DIGIT_8_height}, {SPRITE_DIGIT_9_width, SPRITE_DIGIT_9_height},
    {SPRITE_DIGIT_SMALL_0_width, SPRITE_DIGIT_SMALL_0_height}, {SPRITE_DIGIT_SMALL_1_width, SPRITE_DIGIT_SMALL_1_height}, {SPRITE_DIGIT_SMALL_2_width, SPRITE_DIGIT_SMALL_2_height}, {SPRITE_DIGIT_SMALL_3_width, SPRITE_DIGIT_SMALL_3_height}, {SPRITE_DIGIT_SMALL_4_width, SPRITE_DIGIT_SMALL_4_height}, {SPRITE_DIGIT_SMALL_5_width, SPRITE_DIGIT_SMALL_5_height}, {SPRITE_DIGIT_SMALL_6_width, SPRITE_DIGIT_SMALL_6_height}, {SPRITE_DIGIT_SMALL_7_width, SPRITE_DIGIT_SMALL_7_height}, {SPRITE_DIGIT_SMALL_8_width, SPRITE_DIGIT_SMALL_8_height}, {SPRITE_DIGIT_SMALL_9_width, SPRITE_DIGIT_SMALL_9_height},
    {SPRITE_WALL_LEFT_width, SPRITE_WALL_LEFT_height}, {SPRITE_WALL_RIGHT_width, SPRITE_WALL_RIGHT_height},
    {SPRITE_SPIKE_LEFT_width, SPRITE_SPIKE_LEFT_height}, {SPRITE_SPIKE_RIGHT_width, SPRITE_SPIKE_RIGHT_height},
    {SPRITE_WEEDS_1_LEFT_width, SPRITE_WEEDS_1_LEFT_height}, {SPRITE_WEEDS_1_RIGHT_width, SPRITE_WEEDS_1_RIGHT_height},
    {SPRITE_WEEDS_2_LEFT_width, SPRITE_WEEDS_2_LEFT_height}, {SPRITE_WEEDS_2_RIGHT_width, SPRITE_WEEDS_2_RIGHT_height},
    {SPRITE_WEEDS_3_LEFT_width, SPRITE_WEEDS_3_LEFT_height}, {SPRITE_WEEDS_3_RIGHT_width, SPRITE_WEEDS_3_RIGHT_height},
    {SPRITE_BRICKS_1_LEFT_width, SPRITE_BRICKS_1_LEFT_height}, {SPRITE_BRICKS_1_RIGHT_width, SPRITE_BRICKS_1_RIGHT_height},
    {SPRITE_BRICKS_2_LEFT_width, SPRITE_BRICKS_2_LEFT_height}, {SPRITE_BRICKS_2_RIGHT_width, SPRITE_BRICKS_2_RIGHT_height},
    {SPRITE_BRICKS_3_LEFT_width, SPRITE_BRICKS_3_LEFT_height}, {SPRITE_BRICKS_3_RIGHT_width, SPRITE_BRICKS_3_RIGHT_height}
};

const uint8_t LEVEL_GEN_BG_ELEM_SPRITE_IDS[NUM_LEVEL_GEN_BG_SPRITE_IDS] = {
    SPRITE_ID_BRICKS_1_LEFT, SPRITE_ID_BRICKS_2_LEFT, SPRITE_ID_BRICKS_3_LEFT,
    SPRITE_ID_BRICKS_1_RIGHT, SPRITE_ID_BRICKS_2_RIGHT, SPRITE_ID_BRICKS_3_RIGHT
};
const uint8_t LEVEL_GEN_WALL_ELEM_SPRITE_IDS[NUM_LEVEL_GEN_WALL_SPRITE_IDS] = {
    SPRITE_ID_WEEDS_1_LEFT, SPRITE_ID_WEEDS_2_LEFT, SPRITE_ID_WEEDS_3_LEFT
};


// --- Function Definitions ---
static void generate_side_hazards(int (*hazard_list_ptr)[2], uint8_t *num_hazards_count_ptr, float start_y, float generation_limit_y, int hazard_separation, int hazard_separation_variation, int hazard_size_min, int hazard_size_max);

float clampf(float num, float min_value, float max_value) {
    if (num < min_value) return min_value;
    if (num > max_value) return max_value;
    return num;
}

float lerpf(float ratio, float min_value, float max_value) {
    return min_value + ratio * (max_value - min_value);
}

bool StateIsGrabbing(void) {
    return current_state == STATE_GRABBING_LEFT || current_state == STATE_GRABBING_RIGHT;
}
bool StateIsJumping(void) {
    return current_state == STATE_JUMPING_LEFT_ACTIVE || current_state == STATE_JUMPING_LEFT_PASSIVE ||
           current_state == STATE_JUMPING_RIGHT_ACTIVE || current_state == STATE_JUMPING_RIGHT_PASSIVE;
}
bool StateIsJumpingLeft(void) {
    return current_state == STATE_JUMPING_LEFT_ACTIVE || current_state == STATE_JUMPING_LEFT_PASSIVE;
}
bool StateIsJumpingRight(void) {
    return current_state == STATE_JUMPING_RIGHT_ACTIVE || current_state == STATE_JUMPING_RIGHT_PASSIVE;
}
bool StateIsJumpingActive(void) {
    return current_state == STATE_JUMPING_LEFT_ACTIVE || current_state == STATE_JUMPING_RIGHT_ACTIVE;
}
bool StateIsJumpingPassive(void) {
    return current_state == STATE_JUMPING_LEFT_PASSIVE || current_state == STATE_JUMPING_RIGHT_PASSIVE;
}
bool StateIsPlaying(void) {
    return StateIsGrabbing() || StateIsJumping();
}

void SwitchState(GameState new_state) {
    if (current_state != new_state) {
        current_state = new_state;
        time_since_last_state_change = 0.0f;
    }
}

float get_jump_speed_factor(void) {
    return is_boosting ? JUMP_CHAIN_BOOST_FACTOR : 1.0f;
}

static void generate_side_hazards(int (*hazard_list_ptr)[2], uint8_t *num_hazards_count_ptr, float start_y, float generation_limit_y, int hazard_separation, int hazard_separation_variation, int hazard_size_min, int hazard_size_max) {
    float next_y = start_y;
    while (next_y < generation_limit_y && *num_hazards_count_ptr < MAX_HAZARDS) {
        next_y += (float)(rand() % (hazard_separation_variation * 2 + 1)) + (float)(hazard_separation - hazard_separation_variation);
        if (next_y >= generation_limit_y) break;

        hazard_list_ptr[*num_hazards_count_ptr][0] = (int)next_y; 
        if (hazard_size_max > hazard_size_min) {
            hazard_list_ptr[*num_hazards_count_ptr][1] = hazard_size_min + (rand() % (hazard_size_max - hazard_size_min + 1)); 
        } else {
            hazard_list_ptr[*num_hazards_count_ptr][1] = hazard_size_min;
        }
        (*num_hazards_count_ptr)++;
    }
}

void create_level_elements(float current_y_pos_camera_bottom) {
    if (current_y_pos_camera_bottom < last_generation_height + LEVEL_GEN_MINIMUM_HEIGHT_INCREASE && last_generation_height > -LEVEL_GEN_MINIMUM_HEIGHT_INCREASE) {
        return; 
    }
    last_generation_height = (int)current_y_pos_camera_bottom; 

    float generation_limit_y_hazards = current_y_pos_camera_bottom + SCREEN_HEIGHT + LEVEL_GEN_HAZARDS_END_HEIGHT_MARGIN;
    float start_y_h_left = (num_hazards_left > 0) ? (float)hazards_left[num_hazards_left - 1][0] + (4.0f * hazards_left[num_hazards_left - 1][1]) : (float)LEVEL_GEN_HAZARD_START_HEIGHT;
    float start_y_h_right = (num_hazards_right > 0) ? (float)hazards_right[num_hazards_right - 1][0] + (4.0f * hazards_right[num_hazards_right - 1][1]) : (float)LEVEL_GEN_HAZARD_START_HEIGHT;

    float ratio = (current_y_pos_camera_bottom - (float)LEVEL_GEN_HAZARD_START_HEIGHT) / ((float)LEVEL_GEN_HAZARD_END_HEIGHT - (float)LEVEL_GEN_HAZARD_START_HEIGHT);
    ratio = clampf(ratio, 0.0f, 1.0f);

    int h_sep = (int)roundf(lerpf(ratio, (float)LEVEL_GEN_HAZARD_START_SEPARATION, (float)LEVEL_GEN_HAZARD_END_SEPARATION));
    int h_sep_var = (int)roundf(lerpf(ratio, (float)LEVEL_GEN_HAZARD_START_SEPARATION_VARIATION, (float)LEVEL_GEN_HAZARD_END_SEPARATION_VARIATION));
    int h_size_min = (int)roundf(lerpf(ratio, (float)LEVEL_GEN_HAZARD_START_SIZE_MIN, (float)LEVEL_GEN_HAZARD_END_SIZE_MIN));
    int h_size_max = (int)roundf(lerpf(ratio, (float)LEVEL_GEN_HAZARD_START_SIZE_MAX, (float)LEVEL_GEN_HAZARD_END_SIZE_MAX));
    if(h_size_min <=0) h_size_min = 1;
    if(h_size_max < h_size_min) h_size_max = h_size_min;

    generate_side_hazards(hazards_left, &num_hazards_left, start_y_h_left, generation_limit_y_hazards, h_sep, h_sep_var, h_size_min, h_size_max);
    generate_side_hazards(hazards_right, &num_hazards_right, start_y_h_right, generation_limit_y_hazards, h_sep, h_sep_var, h_size_min, h_size_max);

    float generation_limit_y_bg = current_y_pos_camera_bottom + SCREEN_HEIGHT + LEVEL_GEN_BG_ELEMS_END_HEIGHT_MARGIN;
    float start_y_bg = (num_background_elements > 0) ?
                       (background_elements[num_background_elements - 1].world_pos[1] + all_logical_sprite_sizes[background_elements[num_background_elements - 1].sprite_id].height)
                       : (float)LEVEL_GEN_BG_ELEM_START_HEIGHT;
    float next_y_bg = start_y_bg;
    uint8_t recent_bg_sprite_ids[3] = {0,0,0}; 
    uint8_t recent_bg_s_count = 0;

    while (next_y_bg / LEVEL_GEN_BG_ELEM_PARALLAX_FACTOR < generation_limit_y_bg &&
           num_background_elements < MAX_BACKGROUND_ELEMENTS) {
        next_y_bg += (float)(rand() % (LEVEL_GEN_BG_ELEM_SEPARATION_VARIATION * 2 + 1)) + (float)(LEVEL_GEN_BG_ELEM_SEPARATION - LEVEL_GEN_BG_ELEM_SEPARATION_VARIATION);
        if (next_y_bg / LEVEL_GEN_BG_ELEM_PARALLAX_FACTOR >= generation_limit_y_bg) break;

        SpriteID chosen_bg_sprite_id = (SpriteID)get_random_from_list_without_repeating(LEVEL_GEN_BG_ELEM_SPRITE_IDS, NUM_LEVEL_GEN_BG_SPRITE_IDS, recent_bg_sprite_ids, &recent_bg_s_count, 3);
        uint8_t sprite_w = all_logical_sprite_sizes[chosen_bg_sprite_id].width;
        // Ensure these background elements are generated within the central playable shaft
        int available_shaft_width = SCREEN_WIDTH - 2 * WALL_WIDTH - sprite_w;
        int x_pos_in_shaft;
        if (available_shaft_width > 0) {
             x_pos_in_shaft = rand() % (available_shaft_width + 1);
        } else {
            x_pos_in_shaft = 0; // Sprite is as wide or wider than shaft, place at left edge of shaft
        }


        background_elements[num_background_elements].world_pos[0] = (float)(WALL_WIDTH + x_pos_in_shaft);
        background_elements[num_background_elements].world_pos[1] = next_y_bg;
        background_elements[num_background_elements].parallax_factor = LEVEL_GEN_BG_ELEM_PARALLAX_FACTOR;
        background_elements[num_background_elements].sprite_id = chosen_bg_sprite_id;
        num_background_elements++;
        next_y_bg += all_logical_sprite_sizes[chosen_bg_sprite_id].height; 
    }

    float generation_limit_y_wall = current_y_pos_camera_bottom + SCREEN_HEIGHT + LEVEL_GEN_BG_ELEMS_END_HEIGHT_MARGIN; 
    float start_y_wall_l = (num_wall_elements_left > 0) ?
                              (wall_elements_left[num_wall_elements_left - 1].world_pos[1] + all_logical_sprite_sizes[wall_elements_left[num_wall_elements_left - 1].sprite_id].height)
                              : (float)LEVEL_GEN_WALL_ELEM_START_HEIGHT;
    float start_y_wall_r = (num_wall_elements_right > 0) ?
                               (wall_elements_right[num_wall_elements_right - 1].world_pos[1] + all_logical_sprite_sizes[wall_elements_right[num_wall_elements_right - 1].sprite_id].height)
                               : (float)LEVEL_GEN_WALL_ELEM_START_HEIGHT;
    float next_y_wall = fmaxf(start_y_wall_l, start_y_wall_r);
    uint8_t recent_wall_sprite_ids[1] = {0}; 
    uint8_t recent_wall_s_count = 0;

    while (next_y_wall < generation_limit_y_wall &&
           (num_wall_elements_left < MAX_WALL_ELEMENTS || num_wall_elements_right < MAX_WALL_ELEMENTS) ) {
        next_y_wall += (float)(rand() % (LEVEL_GEN_WALL_ELEM_SEPARATION_VARIATION * 2 + 1)) + (float)(LEVEL_GEN_WALL_ELEM_SEPARATION - LEVEL_GEN_WALL_ELEM_SEPARATION_VARIATION);
        if (next_y_wall >= generation_limit_y_wall) break;

        SpriteID base_sprite_id_wall = (SpriteID)get_random_from_list_without_repeating(LEVEL_GEN_WALL_ELEM_SPRITE_IDS, NUM_LEVEL_GEN_WALL_SPRITE_IDS, recent_wall_sprite_ids, &recent_wall_s_count, 1);
        bool is_left_side = (rand() % 2 == 0);
        SpriteID final_sprite_id_wall;
        if (!is_left_side) {
            if (base_sprite_id_wall == SPRITE_ID_WEEDS_1_LEFT) final_sprite_id_wall = SPRITE_ID_WEEDS_1_RIGHT;
            else if (base_sprite_id_wall == SPRITE_ID_WEEDS_2_LEFT) final_sprite_id_wall = SPRITE_ID_WEEDS_2_RIGHT;
            else if (base_sprite_id_wall == SPRITE_ID_WEEDS_3_LEFT) final_sprite_id_wall = SPRITE_ID_WEEDS_3_RIGHT;
            else final_sprite_id_wall = base_sprite_id_wall; 
        } else {
            final_sprite_id_wall = base_sprite_id_wall;
        }

        uint8_t sprite_w_wall = all_logical_sprite_sizes[final_sprite_id_wall].width;
        uint8_t sprite_h_wall = all_logical_sprite_sizes[final_sprite_id_wall].height;
        // Corrected X-position for wall elements
        int x_pos_wall = is_left_side ? WALL_WIDTH : (SCREEN_WIDTH - WALL_WIDTH - sprite_w_wall);
        bool overlap_found = false;
        int (*hazards_to_check)[2] = is_left_side ? hazards_left : hazards_right;
        uint8_t num_hazards_to_check = is_left_side ? num_hazards_left : num_hazards_right;

        for (uint8_t k = 0; k < num_hazards_to_check; ++k) {
            int hazard_y = hazards_to_check[k][0];
            int hazard_h = 4 * hazards_to_check[k][1]; 
            if ((next_y_wall < hazard_y + hazard_h + LEVEL_GEN_WALL_ELEM_HAZARD_BUFFER) &&
                (next_y_wall + sprite_h_wall > hazard_y - LEVEL_GEN_WALL_ELEM_HAZARD_BUFFER)) {
                overlap_found = true;
                break;
            }
        }

        if (!overlap_found) {
            BackgroundElement *target_wall_list = is_left_side ? wall_elements_left : wall_elements_right;
            uint8_t *num_target_wall_elements = is_left_side ? &num_wall_elements_left : &num_wall_elements_right;
            if (*num_target_wall_elements < MAX_WALL_ELEMENTS) {
                target_wall_list[*num_target_wall_elements].world_pos[0] = (float)x_pos_wall;
                target_wall_list[*num_target_wall_elements].world_pos[1] = next_y_wall;
                target_wall_list[*num_target_wall_elements].parallax_factor = 1; 
                target_wall_list[*num_target_wall_elements].sprite_id = final_sprite_id_wall;
                (*num_target_wall_elements)++;
            }
        }
         next_y_wall += sprite_h_wall; 
    }
}

void init_game_state(void) {
    f_camera_bottom_y = 0.0f;
    last_generation_height = -LEVEL_GEN_MINIMUM_HEIGHT_INCREASE; 
    player_pos[0] = (float)WALL_WIDTH;
    player_pos[1] = 2.0f;
    player_vel[0] = 0.0f;
    player_vel[1] = 0.0f;
    player_size[0] = PLAYER_WIDTH; 
    player_size[1] = PLAYER_HEIGHT;
    key_pressed_start_time = 0.0f;
    time_since_last_state_change = 0.0f;
    time_since_game_start = 0.0f; 
    jump_start_time = 0.0f;
    grab_start_time = 0.0f;
    full_jump_chain = 0;
    is_boosting = false;
    last_boost_particle_gen_time = 0.0f;
    game_over_time = 0.0f;
    last_score = 0;
    last_blink_start = 0.0f; 
    is_grounded = true;
    time_since_grounded = 0.0f;
    num_particles = 0;
    num_hazards_left = 0;
    num_hazards_right = 0;
    num_background_elements = 0;
    num_wall_elements_left = 0;
    num_wall_elements_right = 0;

    if (num_hazards_left < MAX_HAZARDS) {
        hazards_left[num_hazards_left][0] = 50; 
        hazards_left[num_hazards_left][1] = 3;  
        num_hazards_left++;
    }
    current_state = STATE_TITLE_SCREEN; 
    create_level_elements(f_camera_bottom_y); 
}

void update_game_logic(bool key_pressed, float delta_time) {
    static bool key_was_pressed_internal = false; 
    bool key_just_pressed_this_frame = false;

    time_since_last_state_change += delta_time;
    time_since_game_start += delta_time; 

    if (!key_was_pressed_internal && key_pressed) {
        key_just_pressed_this_frame = true;
        key_pressed_start_time = time_since_game_start; 
    }
    key_was_pressed_internal = key_pressed;

    float key_press_duration = key_pressed ? (time_since_game_start - key_pressed_start_time) : 0.0f;
    bool key_input_buffered = key_just_pressed_this_frame || (key_pressed && (key_press_duration <= KEY_PRESS_BUFFER_TIME));
    bool game_over_this_frame = false;

    if (current_state != STATE_TITLE_SCREEN && current_state != STATE_GAME_OVER) {
         create_level_elements(f_camera_bottom_y);
    }

    if (StateIsPlaying()) { 
        if (!is_grounded || player_vel[1] > 0.0f || StateIsGrabbing()) { 
            player_vel[1] -= delta_time * GRAVITY;
        }
    }

    float current_jump_time = StateIsJumping() ? (time_since_game_start - jump_start_time) : 0.0f;
    float current_grab_time = StateIsGrabbing() ? (time_since_game_start - grab_start_time) : 0.0f;

    if (current_state == STATE_TITLE_SCREEN) {
        player_vel[0] = 0.0f; player_vel[1] = 0.0f;
        player_pos[0] = (float)WALL_WIDTH; player_pos[1] = 2.0f;
        is_grounded = true; time_since_grounded = 0.0f;
        f_camera_bottom_y = 0.0f; 
        full_jump_chain = 0; is_boosting = false; 
        if (key_just_pressed_this_frame) {
            // Reset relevant game-play variables when starting from title
            player_pos[0] = (float)WALL_WIDTH; 
            player_pos[1] = 2.0f;
            player_vel[0] = 0.0f; player_vel[1] = 0.0f;
            is_grounded = true; time_since_grounded = 0.0f;
            game_over_time = 0.0f; last_score = 0;
            num_particles = 0;
            // Do not reset f_camera_bottom_y, last_generation_height, hazards, or background elements here,
            // as init_game_state already handles their initial setup.
            // Resetting them here would clear the initial level if the player restarts from title multiple times.
            SwitchState(STATE_GRABBING_LEFT); 
        }
    } else if (current_state == STATE_GAME_OVER) {
        float time_after_game_over = time_since_game_start - game_over_time;
        if (time_after_game_over > TIME_TO_IGNORE_INPUT_AFTER_GAMEOVER) {
            if (key_just_pressed_this_frame) {
                init_game_state(); 
            }
        }
        if (time_after_game_over <= SCREENSHAKE_AFTER_GAMEOVER_TIME) {
            current_shake_offset_x = (rand() % (GAME_UPSCALE_FACTOR * 2 + 1)) - GAME_UPSCALE_FACTOR;
            current_shake_offset_y = (rand() % (GAME_UPSCALE_FACTOR * 2 + 1)) - GAME_UPSCALE_FACTOR;
        } else {
            current_shake_offset_x = 0;
            current_shake_offset_y = 0;
        }
    } else { 
        current_shake_offset_x = 0; 
        current_shake_offset_y = 0;

        if (current_state == STATE_GRABBING_LEFT) {
            player_pos[0] = (float)WALL_WIDTH; 
            player_vel[0] = 0.0f;              
            if (key_input_buffered) {
                is_boosting = (full_jump_chain >= NUM_JUMPS_FOR_ACCUMULATE);
                player_vel[0] = JUMP_INITIAL_SPEED_X * get_jump_speed_factor();
                player_vel[1] = JUMP_INITIAL_SPEED_Y * get_jump_speed_factor();
                SwitchState(STATE_JUMPING_RIGHT_ACTIVE);
                jump_start_time = time_since_game_start;
                is_grounded = false;
                if (DRAW_PARTICLES) {
                    int num_new_particles = rand() % 3 + 2; 
                    for (int i_p = 0; i_p < num_new_particles && num_particles < MAX_PARTICLES; ++i_p) {
                        particles[num_particles].pos[0] = player_pos[0] + player_size[0] -1; 
                        particles[num_particles].pos[1] = player_pos[1] + (rand() % player_size[1]);
                        particles[num_particles].vel[0] = (float)(rand() % 9 + 1); 
                        particles[num_particles].vel[1] = JUMP_INITIAL_SPEED_Y * (0.3f + (float)(rand() % 401) / 1000.0f); 
                        particles[num_particles].time_left = 0.5f;
                        particles[num_particles].blink_time = 0.5f; 
                        particles[num_particles].blink_rate = 3.0f / 60.0f;
                        particles[num_particles].gravity_factor = 1.0f; 
                        particles[num_particles].use_gravity = true;
                        particles[num_particles].size[0] = 1; particles[num_particles].size[1] = 1;
                        num_particles++;
                    }
                }
            } else { 
                if (current_grab_time > MAX_GRAB_TIME_FOR_ACCUM_TO_CONTINUE) {
                    full_jump_chain = 0; is_boosting = false;
                }
            }
        } else if (current_state == STATE_GRABBING_RIGHT) {
            player_pos[0] = (float)SCREEN_WIDTH - WALL_WIDTH - player_size[0]; 
            player_vel[0] = 0.0f;               
            if (key_input_buffered) {
                is_boosting = (full_jump_chain >= NUM_JUMPS_FOR_ACCUMULATE);
                player_vel[0] = -JUMP_INITIAL_SPEED_X * get_jump_speed_factor();
                player_vel[1] = JUMP_INITIAL_SPEED_Y * get_jump_speed_factor();
                SwitchState(STATE_JUMPING_LEFT_ACTIVE);
                jump_start_time = time_since_game_start;
                is_grounded = false;
                if (DRAW_PARTICLES) {
                    int num_new_particles = rand() % 3 + 2; 
                    for (int i_p = 0; i_p < num_new_particles && num_particles < MAX_PARTICLES; ++i_p) {
                        particles[num_particles].pos[0] = player_pos[0]; 
                        particles[num_particles].pos[1] = player_pos[1] + (rand() % player_size[1]);
                        particles[num_particles].vel[0] = -(float)(rand() % 9 + 1); 
                        particles[num_particles].vel[1] = JUMP_INITIAL_SPEED_Y * (0.3f + (float)(rand() % 401) / 1000.0f); 
                        particles[num_particles].time_left = 0.5f;
                        particles[num_particles].blink_time = 0.5f;
                        particles[num_particles].blink_rate = 3.0f / 60.0f;
                        particles[num_particles].gravity_factor = 1.0f;
                        particles[num_particles].use_gravity = true;
                        particles[num_particles].size[0] = 1; particles[num_particles].size[1] = 1;
                        num_particles++;
                    }
                }
            } else { 
                if (current_grab_time > MAX_GRAB_TIME_FOR_ACCUM_TO_CONTINUE) {
                    full_jump_chain = 0; is_boosting = false;
                }
            }
        } else if (current_state == STATE_JUMPING_RIGHT_ACTIVE) {
            if (key_pressed) { 
                if (current_jump_time <= JUMP_KEY_TIME_ACCUMULATE) {
                    float ratio = current_jump_time / JUMP_KEY_TIME_ACCUMULATE;
                    player_vel[0] = lerpf(ratio, JUMP_INITIAL_SPEED_X, JUMP_SPEED_CONTINUATION_SPEED_X) * get_jump_speed_factor();
                    player_vel[1] = lerpf(ratio, JUMP_INITIAL_SPEED_Y, JUMP_SPEED_CONTINUATION_SPEED_Y) * get_jump_speed_factor();
                } else { 
                    SwitchState(STATE_JUMPING_RIGHT_PASSIVE);
                    if(is_boosting) full_jump_chain++; else full_jump_chain = 1;
                }
            } else { 
                SwitchState(STATE_JUMPING_RIGHT_PASSIVE);
                full_jump_chain = 0; is_boosting = false; 
            }
        } else if (current_state == STATE_JUMPING_LEFT_ACTIVE) {
            if (key_pressed) { 
                if (current_jump_time <= JUMP_KEY_TIME_ACCUMULATE) {
                    float ratio = current_jump_time / JUMP_KEY_TIME_ACCUMULATE;
                    player_vel[0] = -lerpf(ratio, JUMP_INITIAL_SPEED_X, JUMP_SPEED_CONTINUATION_SPEED_X) * get_jump_speed_factor();
                    player_vel[1] = lerpf(ratio, JUMP_INITIAL_SPEED_Y, JUMP_SPEED_CONTINUATION_SPEED_Y) * get_jump_speed_factor();
                } else { 
                    SwitchState(STATE_JUMPING_LEFT_PASSIVE);
                     if(is_boosting) full_jump_chain++; else full_jump_chain = 1;
                }
            } else { 
                SwitchState(STATE_JUMPING_LEFT_PASSIVE);
                full_jump_chain = 0; is_boosting = false; 
            }
        }
    }

    player_vel[0] = clampf(player_vel[0], -MAX_SPEED, MAX_SPEED);
    if (!StateIsGrabbing()) { 
        player_vel[1] = clampf(player_vel[1], -MAX_SPEED, MAX_SPEED);
    } else { 
        player_vel[1] = clampf(player_vel[1], -MAX_WALL_SLIDE_SPEED, MAX_SPEED);
    }

    if (StateIsPlaying()) { 
        player_pos[0] += delta_time * player_vel[0];
        player_pos[1] += delta_time * player_vel[1];
    }

    if (StateIsPlaying()) {
        if (player_pos[0] <= (float)WALL_WIDTH) {
            player_pos[0] = (float)WALL_WIDTH;
            if(player_vel[0] < 0.0f) player_vel[0] = 0.0f; 
            if (StateIsJumpingLeft()) { 
                SwitchState(STATE_GRABBING_LEFT);
                grab_start_time = time_since_game_start;
            }
        }
        if (player_pos[0] + player_size[0] >= (float)SCREEN_WIDTH - WALL_WIDTH) {
            player_pos[0] = (float)SCREEN_WIDTH - WALL_WIDTH - player_size[0];
            if(player_vel[0] > 0.0f) player_vel[0] = 0.0f; 
            if (StateIsJumpingRight()) { 
                SwitchState(STATE_GRABBING_RIGHT);
                grab_start_time = time_since_game_start;
            }
        }

        bool previously_grounded = is_grounded;
        if (player_pos[1] < 2.0f) { 
            if (f_camera_bottom_y == 0.0f) { 
                is_grounded = true;
                player_pos[1] = 2.0f;
                if(player_vel[1] < 0.0f) player_vel[1] = 0.0f; 
                if(StateIsJumping()){ 
                    if (player_pos[0] <= (float)SCREEN_WIDTH / 2.0f) { 
                        SwitchState(STATE_GRABBING_LEFT);
                    } else {
                        SwitchState(STATE_GRABBING_RIGHT);
                    }
                    grab_start_time = time_since_game_start;
                }
            } else { 
                if (player_pos[1] + PLAYER_HEIGHT <= f_camera_bottom_y) { 
                    game_over_this_frame = true;
                }
                is_grounded = false; 
            }
        } else { 
            is_grounded = false;
            if (f_camera_bottom_y > 0.0f && (player_pos[1] + PLAYER_HEIGHT <= f_camera_bottom_y) ) {
                 game_over_this_frame = true;
            }
        }


        if (is_grounded) {
            if (!previously_grounded && StateIsPlaying()){ 
                 full_jump_chain = 0;
                 is_boosting = false;
            }
            time_since_grounded += delta_time;
        } else {
            time_since_grounded = 0.0f;
        }
    }
    
    uint8_t anim_player_w = PLAYER_WIDTH;
    uint8_t anim_player_h = PLAYER_HEIGHT;
    if (StateIsGrabbing()) {
        if (time_since_last_state_change < 0.1f) { 
            anim_player_w = PLAYER_WIDTH - 1;
            anim_player_h = PLAYER_HEIGHT + 1;
        }
    }
    if (StateIsGrabbing() && is_grounded && time_since_grounded <= 0.1f && f_camera_bottom_y == 0.0f) {
        anim_player_h = PLAYER_HEIGHT - 1;
    }
    if (anim_player_w == 0) anim_player_w = 1; 
    if (anim_player_h == 0) anim_player_h = 1; 

    player_rect[0] = (int)roundf(player_pos[0]);
    player_rect[1] = (int)roundf(player_pos[1]); 
    player_rect[2] = player_rect[0] + PLAYER_WIDTH - 1;  
    player_rect[3] = player_rect[1] + PLAYER_HEIGHT - 1; 

    if (StateIsPlaying()) { 
      f_camera_bottom_y = fmaxf(f_camera_bottom_y, player_pos[1] - SCROLL_MIN_HEIGHT);
    }

    if (!GOD_MODE && StateIsPlaying()) {
        int p_bottom = player_rect[1]; 
        int p_top    = player_rect[3]; 
        int p_left   = player_rect[0];
        int p_right  = player_rect[2];

        if (p_left < WALL_WIDTH + 2) { 
            for (uint8_t k_idx = 0; k_idx < num_hazards_left; ++k_idx) {
                int h_bottom = hazards_left[k_idx][0];
                int h_top    = h_bottom + (4 * hazards_left[k_idx][1]) - 1; 
                if (p_bottom <= h_top && p_top >= h_bottom) { 
                    if (p_right >= WALL_WIDTH && p_left <= WALL_WIDTH + all_logical_sprite_sizes[SPRITE_ID_SPIKE_LEFT].width -1 ) {
                        game_over_this_frame = true; break;
                    }
                }
            }
        }
        if (game_over_this_frame) goto end_collision_logic_label_final_fix; 

        if (p_right > SCREEN_WIDTH - WALL_WIDTH - 2 ) { 
            for (uint8_t k_idx = 0; k_idx < num_hazards_right; ++k_idx) {
                int h_bottom = hazards_right[k_idx][0];
                int h_top    = h_bottom + (4 * hazards_right[k_idx][1]) - 1;
                if (p_bottom <= h_top && p_top >= h_bottom) { 
                    if (p_left <= (SCREEN_WIDTH - WALL_WIDTH -1) && p_right >= (SCREEN_WIDTH - WALL_WIDTH - all_logical_sprite_sizes[SPRITE_ID_SPIKE_RIGHT].width)) {
                        game_over_this_frame = true; break;
                    }
                }
            }
        }
    }
    end_collision_logic_label_final_fix:; 

    if (game_over_this_frame && current_state != STATE_GAME_OVER) {
        game_over_time = time_since_game_start;
        last_score = (uint24_t)(roundf(f_camera_bottom_y + SCROLL_MIN_HEIGHT) / PLAYER_HEIGHT);
        uint8_t appvar_handle = ti_Open("FLUCHTHS", "r");
        if (appvar_handle) {
            ti_Read(&high_score_appvar, sizeof(uint24_t), 1, appvar_handle);
            ti_Close(appvar_handle);
        } else {
            high_score_appvar = 0;
        }

        if (last_score > high_score_appvar) {
            high_score_appvar = last_score;
            appvar_handle = ti_Open("FLUCHTHS", "w");
            if (appvar_handle) {
                ti_Write(&high_score_appvar, sizeof(uint24_t), 1, appvar_handle);
                ti_SetArchiveStatus(true, appvar_handle); 
                ti_Close(appvar_handle);
            }
        }
        SwitchState(STATE_GAME_OVER);

        if (DRAW_PARTICLES) {
            float avg_x = (player_rect[0] + player_rect[2]) / 2.0f;
            float avg_y = (player_rect[1] + player_rect[3]) / 2.0f;
            uint8_t explosion_w = player_rect[2] - player_rect[0] + 1;
            uint8_t explosion_h = player_rect[3] - player_rect[1] + 1;

            for (int px_x = 0; px_x < explosion_w; ++px_x) {
                for (int px_y = 0; px_y < explosion_h; ++px_y) {
                    if (num_particles >= MAX_PARTICLES) break;
                    float offset_x = (player_rect[0] + px_x) - avg_x;
                    float offset_y = (player_rect[1] + px_y) - avg_y;

                    particles[num_particles].pos[0] = (float)(player_rect[0] + px_x);
                    particles[num_particles].pos[1] = (float)(player_rect[1] + px_y);
                    particles[num_particles].vel[0] = offset_x * ((float)(rand() % 100) / 99.0f) * 5.0f;
                    particles[num_particles].vel[1] = offset_y * ((float)(rand() % 100) / 99.0f) * 5.0f + 10.0f; 
                    particles[num_particles].time_left = 0.7f + ((float)(rand() % 301) / 1000.0f); 
                    particles[num_particles].blink_time = particles[num_particles].time_left * 0.5f;
                    particles[num_particles].blink_rate = 3.0f / 60.0f; 
                    particles[num_particles].gravity_factor = 0.5f;
                    particles[num_particles].use_gravity = true;
                    particles[num_particles].size[0] = 1; particles[num_particles].size[1] = 1; 
                    num_particles++;
                }
                if (num_particles >= MAX_PARTICLES) break;
            }
        }
    }

    if (DRAW_PARTICLES) {
        uint8_t live_particle_idx = 0;
        for (uint8_t i = 0; i < num_particles; ++i) {
            if (particles[i].use_gravity) {
                particles[i].vel[1] -= GRAVITY * particles[i].gravity_factor * delta_time;
            }
            particles[i].pos[0] += particles[i].vel[0] * delta_time;
            particles[i].pos[1] += particles[i].vel[1] * delta_time;
            particles[i].time_left -= delta_time;

            if (particles[i].time_left > 0.0f && (particles[i].pos[1] + particles[i].size[1] >= f_camera_bottom_y)) {
                if (live_particle_idx != i) { 
                    particles[live_particle_idx] = particles[i];
                }
                live_particle_idx++;
            }
        }
        num_particles = live_particle_idx;
    }

    int camera_bottom_y_int_cull = (int)roundf(f_camera_bottom_y);
    uint8_t live_idx;

    live_idx = 0;
    for(uint8_t i = 0; i < num_hazards_left; ++i) {
        if (hazards_left[i][0] + (4 * hazards_left[i][1]) >= camera_bottom_y_int_cull) { 
            if (live_idx != i) { hazards_left[live_idx][0] = hazards_left[i][0]; hazards_left[live_idx][1] = hazards_left[i][1]; }
            live_idx++;
        }
    }
    num_hazards_left = live_idx;

    live_idx = 0;
    for(uint8_t i = 0; i < num_hazards_right; ++i) {
        if (hazards_right[i][0] + (4 * hazards_right[i][1]) >= camera_bottom_y_int_cull) {
            if (live_idx != i) { hazards_right[live_idx][0] = hazards_right[i][0]; hazards_right[live_idx][1] = hazards_right[i][1]; }
            live_idx++;
        }
    }
    num_hazards_right = live_idx;

    live_idx = 0;
    for(uint8_t i = 0; i < num_background_elements; ++i) {
        if ( (background_elements[i].world_pos[1] / background_elements[i].parallax_factor) + all_logical_sprite_sizes[background_elements[i].sprite_id].height >= camera_bottom_y_int_cull ) {
            if (live_idx != i) { background_elements[live_idx] = background_elements[i]; }
            live_idx++;
        }
    }
    num_background_elements = live_idx;

    live_idx = 0;
    for(uint8_t i = 0; i < num_wall_elements_left; ++i) {
        if ( wall_elements_left[i].world_pos[1] + all_logical_sprite_sizes[wall_elements_left[i].sprite_id].height >= camera_bottom_y_int_cull ) {
            if (live_idx != i) { wall_elements_left[live_idx] = wall_elements_left[i]; }
            live_idx++;
        }
    }
    num_wall_elements_left = live_idx;

    live_idx = 0;
    for(uint8_t i = 0; i < num_wall_elements_right; ++i) {
        if ( wall_elements_right[i].world_pos[1] + all_logical_sprite_sizes[wall_elements_right[i].sprite_id].height >= camera_bottom_y_int_cull ) {
            if (live_idx != i) { wall_elements_right[live_idx] = wall_elements_right[i]; }
            live_idx++;
        }
    }
    num_wall_elements_right = live_idx;
}