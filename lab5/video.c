// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "video.h"


vbe_mode_info_t vmi_p;
static char *video_mem;
static unsigned memory_model;
static unsigned h_res;
static unsigned v_res;
static unsigned red_pos, green_pos, blue_pos;
static unsigned red_size, green_size, blue_size;
static unsigned bits_per_pixel, bytes_per_pixel;

extern int video_mode;

int (set_mode)(uint16_t mode){

    reg86_t reg;
    memset(&reg,0,sizeof(reg86_t));

    reg.intno = 0x10;
    reg.ax = 0x4F02;
    reg.bx = 1 << 14 | mode;

    if(sys_int86(&reg) != 0) return 1;

    return 0;
}

void *(vg_init)(uint16_t mode){
    
    struct minix_mem_range mr;
    unsigned int vram_base;
    unsigned int vram_size;

    int r;

    my_vbe_get_mode_info(mode,&vmi_p);
    vram_base = vmi_p.PhysBasePtr;
    memory_model = vmi_p.MemoryModel;

    v_res = vmi_p.YResolution;
    h_res = vmi_p.XResolution;

    red_pos = vmi_p.RedFieldPosition; red_size = vmi_p.RedMaskSize;
    green_pos = vmi_p.GreenFieldPosition; green_size = vmi_p.GreenMaskSize;
    blue_pos = vmi_p.BlueFieldPosition; blue_size = vmi_p.BlueMaskSize;

    bits_per_pixel = vmi_p.BitsPerPixel;
    bytes_per_pixel = (bits_per_pixel + ((8 - bits_per_pixel) % 8)) / 8;

    vram_size = bytes_per_pixel * v_res * h_res;

    mr.mr_base = (phys_bytes) vram_base;	
    mr.mr_limit = mr.mr_base + vram_size;  

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED)
        panic("couldn't map video memory");

    reg86_t reg86;
    memset(&reg86,0,sizeof(reg86_t));

    reg86.ax = 0x4F02;
    reg86.intno = 0x10;
    reg86.bx = 1 << 14 | mode;

    if(sys_int86(&reg86) != 0) return NULL;
    return video_mem;


}

int (my_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t *vmi_p){

    mmap_t map;
    if(lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) return 1;

    reg86_t r;
    memset(&r, 0, sizeof(reg86_t));

    r.intno = 0x10;
    r.ax = 0x4F01;
    r.es = PB2BASE(map.phys);
    r.di = PB2OFF(map.phys);
    r.cx = mode;

    if(sys_int86(&r) != 0) return 1;

    *vmi_p = *((vbe_mode_info_t*) map.virt);

    if(!lm_free(&map)) return 1;

    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){


    // if(video_mode == 0x110 || video_mode == 0x11A){

    //     uint32_t red = 0, green = 0, blue = 0;
    //     if(video_mode == 0x110){
    //         red = (color & 0x00FF0000) >> 19;
    //         green = (color & 0x00000FF00) >> 11;
    //         blue = (color & 0x000000FF) >> 3;

    //         color = (red << 10)  | (green << 5) | (blue);
    //     }
    //     else if(video_mode == 0x11A){
    //         red = (color & 0x00FF0000) >> 19;
    //         green = (color & 0x0000FF00) >> 10;
    //         blue = (color & 0x000000FF) >> 3;

    //         color = (red << 11)  | (green << 5) | (blue);
    //     }
        
    // }



    if(video_mode == 0x110 || video_mode == 0x11A){
        uint32_t red = 0, green = 0, blue = 0;
        if(video_mode == 0x110){
            red = (color & 0x00FF0000) >> 19;
            green = (color & 0x0000FF00) >> 11;
            blue = (color & 0x000000FF) >> 3;

            color = (red << 10) | (green << 5) | (blue);
        }
        else if(video_mode == 0x11A){
            red = (color & 0x00FF0000) >> 19;
            green = (color & 0x0000FF00) >> 10;
            blue = (color & 0x000000FF) >> 3;

            color = (red << 11) | (green << 5) | (blue);
        }
    }


    for(int i = 0; i < height; i++){
        if(vg_draw_hline(x,y + i, width, color) != 0) return 1;
    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

    for(int i = 0; i < len; i++){
        if(vg_draw_pixel(x + i, y, color) != 0) return 1;
    }
    return 0;
}


int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){

    if(x > vmi_p.XResolution || y > vmi_p.YResolution) return 1;
    memcpy(video_mem + (vmi_p.XResolution * y + x) * bytes_per_pixel , &color, bytes_per_pixel);

    return 0;
}

void (get_rgb_first)(uint32_t first, uint32_t* redFirst, uint32_t* greenFirst, uint32_t* blueFirst){

    *redFirst = (first << 8) >> 24;
    *greenFirst = (first << 16) >> 24;
    *blueFirst = (first << 24) >> 24;

}

int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step){
    uint16_t width = h_res / no_rectangles;
    uint16_t height = v_res / no_rectangles;
    uint32_t color = first;
    
    uint32_t redFirst = 0;
    uint32_t blueFirst = 0;
    uint32_t greenFirst = 0;

    uint16_t x = 0;
    uint16_t y = 0;

    get_rgb_first(first, &redFirst, &greenFirst, &blueFirst);

    for(int row = 0; row < no_rectangles; row++){
        for(int col = 0; col < no_rectangles; col++){
            if(memory_model == INDEXED_MODE){
                color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
            }
            else if (memory_model == DIRECT_MODE){
                uint32_t red = (redFirst + col * step) % (1 << red_size);
                uint32_t green = (greenFirst + row * step) % (1 << green_size);
                uint32_t blue = (blueFirst + (col + row) * step) % ( 1 << blue_size);
                color = (red << red_pos) | (green << green_pos) | (blue << blue_pos);
                
            }
            printf("%x\n",color);
            if(vg_draw_rectangle(x,y,width,height,color) != 0) return 1;
            x += width;
        }
        x = 0;
        y += height;
    }

    return 0;

}

int (vg_draw_sprite)(uint8_t *map, uint16_t x, uint16_t y, xpm_image_t img){
    for(int i = 0; i < img.height; i++){
        for(int j = 0; j < img.width; j++){
            vg_draw_pixel(x + j, y + i,*(map));
            map++;
        }
    }

    return 0;
}

int (vg_destroy_sprite)(uint8_t *map, uint16_t x, uint16_t y, xpm_image_t img){

    for(int i = 0; i < img.height; i++){
        for(int j = 0; j < img.width; j++){
            vg_draw_pixel(x + j, y + i,0);
            map++;
        }
    }

    return 0;
}

int (vg_draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y, enum xpm_image_type type){
    

    xpm_image_t img;
    uint8_t *map;
    map = xpm_load(xpm,type,&img);

    for(int i = 0; i < img.height; i++){
        for(int j = 0; j < img.width; j++){
            vg_draw_pixel(x + j, y + i,*(map));
            map++;
        }
    }

    return 0;

}

