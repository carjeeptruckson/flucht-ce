#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

// Basic types and constants FIRST
#include <stdint.h>
#include <stdbool.h>
#include "game_constants.h" // Defines MAX_..., GameState, etc.

// Types needed for declarations below
#include <graphx.h>         // For gfx_sprite_t
#include "game_entities.h"  // For BackgroundElement, Particle, LogicalSpriteSize

// Game state variables (extern declarations)
extern GameState current_state;
extern float time_since_last_state_change;
extern float f_camera_bottom_y;
extern float player_pos[2];
extern float player_vel[2];
extern uint8_t player_size[2];
extern int player_rect[4];
extern float key_pressed_start_time;
extern float jump_start_time;
extern float grab_start_time;
extern uint8_t full_jump_chain;
extern bool is_boosting;
extern float last_boost_particle_gen_time;
extern float game_over_time;
extern float time_since_game_start;
extern uint24_t last_score;
extern float last_blink_start;
extern bool is_grounded;
extern float time_since_grounded;
extern int last_generation_height;

// Game object lists (extern declarations)
extern BackgroundElement background_elements[MAX_BACKGROUND_ELEMENTS];
extern uint8_t num_background_elements;
extern BackgroundElement wall_elements_left[MAX_WALL_ELEMENTS];
extern uint8_t num_wall_elements_left;
extern BackgroundElement wall_elements_right[MAX_WALL_ELEMENTS];
extern uint8_t num_wall_elements_right;
extern int hazards_left[MAX_HAZARDS][2];
extern uint8_t num_hazards_left;
extern int hazards_right[MAX_HAZARDS][2];
extern uint8_t num_hazards_right;
extern Particle particles[MAX_PARTICLES];
extern uint8_t num_particles;
extern uint24_t high_score_appvar; // Made non-static extern

// SpriteID enum
typedef enum {
    SPRITE_ID_TITLE = 0, SPRITE_ID_SCORE, SPRITE_ID_BEST,
    SPRITE_ID_DIGIT_0, SPRITE_ID_DIGIT_1, SPRITE_ID_DIGIT_2, SPRITE_ID_DIGIT_3, SPRITE_ID_DIGIT_4, SPRITE_ID_DIGIT_5, SPRITE_ID_DIGIT_6, SPRITE_ID_DIGIT_7, SPRITE_ID_DIGIT_8, SPRITE_ID_DIGIT_9,
    SPRITE_ID_DIGIT_SMALL_0, SPRITE_ID_DIGIT_SMALL_1, SPRITE_ID_DIGIT_SMALL_2, SPRITE_ID_DIGIT_SMALL_3, SPRITE_ID_DIGIT_SMALL_4, SPRITE_ID_DIGIT_SMALL_5, SPRITE_ID_DIGIT_SMALL_6, SPRITE_ID_DIGIT_SMALL_7, SPRITE_ID_DIGIT_SMALL_8, SPRITE_ID_DIGIT_SMALL_9,
    SPRITE_ID_WALL_LEFT, SPRITE_ID_WALL_RIGHT, SPRITE_ID_SPIKE_LEFT, SPRITE_ID_SPIKE_RIGHT,
    SPRITE_ID_WEEDS_1_LEFT, SPRITE_ID_WEEDS_1_RIGHT, SPRITE_ID_WEEDS_2_LEFT, SPRITE_ID_WEEDS_2_RIGHT, SPRITE_ID_WEEDS_3_LEFT, SPRITE_ID_WEEDS_3_RIGHT,
    SPRITE_ID_BRICKS_1_LEFT, SPRITE_ID_BRICKS_1_RIGHT, SPRITE_ID_BRICKS_2_LEFT, SPRITE_ID_BRICKS_2_RIGHT, SPRITE_ID_BRICKS_3_LEFT, SPRITE_ID_BRICKS_3_RIGHT,
    NUM_SPRITE_IDS
} SpriteID;

// Pointers to the actual gfx_sprite_t objects from gfx.h
extern const gfx_sprite_t* gfx_sprite_pointers[NUM_SPRITE_IDS];

// Logical dimensions of sprites
extern const LogicalSpriteSize all_logical_sprite_sizes[NUM_SPRITE_IDS]; 

// Define sizes for these arrays (used for sizeof in game_logic.c)
#define NUM_LEVEL_GEN_BG_SPRITE_IDS 6
#define NUM_LEVEL_GEN_WALL_SPRITE_IDS 3

// Level Generation Data
extern const uint8_t LEVEL_GEN_BG_ELEM_SPRITE_IDS[NUM_LEVEL_GEN_BG_SPRITE_IDS];
extern const uint8_t LEVEL_GEN_WALL_ELEM_SPRITE_IDS[NUM_LEVEL_GEN_WALL_SPRITE_IDS];

// Game state management functions
bool StateIsGrabbing(void);
bool StateIsJumping(void);
bool StateIsJumpingLeft(void);
bool StateIsJumpingRight(void);
bool StateIsJumpingActive(void);
bool StateIsJumpingPassive(void);
bool StateIsPlaying(void);
void SwitchState(GameState new_state);
void init_game_state(void);
void update_game_logic(bool key_pressed, float delta_time);
void draw_game_elements(void); // Declaration moved from game_logic.c

#endif // GAME_LOGIC_H