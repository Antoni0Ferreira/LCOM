#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

unsigned int counter;
int hook_idT;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if(timer < 0 || timer > 2){
    return 1;
  }
  uint8_t st;
  timer_get_conf(timer, &st);
  uint8_t lsb, msb;
  util_get_LSB(TIMER_FREQ/freq, &lsb);
  util_get_MSB(TIMER_FREQ/freq >> 8, &msb);
  uint8_t timerSel = timer << 6;
  uint8_t aux = st & 0x0F;
  aux = aux | TIMER_LSB_MSB | timerSel;
  sys_outb(TIMER_CTRL, aux);
  sys_outb(TIMER_0 + timer, lsb);
  sys_outb(TIMER_0 + timer, msb);
  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
	hook_idT = *bit_no;
	if(sys_irqsetpolicy(0, IRQ_REENABLE, &hook_idT) == 0)
		return 0;

	return 1;
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_idT) == 0) return 0;

  return 1;
}

void (timer_int_handler)() {
	counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t t = TIMER_0 + timer;
  uint8_t word = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  sys_outb(TIMER_CTRL, word);
  util_sys_inb(t, st);
  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val val;
  uint8_t aux;
  switch (field){
    case tsf_all:
      val.byte = st;
      break;

    case tsf_initial:
      aux = st & 0x30;
      aux = aux >> 4;
      if(aux == 0x01){
          val.in_mode = LSB_only;
      }
      else if(aux == 0x02){
        val.in_mode = MSB_only;
      }
      else if(aux == 0x03){
        val.in_mode = MSB_after_LSB;
      }
      else{
        val.in_mode = INVAL_val;
      }
      break;

    case tsf_mode:
      aux = st & 0x0E;
      aux = aux >> 1;
      if(aux == 0x06){
        val.count_mode = 0x02;
      }
      else if(aux == 0x07){
        val.count_mode = 0x03;
      }
      else{
        val.count_mode = aux;
      }
      break;

    case tsf_base:
      if((st & 0x01) == 0x01){
        val.bcd = true;
      }
      else{
        val.bcd = false;
      }
      break;

  }
  timer_print_config(timer, field, val);

  return 1;
}
