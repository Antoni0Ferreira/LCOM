// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "mouse.h"
#include "i8042.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

uint8_t packet_byte;

int (mouse_test_packet)(uint32_t cnt) {
    uint8_t *p = malloc(3 * sizeof(uint8_t));

    //enable data reporting
    issue_command(0xD4);
    mouse_toggle_data(0xF4);
    //subscribe interrupts irq 12
    uint8_t bit_no;
    mouse_subscribe_int(&bit_no);
    
    //ciclo do driver receive
        //mouse_ih
        //mouse_print_packet
    int ipc_status;
    message msg;
    int r;
    int counter = 0;
    int byte_counter = 0;
    packet_byte = 0x00;
    while( counter < (int)cnt ) { 
    /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */				
					if (msg.m_notify.interrupts & BIT(12)) { /* subscribed interrupt */
						mouse_ih();
						//verificar se é do mesmo packet
                        if(byte_counter == 0 && (packet_byte & BIT(3)) == 0){
                            continue;
                        }
                        p[byte_counter] = packet_byte;
                        byte_counter++;
                        
                        if(byte_counter == 3){

                            struct packet pp;
                            pp.bytes[0] = p[0];
                            pp.bytes[1] = p[1];
                            pp.bytes[2] = p[2];
                            pp.mb = (p[0] & BIT(2)) >> 2;
                            pp.rb = (p[0] & BIT(1)) >> 1;
                            pp.lb = (p[0] & BIT(0));
                            pp.delta_x = (int16_t)p[1];
                            pp.delta_y = (int16_t)p[2];
                            if((p[0] & BIT(4)) != 0){
                                pp.delta_x -= 256;
                            }
                            if((p[0] & BIT(5)) != 0){
                                pp.delta_y -= 256;       
                            }
                            pp.x_ov = (p[0] & BIT(6)) >> 6;
                            pp.y_ov = (p[0] & BIT(7)) >> 7;
                            mouse_print_packet(&pp);
                            byte_counter = 0;
                            counter += 1;
                        }
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */	
				}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
  }

     //unsubscribe interrupts
    mouse_unsubscribe_int();
    //disable data reporting
    issue_command(0xD4);
    mouse_toggle_data(0xF5);
    
   
    return 1;
}

extern int counter;

int (mouse_test_async)(uint8_t idle_time) {
    uint8_t *p = malloc(3 * sizeof(uint8_t));

    //enable data reporting
    issue_command(0xD4);
    mouse_toggle_data(0xF4);
    //subscribe interrupts irq 12
    uint8_t bit_no;
    mouse_subscribe_int(&bit_no);
    uint8_t mouse_bit_no = 0x00;
    timer_subscribe_int(&mouse_bit_no);
    
    
    //ciclo do driver receive
        //mouse_ih
        //mouse_print_packet
    int ipc_status;
    message msg;
    int r;
    counter = 0;
    int byte_counter = 0;
    packet_byte = 0x00;
    while( counter < (int)idle_time * 60 ) { /* You may want to use a different condition */
    /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
                    if(msg.m_notify.interrupts & BIT(0)){
                        timer_int_handler();
                    }				
					if (msg.m_notify.interrupts & BIT(12)) { /* subscribed interrupt */
                        counter = 0;
						mouse_ih();
						//verificar se é do mesmo packet
                        if(byte_counter == 0 && (packet_byte & BIT(3)) == 0){
                            continue;
                        }
                        p[byte_counter] = packet_byte;
                        byte_counter++;
                        //printf("%x\n", packet_byte);
                        if(byte_counter == 3){

                            struct packet pp;
                            pp.bytes[0] = p[0];
                            pp.bytes[1] = p[1];
                            pp.bytes[2] = p[2];
                            pp.mb = (p[0] & BIT(2)) >> 2;
                            pp.rb = (p[0] & BIT(1)) >> 1;
                            pp.lb = (p[0] & BIT(0));
                            pp.delta_x = (int16_t)p[1];
                            pp.delta_y = (int16_t)p[2];
                            if((p[0] & BIT(4)) != 0){
                                pp.delta_x -= 256;
                            }
                            if((p[0] & BIT(5)) != 0){
                                pp.delta_y -= 256;       
                            }
                            pp.x_ov = (p[0] & BIT(6)) >> 6;
                            pp.y_ov = (p[0] & BIT(7)) >> 7;
                            mouse_print_packet(&pp);
                            byte_counter = 0;
                            
                        }
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */	
				}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
  }

     //unsubscribe interrupts
    mouse_unsubscribe_int();
    timer_unsubscribe_int();
    //disable data reporting
    issue_command(0xD4);
    mouse_toggle_data(0xF5);
    
   
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    uint8_t *p = malloc(3 * sizeof(uint8_t));

    //enable data reporting
    issue_command(0xD4);
    mouse_toggle_data(0xF4);
    //subscribe interrupts irq 12
    uint8_t bit_no;
    mouse_subscribe_int(&bit_no);
    
    //ciclo do driver receive
        //mouse_ih
        //mouse_print_packet
    int ipc_status;
    message msg;
    int r;
    int counter = 0;
    int byte_counter = 0;
    packet_byte = 0x00;
    int state = 0;
    bool done = false;
    while(!done ) { 
    /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */				
					if (msg.m_notify.interrupts & BIT(12)) { /* subscribed interrupt */
						mouse_ih();
						//verificar se é do mesmo packet
                        if(byte_counter == 0 && (packet_byte & BIT(3)) == 0){
                            continue;
                        }
                        p[byte_counter] = packet_byte;
                        byte_counter++;
                        
                        if(byte_counter == 3){

                            struct packet pp;
                            pp.bytes[0] = p[0];
                            pp.bytes[1] = p[1];
                            pp.bytes[2] = p[2];
                            pp.mb = (p[0] & BIT(2)) >> 2;
                            pp.rb = (p[0] & BIT(1)) >> 1;
                            pp.lb = (p[0] & BIT(0));
                            pp.delta_x = (int16_t)p[1];
                            pp.delta_y = (int16_t)p[2];
                            if((p[0] & BIT(4)) != 0){
                                pp.delta_x -= 256;
                            }
                            if((p[0] & BIT(5)) != 0){
                                pp.delta_y -= 256;       
                            }
                            pp.x_ov = (p[0] & BIT(6)) >> 6;
                            pp.y_ov = (p[0] & BIT(7)) >> 7;
                            //mouse_print_packet(&pp);
                            byte_counter = 0;
                            counter += 1;
                            int x_mov = 0, y_mov = 0;
                            switch (state)
                            {
                            case 0:
                                x_mov = 0; y_mov = 0;
                                if(pp.rb || pp.mb){
                                    break;
                                }
                                if(pp.lb){
                                    if(pp.delta_x < - tolerance ){
                                        break;
                                    }
                                    x_mov += pp.delta_x;
                                    y_mov += pp.delta_y;
                                    state = 1;
                                }
                                break;
                            case 1:
                                if(!pp.lb || pp.mb || (!pp.rb && x_mov < x_len)|| (double)pp.delta_y/pp.delta_x <= 1 ){
                                    x_mov = 0; y_mov = 0;
                                    state = 0;
                                    break;
                                }
                                if(pp.lb && pp.rb && !pp.mb && x_mov >= x_len){
                                    x_mov = 0; y_mov = 0;
                                    state = 2;
                                    break;
                                }
                                x_mov += pp.delta_x;
                                y_mov += pp.delta_y;
                                break;
                            case 2:
                                if(pp.lb || pp.mb || !pp.rb || (double)pp.delta_y/pp.delta_x >= -1 || pp.delta_x > - tolerance){
                                    x_mov = 0; y_mov = 0;
                                    state = 0;
                                    break;
                                }
                                if(x_mov >= x_len){
                                    done = true;
                                    break;
                                }
                                x_mov += pp.delta_x;
                                y_mov += pp.delta_y;
                                break;
    
                            default:
                                break;
                            }

                        }
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */	
				}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
  }

     //unsubscribe interrupts
    mouse_unsubscribe_int();
    //disable data reporting
    issue_command(0xD4);
    mouse_toggle_data(0xF5);
    
   
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
