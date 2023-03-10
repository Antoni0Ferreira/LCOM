#include "mouse.h"
#define DELAY_US    20000

extern uint8_t packet_byte;

int hook_id = 12;
extern uint8_t packet_data;
extern int num_bytes;
extern bool valid;

int (mouse_subscribe_int)(uint8_t *bit_no) {
	*bit_no = hook_id;
	if(sys_irqsetpolicy(12, IRQ_EXCLUSIVE |IRQ_REENABLE, &hook_id) == 0)
		return 0;
        
	return 1;
}

int (mouse_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id) == 0) return 0;

}


uint8_t (mouse_read_data)(uint8_t *packet_data){

    // uint8_t stat;

    // while(1){
    //     util_sys_inb(KBC_ST_REG,&stat);
    //     if(stat & OBF){
    //         util_sys_inb(KBC_OUT_BUF,packet_data);
    //         if((stat & (PAR_ERROR | TIME_ERROR)) != 0){
    //             return 1;
    //         }
    //         return 0;
    //     }
    //     tickdelay(micros_to_ticks(DELAY_US));
    // }
    // return 1;

    uint8_t status;
    while(1){
        util_sys_inb(0x64,&status);
        if(status & OBF){
            util_sys_inb(0x60,packet_data);
            if((status & (PAR_ERROR | TIME_ERROR)) == 0) return 0;
            return 0;
        }
        tickdelay(micros_to_ticks(20000));
    }
    return 1;
}

void (mouse_ih)(){

    // if(mouse_read_data(&packet_data) != 0){
    //     valid = false;
    //     return;
    // }
    // if(num_bytes == 0){
    //     if((packet_data & BIT(3)) == 0){
    //         valid = false;
    //         return;
    //     }
    // }

    // num_bytes++;
    // valid = true;

    if(mouse_read_data(&packet_data) != 0){
        valid = false;
        return;
    }
    if(num_bytes == 0){
        if((packet_data & BIT(3)) == 0){
            valid = false;
            return;
        }
    }
    valid = true;
    num_bytes++;

    ///////

    // valid = true;

    // for(int i = 0; i < 3; i++){
    //     if(i == 0){
    //         if(mouse_read_data(&packet_1) != 0){
    //             valid = false;
    //             return;
    //         }
    //         else if{
    //             ((packet_1 & BIT(3)) == 0){
    //                 valid = false;
    //                 return;
    //             }
    //         }
    //         else{
    //             bytes[i] = packet_1;
    //             num_bytes++;
    //         }
    //     }
    //     else if(i == 1){
    //         if(mouse_read_data(&packet_2) != 0){
    //             valid = false;
    //             return;
    //         }
    //         if(packet_1 & BIT(0)){
    //             valid2 = true;
    //             bytes[i] = packet_2;
    //             num_bytes++;
    //         }
    //     }
    //     else if(i == 2){
    //         if(mouse_read_data(&packet_3) != 0){
    //             valid = false;
    //             return;
    //         }
    //         if(packet_1 & BIT(1)){
    //             valid3 = true;
    //             bytes[i] = packet_3;
    //             num_bytes++;
    //         }
    //     }
    // }


}

void (print_packet_data)(uint8_t *bytes,struct packet *pp){

    pp->bytes[0] = bytes[0];
    pp->bytes[1] = bytes[1];
    pp->bytes[2] = bytes[2];

    pp->x_ov = (pp->bytes[0] & BIT(6)) >> 6;
    pp->y_ov = (pp->bytes[0] & BIT(7)) >> 7;

    pp->lb = pp->bytes[0] & BIT(0);
    pp->mb = (pp->bytes[0] & BIT(2)) >> 2;
    pp->rb = (pp->bytes[0] & BIT(1)) >> 1;

    int16_t XDelta = pp->bytes[1];
    int16_t YDelta = pp->bytes[2];

    if((pp->bytes[0] & BIT(4)) != 0) XDelta = XDelta - 256;
    if((pp->bytes[0] & BIT(5)) != 0) YDelta = YDelta - 256;

    pp->delta_x = XDelta;
    pp->delta_y = YDelta;
    
    mouse_print_packet(pp);

}

uint8_t (mouse_issue_command)(uint8_t cmd){
    uint8_t status;
    uint8_t acknow;

    while(1){
        util_sys_inb(KBC_ST_REG,&status);
        if((status & IBF) == 0){
            if(sys_outb(KBC_CMD_REG,MOUSE_COMMAND) != 0) return 1;
            break;
        }
    }
    
    while(1){
        util_sys_inb(KBC_ST_REG,&status);
        if((status & IBF) == 0) {
            if(sys_outb(KBC_ARG_REG,cmd) != 0) return 1;

            util_sys_inb(KBC_OUT_BUF,&acknow);
            
            if(acknow == 0xFA) return 0;
            else if (acknow == 0xFE){
                tickdelay(micros_to_ticks(DELAY_US));
            }
            else if(acknow == 0xFC) return 1;

            else tickdelay(micros_to_ticks(DELAY_US));
        }
    }
    return 1;

}

uint8_t (mouse_disable_data)(){
    // if(mouse_issue_command(MOUSE_DISABLE_DATA) != 0) return 1;    
    // return 0;
    if(mouse_issue_command(0xF5) != 0) return 1;
    return 0;
}

uint8_t (mouse_enable_data)(){
    // if(mouse_issue_command(MOUSE_ENABLE_DATA) != 0) return 1;
    // return 0;
    if(mouse_issue_command(0xF4) != 0) return 1;
    return 0;


}

