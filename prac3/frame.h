#ifndef FRAME_H
#define FRAME_H

#include <inttypes.h>
#include <stdbool.h>
#include <pbn.h>
#include <stdint.h>

extern block_morse rx_block_f;

typedef void (*frame_callback_t)(void);

void frame_init(void);

bool frame_can_put(void);
void frame_block_put(const block_morse b);

void frame_block_get(block_morse b);

void on_frame_received(frame_callback_t l);

#endif
