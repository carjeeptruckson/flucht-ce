#ifndef GAME_DRAWING_H
#define GAME_DRAWING_H

#include <stdint.h>
#include <stdbool.h>
#include <graphx.h>         // For gfx_sprite_t
#include "game_constants.h" // For GAME_UPSCALE_FACTOR, SCREEN_HEIGHT etc.

// Palette indices
#define COLOR_BLACK_IDX 0 // Dark color
#define COLOR_WHITE_IDX 1 // Light color

// Shake offsets - declared extern, defined in game_drawing.c
extern int current_shake_offset_x;
extern int current_shake_offset_y;

// Drawing function declarations
void game_draw_init(void);
void game_draw_fill_screen(uint8_t color_index);
void game_draw_pixel(int lx, int ly, uint8_t color_index);
void game_draw_line(int lx1, int ly1, int lx2, int ly2, uint8_t color_index);
void game_draw_rectangle(int lx, int ly, int lw, int lh, uint8_t color_index);
void game_draw_filled_rectangle(int lx, int ly, int lw, int lh, uint8_t color_index);
// Changed 'transparent' to 'use_transparent_color_key' for clarity
void game_draw_sprite(const gfx_sprite_t *sprite, uint8_t sprite_logical_height, int lx, int ly, bool use_transparent_color_key, uint8_t transparent_color_idx_if_true);
void game_draw_number_centered(int num, int ly, bool small);

#endif // GAME_DRAWING_H