#include <lcom/lcf.h>

#include "lab3.h"

#include <stdbool.h>
#include <stdint.h>

uint8_t scan_code;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
uint32_t cnt;

int(kbd_test_scan)() {
    uint8_t no = 0x01;
    kbd_subscribe_int(&no);
    int ipc_status;
    message msg;
    int r;

	uint8_t* bytes = malloc(2 * sizeof(char));
	int count = 1;
	cnt = 0;
    while( scan_code != 0x81 /*break code do ESC*/ ) { /* You may want to use a different condition */
    /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */				
					if (msg.m_notify.interrupts & BIT(1)) { /* subscribed interrupt */
						kbd_ih();
						uint8_t make = !(scan_code & BIT(7));
						if(scan_code == 0xE0){
							count += 1;
							bytes[0] = scan_code;
						}
						else{
							if(count == 2){
								bytes[1] = scan_code;
							}
							else{
								bytes[0] = scan_code;
							}
							kbd_print_scancode(make, count, bytes);
							count = 1;
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
	kbd_print_no_sysinb(cnt);
    kbd_unsubscribe_int();
    return 1;
}

int(kbd_test_poll)() {
	
	uint8_t stat, data;
	uint8_t* bytes = malloc(2 * sizeof(char));
	int count = 1;
	while(scan_code != 0X81){
		util_sys_inb(KBC_ST_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if( (stat & OBF) /* && !(stat & AUX)*/) {
			util_sys_inb(KBC_OUT_BUF, &data); /* ass. it returns OK */
			if ( (stat &(PAR_ERROR | TIME_ERROR)) == 0 ){
				scan_code = data;
				uint8_t make = !(scan_code & BIT(7));
				if(scan_code == 0xE0){
					count += 1;
					bytes[0] = scan_code;
				}
				else{
					if(count == 2){
						bytes[1] = scan_code;
					}
					else{
						bytes[0] = scan_code;
					}
					kbd_print_scancode(make, count, bytes);
					count = 1;
				}
			}
		}
	}

	uint8_t command;
	issue_command(KBC_RCB);
	util_sys_inb(KBC_OUT_BUF, &command);

	issue_command(KBC_WCB);
	command = command | KBD_INT; 
	command = command & ~(DIS_KBD_INTERFACE | DIS_MOUSE_INTERFACE);
	sys_outb(0X60, command);

	kbd_print_no_sysinb(cnt);
	return 1;
}

extern unsigned int counter;

int(kbd_test_timed_scan)(uint8_t n) {
  	uint8_t no = 0x01;
    kbd_subscribe_int(&no);
	uint8_t no2 = 0x00;
    timer_subscribe_int(&no2);
    int ipc_status;
    message msg;
    int r;
	counter = 0;
	uint8_t* bytes = malloc(2 * sizeof(char));
	int count = 1;
	cnt = 0;
    while( scan_code != 0x81 /*break code do ESC*/  && counter < n * 60 ) { /* You may want to use a different condition */
    /* Get a request message. */
		if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */				
					if (msg.m_notify.interrupts & BIT(1)) { /* subscribed interrupt */
						kbd_ih();
						uint8_t make = !(scan_code & BIT(7));
						if(scan_code == 0xE0){
							count += 1;
							bytes[0] = scan_code;
						}
						else{
							if(count == 2){
								bytes[1] = scan_code;
							}
							else{
								bytes[0] = scan_code;
							}
							kbd_print_scancode(make, count, bytes);
							count = 1;
						}
						counter = 0;
						
					}
					else if(msg.m_notify.interrupts & BIT(0)){
						timer_int_handler();
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */	
				}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
  }
	timer_unsubscribe_int();
    kbd_unsubscribe_int();
    return 1;
}
