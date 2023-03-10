// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#define INDEXED_MODE 0x04
#define DIRECT_MODE 0x06

void *(vg_init)(uint16_t mode);
int (set_mode)(uint16_t mode);
int wait_for_esc();
int (my_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t *vmi_p);
int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step);
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y, enum xpm_image_type type);
int (vg_draw_sprite)(uint8_t *map, uint16_t x, uint16_t y, xpm_image_t img);
int (vg_destroy_sprite)(uint8_t *map, uint16_t x, uint16_t y, xpm_image_t img);
void (get_rgb_first)(uint32_t first, uint32_t* red, uint32_t* green, uint32_t* blue);

