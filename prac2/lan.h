#ifndef LAN_H
#define LAN_H

#define MAX 3
//definimos nuestro nodo de origen //
#include <pbn.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>


typedef enum {esperant, pendent_enviar} state;
extern state estat;
extern block_morse rx_block_l;
typedef void (*lan_callback_t)(void);
void lan_init(uint8_t no);

bool lan_can_put(void);
void lan_block_put(const block_morse b, uint8_t nd);

uint8_t lan_block_get(block_morse b);
void on_lan_received(lan_callback_t l);

#endif
