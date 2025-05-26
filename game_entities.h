#ifndef GAME_ENTITIES_H
#define GAME_ENTITIES_H

#include <stdint.h>
#include <stdbool.h>

// Define LogicalSpriteSize here
typedef struct {
    uint8_t width;
    uint8_t height;
} LogicalSpriteSize;

// Background Element structure
typedef struct BackgroundElement_t {
    float world_pos[2];
    uint8_t parallax_factor;
    uint8_t sprite_id; // Will be an enum of type SpriteID (defined in game_logic.h)
} BackgroundElement;

// Particle structure
typedef struct Particle_t {
    float pos[2];
    float vel[2];
    int8_t size[2];
    bool use_gravity;
    float time_left;
    float blink_time;
    float blink_rate;
    float gravity_factor;
} Particle;

// Helper function declaration
uint8_t get_random_from_list_without_repeating(const uint8_t *full_list, uint8_t list_size, uint8_t *recent_indices, uint8_t *recent_count, uint8_t recent_capacity);

#endif // GAME_ENTITIES_H