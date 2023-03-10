#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"

/**
 * @brief Subscribes and enables Mouse interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Mouse interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_unsubscribe_int)();

int(mouse_toggle_data)(uint8_t cmd);
