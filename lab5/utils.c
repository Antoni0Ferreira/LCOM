#include <lcom/lcf.h>

#include <stdint.h>

extern uint32_t cnt;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) val;
  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t) val;
  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t v = 0;
  sys_inb(port, &v);
  *value = (uint8_t) v;
  #ifdef LAB3
  cnt+=1;
  #endif
  return 1;
}
