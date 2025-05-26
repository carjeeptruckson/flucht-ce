#include "game_drawing.h"
#include <graphx.h>      // For all gfx_ functions
#include <string.h>      // For memset (though less used now)
#include <stdio.h>       // For snprintf in game_draw_number_centered (or manual conversion)
#include "sprites/gfx.h" // For global_palette and sprite data definitions
#include "game_logic.h"  // For SpriteID, all_logical_sprite_sizes, gfx_sprite_pointers
#include "game_constants.h" // For DRAW_PRE_CULL_MARGIN

// Definitions for shake offsets
int current_shake_offset_x = 0;
int current_shake_offset_y = 0;

void game_draw_init(void) {
    gfx_Begin();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0); 
    gfx_SetDrawBuffer();
}

void game_draw_fill_screen(uint8_t color_index) {
    gfx_FillScreen(color_index);
}

void game_draw_pixel(int lx, int ly, uint8_t color_index) {
    gfx_SetColor(color_index);
    int physical_x_left_of_pixel = GAME_AREA_X_OFFSET + lx * GAME_UPSCALE_FACTOR + current_shake_offset_x;
    int physical_y_top_of_pixel = GAME_AREA_Y_OFFSET + (SCREEN_HEIGHT - (ly + 1)) * GAME_UPSCALE_FACTOR + current_shake_offset_y;

    if (physical_x_left_of_pixel >= 0 && physical_x_left_of_pixel + GAME_UPSCALE_FACTOR <= GFX_LCD_WIDTH &&
        physical_y_top_of_pixel >= 0 && physical_y_top_of_pixel + GAME_UPSCALE_FACTOR <= GFX_LCD_HEIGHT) {
        gfx_FillRectangle_NoClip(physical_x_left_of_pixel, physical_y_top_of_pixel, GAME_UPSCALE_FACTOR, GAME_UPSCALE_FACTOR);
    } else {
        gfx_FillRectangle(physical_x_left_of_pixel, physical_y_top_of_pixel, GAME_UPSCALE_FACTOR, GAME_UPSCALE_FACTOR);
    }
}

void game_draw_line(int lx1, int ly1, int lx2, int ly2, uint8_t color_index) {
    gfx_SetColor(color_index);
    int p1x = GAME_AREA_X_OFFSET + lx1 * GAME_UPSCALE_FACTOR + current_shake_offset_x;
    int p1y = GAME_AREA_Y_OFFSET + (SCREEN_HEIGHT - 1 - ly1) * GAME_UPSCALE_FACTOR + current_shake_offset_y;
    int p2x = GAME_AREA_X_OFFSET + lx2 * GAME_UPSCALE_FACTOR + current_shake_offset_x;
    int p2y = GAME_AREA_Y_OFFSET + (SCREEN_HEIGHT - 1 - ly2) * GAME_UPSCALE_FACTOR + current_shake_offset_y;
    gfx_Line(p1x, p1y, p2x, p2y);
}

void game_draw_rectangle(int lx, int ly, int lw, int lh, uint8_t color_index) {
    gfx_SetColor(color_index);
    int px = GAME_AREA_X_OFFSET + lx * GAME_UPSCALE_FACTOR + current_shake_offset_x;
    int py = GAME_AREA_Y_OFFSET + (SCREEN_HEIGHT - (ly + lh)) * GAME_UPSCALE_FACTOR + current_shake_offset_y;
    int pw = lw * GAME_UPSCALE_FACTOR;
    int ph = lh * GAME_UPSCALE_FACTOR;
    gfx_Rectangle(px, py, pw, ph);
}

void game_draw_filled_rectangle(int lx, int ly, int lw, int lh, uint8_t color_index) {
    gfx_SetColor(color_index);
    int px = GAME_AREA_X_OFFSET + lx * GAME_UPSCALE_FACTOR + current_shake_offset_x;
    int py = GAME_AREA_Y_OFFSET + (SCREEN_HEIGHT - (ly + lh)) * GAME_UPSCALE_FACTOR + current_shake_offset_y;
    int pw = lw * GAME_UPSCALE_FACTOR;
    int ph = lh * GAME_UPSCALE_FACTOR;
    gfx_FillRectangle(px, py, pw, ph);
}

void game_draw_sprite(const gfx_sprite_t *sprite, uint8_t sprite_logical_height, int lx, int ly, bool use_transparent_color_key, uint8_t transparent_color_idx_if_true) {
    if (!sprite) return;

    int physical_draw_x = GAME_AREA_X_OFFSET + lx * GAME_UPSCALE_FACTOR + current_shake_offset_x;
    int physical_draw_y = GAME_AREA_Y_OFFSET + (SCREEN_HEIGHT - (ly + sprite_logical_height)) * GAME_UPSCALE_FACTOR + current_shake_offset_y;

    int data_scaled_width = sprite->width * GAME_UPSCALE_FACTOR;
    int data_scaled_height = sprite->height * GAME_UPSCALE_FACTOR; 

    // Basic culling for sprites that are entirely off the physical screen
    if (physical_draw_x + data_scaled_width <= 0 || physical_draw_x >= GFX_LCD_WIDTH ||
        physical_draw_y + data_scaled_height <= 0 || physical_draw_y >= GFX_LCD_HEIGHT) {
        return; 
    }

    if (use_transparent_color_key) {
        gfx_SetTransparentColor(transparent_color_idx_if_true); 
        gfx_ScaledTransparentSprite_NoClip(sprite, physical_draw_x, physical_draw_y, GAME_UPSCALE_FACTOR, GAME_UPSCALE_FACTOR);
    } else {
        gfx_ScaledSprite_NoClip(sprite, physical_draw_x, physical_draw_y, GAME_UPSCALE_FACTOR, GAME_UPSCALE_FACTOR);
    }
}

void game_draw_number_centered(int num, int ly, bool small) {
    char str_num[12]; 
    uint8_t i = 0;
    uint8_t digit_count = 0;
    int temp_num = num;
    bool is_negative = false;
    
    if (temp_num == 0) {
        str_num[0] = '0';
        str_num[1] = '\0';
        digit_count = 1;
    } else {
        if (temp_num < 0) {
            is_negative = true;
            temp_num = -temp_num;
        }
        while (temp_num > 0 && i < 10) {
            str_num[i++] = (temp_num % 10) + '0';
            temp_num /= 10;
        }
        if (is_negative) {
            str_num[i++] = '-';
        }
        str_num[i] = '\0';
        digit_count = i;

        for (uint8_t j = 0; j < digit_count / 2; j++) {
            char temp_char = str_num[j];
            str_num[j] = str_num[digit_count - 1 - j];
            str_num[digit_count - 1 - j] = temp_char;
        }
    }

    SpriteID base_id_lookup = small ? SPRITE_ID_DIGIT_SMALL_0 : SPRITE_ID_DIGIT_0;
    uint8_t DIGIT_LOGICAL_WIDTH = all_logical_sprite_sizes[base_id_lookup].width;
    uint8_t DIGIT_LOGICAL_HEIGHT = all_logical_sprite_sizes[base_id_lookup].height;

    int total_logical_width = 0;
    for(uint8_t k=0; k < digit_count; ++k) {
         total_logical_width += DIGIT_LOGICAL_WIDTH;
        if (k < digit_count - 1) {
            total_logical_width += 1; 
        }
    }
        
    int logical_left = (SCREEN_WIDTH - total_logical_width) / 2;

    int rect_lx = logical_left - 2;
    int rect_ly = ly - 1;
    int rect_lw = total_logical_width + 4;
    int rect_lh = DIGIT_LOGICAL_HEIGHT + 2;

    if (rect_lx < 0) {
        rect_lw += rect_lx; 
        rect_lx = 0;
    }
    if (rect_lx + rect_lw > SCREEN_WIDTH) {
        rect_lw = SCREEN_WIDTH - rect_lx;
    }
    if (rect_lw > 0 && rect_lh > 0) { 
         game_draw_filled_rectangle(rect_lx, rect_ly, rect_lw, rect_lh, COLOR_BLACK_IDX);
    }

    int current_logical_x = logical_left;
    for (uint8_t d_idx = 0; d_idx < digit_count; ++d_idx) {
        const gfx_sprite_t* digit_sprite_ptr = NULL;
        // uint8_t current_digit_logical_width = DIGIT_LOGICAL_WIDTH; // Already have DIGIT_LOGICAL_WIDTH

        if (str_num[d_idx] == '-') {
            int minus_y_center = ly + DIGIT_LOGICAL_HEIGHT / 2;
            int minus_line_height_logical = (GAME_UPSCALE_FACTOR > 2) ? 1 : 0; 
            if (minus_line_height_logical == 0 && DIGIT_LOGICAL_HEIGHT > 2) minus_line_height_logical = 1; 
            if(minus_line_height_logical == 0) minus_line_height_logical = 1; 

            game_draw_filled_rectangle(current_logical_x + 1, minus_y_center - minus_line_height_logical/2, DIGIT_LOGICAL_WIDTH - 2, minus_line_height_logical, COLOR_WHITE_IDX);
        } else {
            uint8_t digit_val = str_num[d_idx] - '0';
            SpriteID id_to_use = (SpriteID)((small ? SPRITE_ID_DIGIT_SMALL_0 : SPRITE_ID_DIGIT_0) + digit_val);
            digit_sprite_ptr = gfx_sprite_pointers[id_to_use];
        }
        
        if (digit_sprite_ptr) {
             game_draw_sprite(digit_sprite_ptr, DIGIT_LOGICAL_HEIGHT, current_logical_x, ly, true, COLOR_BLACK_IDX);
        }
        current_logical_x += DIGIT_LOGICAL_WIDTH + 1; // Use DIGIT_LOGICAL_WIDTH
    }
}


void draw_game_elements(void) {
    int camera_bottom_y_int = (int)roundf(f_camera_bottom_y);

    game_draw_fill_screen(COLOR_BLACK_IDX);

    if (current_state != STATE_GAME_OVER || time_since_last_state_change <= SCREENSHAKE_AFTER_GAMEOVER_TIME) {
        for (uint8_t i = 0; i < num_background_elements; ++i) {
            BackgroundElement *bg_elem = &background_elements[i];
            int draw_y_bg_logical = (int)roundf(bg_elem->world_pos[1] / bg_elem->parallax_factor) - camera_bottom_y_int;
            if (draw_y_bg_logical + all_logical_sprite_sizes[bg_elem->sprite_id].height >= -DRAW_PRE_CULL_MARGIN &&
                draw_y_bg_logical < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN) {
                 int elem_lx = (int)roundf(bg_elem->world_pos[0]);
                 game_draw_sprite(gfx_sprite_pointers[bg_elem->sprite_id],
                                 all_logical_sprite_sizes[bg_elem->sprite_id].height,
                                 elem_lx, draw_y_bg_logical, 
                                 false, 0); 
            }
        }
        for (uint8_t i = 0; i < num_wall_elements_left; ++i) {
            BackgroundElement *wall_elem = &wall_elements_left[i];
            int draw_y_wall_logical = (int)roundf(wall_elem->world_pos[1]) - camera_bottom_y_int;
             if (draw_y_wall_logical + all_logical_sprite_sizes[wall_elem->sprite_id].height >= -DRAW_PRE_CULL_MARGIN &&
                 draw_y_wall_logical < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN){
                game_draw_sprite(gfx_sprite_pointers[wall_elem->sprite_id],
                                 all_logical_sprite_sizes[wall_elem->sprite_id].height,
                                 (int)roundf(wall_elem->world_pos[0]), draw_y_wall_logical, 
                                 true, COLOR_BLACK_IDX); 
            }
        }
        for (uint8_t i = 0; i < num_wall_elements_right; ++i) {
            BackgroundElement *wall_elem = &wall_elements_right[i];
            int draw_y_wall_logical = (int)roundf(wall_elem->world_pos[1]) - camera_bottom_y_int;
            if (draw_y_wall_logical + all_logical_sprite_sizes[wall_elem->sprite_id].height >= -DRAW_PRE_CULL_MARGIN &&
                draw_y_wall_logical < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN){
                 game_draw_sprite(gfx_sprite_pointers[wall_elem->sprite_id],
                                 all_logical_sprite_sizes[wall_elem->sprite_id].height,
                                 (int)roundf(wall_elem->world_pos[0]), draw_y_wall_logical, 
                                 true, COLOR_BLACK_IDX); 
            }
        }
    }

    if (current_state != STATE_GAME_OVER) {
        uint8_t display_player_w = PLAYER_WIDTH;
        uint8_t display_player_h = PLAYER_HEIGHT;
        if (StateIsGrabbing()) {
            if (time_since_last_state_change < 0.1f) { display_player_w = PLAYER_WIDTH - 1; display_player_h = PLAYER_HEIGHT + 1; }
        }
        if (StateIsGrabbing() && is_grounded && time_since_grounded <= 0.1f && f_camera_bottom_y == 0.0f) {
            display_player_h = PLAYER_HEIGHT - 1;
        }
        if (display_player_w == 0) display_player_w = 1;
        if (display_player_h == 0) display_player_h = 1;

        int logical_player_bottom_y_on_screen = player_rect[1] - camera_bottom_y_int;

        game_draw_filled_rectangle(player_rect[0] - 1, logical_player_bottom_y_on_screen - 1, display_player_w + 2, display_player_h + 2, COLOR_BLACK_IDX);
        game_draw_filled_rectangle(player_rect[0], logical_player_bottom_y_on_screen, display_player_w, display_player_h, COLOR_WHITE_IDX);

        bool looking_right = current_state == STATE_TITLE_SCREEN || current_state == STATE_GRABBING_LEFT || StateIsJumpingRight();
        int vertical_eye_offset = 0;
        if (player_vel[1] < -25.0f) vertical_eye_offset = -2;
        else if (player_vel[1] < -10.0f) vertical_eye_offset = -1;
        
        if (current_state == STATE_TITLE_SCREEN || (player_vel[0] == 0.0f && player_vel[1] == 0.0f && time_since_last_state_change > 2.0f && StateIsGrabbing()) ){
            float f_val = 0.23f * time_since_game_start; 
            if (f_val - floorf(f_val) < 0.23f) vertical_eye_offset = -1;
        }

        float time_since_lb_start = time_since_game_start - last_blink_start;
        if (time_since_lb_start > 2.5f) {
            last_blink_start = time_since_game_start;
            time_since_lb_start = 0.0f;
        }
        
        bool is_blinking_eyes_white; 
        if (current_state == STATE_TITLE_SCREEN || StateIsGrabbing()) {
            is_blinking_eyes_white = (time_since_lb_start <= 0.2f); 
        } else { 
            is_blinking_eyes_white = false; 
        }
        uint8_t eye_pixel_color = is_blinking_eyes_white ? COLOR_WHITE_IDX : COLOR_BLACK_IDX;

        if (looking_right) {
            game_draw_pixel(player_rect[0] + PLAYER_WIDTH - 4, logical_player_bottom_y_on_screen + PLAYER_HEIGHT - 2 + vertical_eye_offset, eye_pixel_color);
            game_draw_pixel(player_rect[0] + PLAYER_WIDTH - 1, logical_player_bottom_y_on_screen + PLAYER_HEIGHT - 2 + vertical_eye_offset, eye_pixel_color);
        } else {
            game_draw_pixel(player_rect[0] + 0, logical_player_bottom_y_on_screen + PLAYER_HEIGHT - 2 + vertical_eye_offset, eye_pixel_color);
            game_draw_pixel(player_rect[0] + 3, logical_player_bottom_y_on_screen + PLAYER_HEIGHT - 2 + vertical_eye_offset, eye_pixel_color);
        }
        
        if (StateIsJumpingLeft()) {
            game_draw_pixel(player_rect[0] + 1,               logical_player_bottom_y_on_screen - 1                     , COLOR_WHITE_IDX);
            game_draw_pixel(player_rect[0] + PLAYER_WIDTH,    logical_player_bottom_y_on_screen + PLAYER_HEIGHT - 2 , COLOR_WHITE_IDX);
            game_draw_pixel(player_rect[0] + PLAYER_WIDTH,    logical_player_bottom_y_on_screen - 1                     , COLOR_WHITE_IDX);
        } else if (StateIsJumpingRight()) {
            game_draw_pixel(player_rect[0] - 1,                  logical_player_bottom_y_on_screen + PLAYER_HEIGHT - 2 , COLOR_WHITE_IDX);
            game_draw_pixel(player_rect[0] - 1,                  logical_player_bottom_y_on_screen - 1                       , COLOR_WHITE_IDX);
            game_draw_pixel(player_rect[0] + PLAYER_WIDTH - 2, logical_player_bottom_y_on_screen - 1                       , COLOR_WHITE_IDX);
        }
    }

    if (camera_bottom_y_int == 0) { 
        game_draw_filled_rectangle(0, 0, SCREEN_WIDTH, 2, COLOR_WHITE_IDX);
    }
    
    int wall_logical_h = all_logical_sprite_sizes[SPRITE_ID_WALL_LEFT].height;
    for (int ly_wall_loop_start = (-camera_bottom_y_int % wall_logical_h) - wall_logical_h; ly_wall_loop_start < SCREEN_HEIGHT; ly_wall_loop_start += wall_logical_h) {
        int ly_to_draw_wall = ly_wall_loop_start + 2; 
        if ((ly_to_draw_wall + wall_logical_h > -DRAW_PRE_CULL_MARGIN) && (ly_to_draw_wall < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN)) { // Adjusted culling
            game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_WALL_LEFT], wall_logical_h, 0, ly_to_draw_wall, false, 0); 
            game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_WALL_RIGHT], wall_logical_h, SCREEN_WIDTH - WALL_WIDTH, ly_to_draw_wall, false, 0);
        }
    }

    uint8_t spike_log_h = all_logical_sprite_sizes[SPRITE_ID_SPIKE_LEFT].height;
    uint8_t spike_log_w = all_logical_sprite_sizes[SPRITE_ID_SPIKE_LEFT].width;

    for (uint8_t k_idx = 0; k_idx < num_hazards_left; ++k_idx) {
        int hazard_base_y_logical_on_screen = hazards_left[k_idx][0] - camera_bottom_y_int;
        int hazard_spike_count = hazards_left[k_idx][1];
        if (hazard_base_y_logical_on_screen + (4 * hazard_spike_count) >= -DRAW_PRE_CULL_MARGIN && hazard_base_y_logical_on_screen < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN) { // Adjusted
            game_draw_filled_rectangle(1, hazard_base_y_logical_on_screen, 2, 4 * hazard_spike_count + 1, COLOR_WHITE_IDX); 
            for (int s_idx = 0; s_idx < hazard_spike_count; ++s_idx) {
                int spike_y_pos_logical_on_screen = hazard_base_y_logical_on_screen + s_idx * 4 + 1;
                 if (spike_y_pos_logical_on_screen + spike_log_h >= -DRAW_PRE_CULL_MARGIN && spike_y_pos_logical_on_screen < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN){ // Adjusted
                    game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_SPIKE_LEFT], spike_log_h, WALL_WIDTH, spike_y_pos_logical_on_screen, true, COLOR_BLACK_IDX); 
                 }
            }
        }
    }
    for (uint8_t k_idx = 0; k_idx < num_hazards_right; ++k_idx) {
        int hazard_base_y_logical_on_screen = hazards_right[k_idx][0] - camera_bottom_y_int;
        int hazard_spike_count = hazards_right[k_idx][1];
         if (hazard_base_y_logical_on_screen + (4 * hazard_spike_count) >= -DRAW_PRE_CULL_MARGIN && hazard_base_y_logical_on_screen < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN) { // Adjusted
            game_draw_filled_rectangle(SCREEN_WIDTH - 3, hazard_base_y_logical_on_screen, 2, 4 * hazard_spike_count, COLOR_WHITE_IDX); 
            for (int s_idx = 0; s_idx < hazard_spike_count; ++s_idx) {
                int spike_y_pos_logical_on_screen = hazard_base_y_logical_on_screen + s_idx * 4 + 1;
                if(spike_y_pos_logical_on_screen + spike_log_h >= -DRAW_PRE_CULL_MARGIN && spike_y_pos_logical_on_screen < SCREEN_HEIGHT + DRAW_PRE_CULL_MARGIN){ // Adjusted
                    game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_SPIKE_RIGHT], spike_log_h, SCREEN_WIDTH - WALL_WIDTH - spike_log_w, spike_y_pos_logical_on_screen, true, COLOR_BLACK_IDX); 
                }
            }
        }
    }

    if (DRAW_PARTICLES) {
        for (uint8_t k_idx = 0; k_idx < num_particles; ++k_idx) {
            Particle *p_ptr = &particles[k_idx];
            bool show_particle = true;
            if (p_ptr->time_left < p_ptr->blink_time && p_ptr->blink_rate > 0.0f) { 
                uint8_t stage = (uint8_t)floorf(p_ptr->time_left / p_ptr->blink_rate);
                show_particle = (stage % 2 == 0);
            }
            if (show_particle) {
                for (int x_off = 0; x_off < p_ptr->size[0]; ++x_off) {
                    for (int y_off = 0; y_off < p_ptr->size[1]; ++y_off) {
                        int particle_draw_lx = (int)roundf(p_ptr->pos[0]) + x_off;
                        int particle_draw_ly = (int)roundf(p_ptr->pos[1]) - y_off - camera_bottom_y_int;
                        // Particle culling can be simpler: just check if the point is within logical screen
                        if (particle_draw_lx >=0 && particle_draw_lx < SCREEN_WIDTH && particle_draw_ly >=0 && particle_draw_ly < SCREEN_HEIGHT) {
                             game_draw_pixel(particle_draw_lx, particle_draw_ly, COLOR_WHITE_IDX);
                        }
                    }
                }
            }
        }
    }

    if (current_state == STATE_TITLE_SCREEN) { 
        game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_TITLE], all_logical_sprite_sizes[SPRITE_ID_TITLE].height, 0, 64, false, 0);
    } else if (current_state == STATE_GAME_OVER) { 
        game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_SCORE], all_logical_sprite_sizes[SPRITE_ID_SCORE].height, 0, 66, false, 0); 
        float num_y_pos = 2.0f * sinf(5.0f * time_since_last_state_change) + 56.0f; 
        game_draw_number_centered(last_score, (int)roundf(num_y_pos), false);
        if (high_score_appvar > 0) { 
            game_draw_sprite(gfx_sprite_pointers[SPRITE_ID_BEST], all_logical_sprite_sizes[SPRITE_ID_BEST].height, 6, 30, false, 0);
            game_draw_number_centered(high_score_appvar, 24, true); 
        }
    }
    
    if(DEBUG_DRAW_FPS){ 
        // Placeholder for FPS drawing if you re-implement it
    }
}