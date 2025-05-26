#include "game_entities.h" // This now defines BackgroundElement, Particle, LogicalSpriteSize
#include <stdlib.h>        // For rand()
// #include "game_logic.h" // Not strictly needed if game_entities.h doesn't include it and this file doesn't use other game_logic types

// The global 'all_logical_sprite_sizes' array is defined in game_logic.c
// and declared 'extern' in game_logic.h.
// This file (game_entities.c) does not need to redefine or declare it again.

uint8_t get_random_from_list_without_repeating(const uint8_t *full_list, uint8_t list_size, uint8_t *recent_indices, uint8_t *recent_count, uint8_t recent_capacity) {
    uint8_t index_val; 
    bool found_unique = false;
    int attempts = 0; 

    if (list_size == 0) {
        return 0; 
    }
    if (recent_capacity > list_size) { 
        recent_capacity = list_size;
    }

    while (!found_unique && attempts < list_size * 2 + 5) { 
        index_val = rand() % list_size;
        found_unique = true;
        
        if (recent_capacity > 0 && *recent_count > 0) {
             bool is_in_recent = false;
            for (uint8_t i = 0; i < *recent_count; ++i) {
                // We stored sprite IDs in recent_indices, and full_list contains sprite IDs
                if (recent_indices[i] == full_list[index_val]) {
                    is_in_recent = true;
                    break;
                }
            }
            // Only try to find a unique one if the list is larger than our capacity to remember
            if (is_in_recent && list_size > recent_capacity) {
                 found_unique = false;
            } else if (is_in_recent && list_size <= recent_capacity && *recent_count >= list_size) {
                // If we've seen all items and list is small, any is fine, effectively.
                // Or, if we want to ensure we pick one not *currently* in the short recent list (if list_size > recent_capacity)
                found_unique = (list_size <= recent_capacity);
            }
        }
        attempts++;
    }
    if (!found_unique) { // Fallback
       index_val = rand() % list_size; 
    }

    uint8_t chosen_item_id = full_list[index_val];

    if (recent_capacity > 0) {
        if (*recent_count == recent_capacity) {
            for (uint8_t i = 0; i < recent_capacity - 1; ++i) {
                recent_indices[i] = recent_indices[i + 1];
            }
            recent_indices[recent_capacity - 1] = chosen_item_id; 
        } else { 
            recent_indices[*recent_count] = chosen_item_id;
            (*recent_count)++;
        }
    }
    return chosen_item_id; 
}