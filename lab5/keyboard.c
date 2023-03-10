#include "keyboard.h"
#define DELAY_US 20000

uint8_t scanCode; //scan_code

int hook_id = 0x01;

int (keyboard_subscribe_int)(uint8_t *bit_no) {
	*bit_no = hook_id;
	if(sys_irqsetpolicy(1, IRQ_EXCLUSIVE |IRQ_REENABLE, &hook_id) == 0)
		return 0;

	return 1;
}

int (keyboard_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id) == 0) return 0;

  return 1;
}

void (kbd_ih)(){
	scanCode = read_from_kbc();
}

extern uint32_t cnt;

uint8_t (read_from_kbc)(){
	uint8_t stat, data;
	while( 1 ) {
		util_sys_inb(KBC_ST_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if( stat & OBF ) {
			util_sys_inb(KBC_OUT_BUF, &data); /* ass. it returns OK */
			if ( (stat &(PAR_ERROR | TIME_ERROR)) == 0 )
				return data;
			else
				return 0;
			}
		tickdelay(micros_to_ticks(DELAY_US));
	}
}

int (issue_command)(uint8_t cmd){
	uint8_t stat;
	while( 1 ) {
		util_sys_inb(KBC_ST_REG, &stat);
		 /* assuming it returns OK */
		/* loop while 8042 input buffer is not empty */
		if( (stat & IBF) == 0 ) {
			sys_outb(KBC_CMD_REG, cmd); /* no args command */
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
		}

}

