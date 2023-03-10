#include <lcom/lcf.h>

enum 13223_time_units{
  13223_microsecond,
  13223_millisecond,
  13223_second
};

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

uint8_t hook_id;

int timerpp_subscribe_int(int *bit_no){
  hook_id = bit_no;
  if(sys_irqsetpolicy(10,IRQ_REENABLE, hook_id )== 0)
    return 0;
  return 1;
}

int timerpp_unsubscribe_int(){
  if(sys_irqrmpolicy(hook_id) == 0){
    return 0;
  }
  return 1;
}

int util_sys_inb(int port, uint_8 *st){
  uint32_t v = 0;
  sys_inb(port, v);
  *st = (uint8_t)v;
  return 1;
}

int pp_test_alarm(int timer, int interval, enum 13223_time_units unit){
    uint8_t TIMER_CTRL = 0x23;
    uint8_t timerAdress = 0x20 + timer;
    uint8_t timerSelect = timerAdress << 6;
    uint8_t word;
    switch (unit)
    {
    case 13223_microsecond: /* constant-expression */
      /* code */
      word = timerSelect | BIT(4);
      break;
    case 13223_millisecond:
      word = timerSelect | BIT(4) | BIT(0);
      break;
    case 13223_second:
      word = timerSelect | BIT(4) | BIT(1);
      break;
    default:
      break;
    }
    sys_outb(TIMER_CTRL, word);
    int counter = 0;
    int ipc_status;
    message msg;
    int r;
    uint8_t bit_no = 0x0A; //10 em hexadecimal
    timerpp_subscribe_int(bit_no);
    uint8_t intervalLSB = interval;
    uint8_t intervalMSB = interval << 8;
    sys_outb(timerAdress, intervalLSB);
    sys_outb(timerAdress, intervalMSB);
    while (counter < interval) {
    // Get a request message.
      if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
        printf("driver_receive failed with: %d", r);
        continue;
      }

      if (is_ipc_notify(ipc_status)) { // received notification
        switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE:                             // hardware interrupt notification
            if (msg.m_notify.interrupts & BIT(10) ) { // subscribed interrupt
              uint8_t interrupt;
              util_sys_inb(TIMER_CTRL, &interrupt);
              if(interrupt & BIT(timer)){
                counter += 1;     
              }
                                            // handle interrupt
            }
            break;
          default:
            break;
        }
      }
  }
  pp_print_alarm(timer, interval, unit);
  timerpp_unsubscribe_int();
}

