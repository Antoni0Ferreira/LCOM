#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>
#include "i8042.h"

/**
 * @brief Subscribes and enables Keyboard interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Timer 0 interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbd_unsubscribe_int)();

/**
 * @brief Keyboard interrupt handler
 *
 * 
 */
void(kbd_ih)();

uint8_t(read_from_kbc)();

int(issue_command)(uint8_t cmd);

#endif
