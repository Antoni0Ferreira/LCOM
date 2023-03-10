// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "video.h"
#include "i8042.h"
#include "keyboard.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

uint8_t scanCode;
uint16_t video_mode = 0x0000;

int(video_test_init)(uint16_t mode, uint8_t delay) {
//   if(set_mode(mode) != 0) return 1;
//   tickdelay(micros_to_ticks(delay * 1e6));

    if(set_mode(mode) != 0) return 1;
    tickdelay(micros_to_ticks(delay * 1e6));

  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {

    vg_init(mode);
    // memset(mem,3,70000);


    uint8_t bit_keyboard_no;

    if(keyboard_subscribe_int(&bit_keyboard_no) != 0) return 1;

    if(vg_draw_rectangle(x,y,width,height,color) != 0) return 1;
    uint8_t irq_set = BIT(bit_keyboard_no);
    message msg;
    int r, ipc_status;

    while(scanCode != ESC){
        if((r = driver_receive(ANY,&msg,&ipc_status)) != 0){
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if(is_ipc_notify(ipc_status)){
            switch(_ENDPOINT_P(msg.m_source)){
                case HARDWARE:
                    if(msg.m_notify.interrupts & irq_set){
                        kbc_ih();
                    }
                    break;

                default: break;
            }
        }
    }

    if(keyboard_unsubscribe_int() != 0) return 1;

    if(vg_exit() != 0) return 1;

    return 0;


}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
    vg_init(mode);

    uint8_t bit_keyboard_no;

    if(keyboard_subscribe_int(&bit_keyboard_no) != 0) return 1;


    if(vg_draw_pattern(no_rectangles, first, step) != 0) return 1;


    uint8_t irq_set = BIT(bit_keyboard_no);
    message msg;
    int r, ipc_status;

    while(scanCode != ESC){
        if((r = driver_receive(ANY,&msg,&ipc_status)) != 0){
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if(is_ipc_notify(ipc_status)){
            switch(_ENDPOINT_P(msg.m_source)){
                case HARDWARE:
                    if(msg.m_notify.interrupts & irq_set){
                        kbc_ih();
                    }
                    break;

                default: break;
            }
        }
    }

    if(keyboard_unsubscribe_int() != 0) return 1;

    if(vg_exit() != 0) return 1;

    return 0;


}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {


    //vg_init(0x105);
    vg_init(0x105);

    uint8_t bit_keyboard_no;

    if(keyboard_subscribe_int(&bit_keyboard_no) != 0) return 1;
  
    //if(vg_draw_xpm(xpm,x,y,XPM_INDEXED) != 0) return 1;
    if(vg_draw_xpm(xpm,x,y,XPM_INDEXED) != 0) return 1;

    uint8_t irq_set = BIT(bit_keyboard_no);
    message msg;
    int r, ipc_status;

    while(scanCode != ESC){
        if((r = driver_receive(ANY,&msg,&ipc_status)) != 0){
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if(is_ipc_notify(ipc_status)){
            switch(_ENDPOINT_P(msg.m_source)){
                case HARDWARE:
                    if(msg.m_notify.interrupts & irq_set){
                        kbc_ih();
                    }
                    break;

                default: break;
            }
        }
    }

    if(keyboard_unsubscribe_int() != 0) return 1;

    if(vg_exit() != 0) return 1;

    return 0;
}

extern int counter;

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

    vg_init(0x105);

    uint8_t bit_keyboard_no;
    uint8_t bit_timer_no;
    
    if(timer_set_frequency(0,fr_rate) != 0) return 1;

    if(keyboard_subscribe_int(&bit_keyboard_no) != 0) return 1;
    if(timer_subscribe_int(&bit_timer_no) != 0) return 1;
  
    xpm_image_t img;
    uint8_t *sprite = xpm_load(xpm,XPM_INDEXED,&img);

    uint8_t irq_set = BIT(bit_keyboard_no);
    uint8_t irq_timer_set = BIT(bit_timer_no);
    message msg;
    int r, ipc_status;

    uint16_t x = xi;
    uint16_t y = yi;

    int frames = 0;

    if(vg_draw_sprite(sprite,x,y,img) != 0) return 1;

    while(scanCode != ESC){
        if((r = driver_receive(ANY,&msg,&ipc_status)) != 0){
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if(is_ipc_notify(ipc_status)){
            switch(_ENDPOINT_P(msg.m_source)){
                case HARDWARE:
                    if(msg.m_notify.interrupts & irq_set){
                        kbc_ih();
                    }
                    if(msg.m_notify.interrupts & irq_timer_set){
                        timer_int_handler();
                        if(counter % (60 / fr_rate) != 0) continue;
                        if(x == xf && y == yf) continue;

                        if(speed >= 0){
                            vg_destroy_sprite(sprite,x,y,img);
                            if(xi == xf) y += speed; //andar apenas na vertical
                            else x += speed;

                            if(x > xf) x = xf; if(y > yf) y = yf;
                        }
                        else{
                            frames++;
                            if(frames >= -speed){
                                vg_destroy_sprite(sprite,x,y,img);
                                if(xi == xf) y++; //andar apenas na vertical
                                else x++;
                                frames = 0; 
                            }
                            else continue;
                        }
                        vg_draw_sprite(sprite,x,y,img);

                    }
                    break;

                default: break;
            }
        }
    }

    if(keyboard_unsubscribe_int() != 0) return 1;
    if(timer_unsubscribe_int() != 0) return 1;

    if(vg_exit() != 0) return 1;

    return 0;

}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
