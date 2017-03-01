#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "avr_checksum.h"
#include "frame.h"
uint8_t t[32];
block_morse taula=t;

static void print_msg(void);

int main(void){
  serial_open();
  frame_init();
  on_frame_received(print_msg);
  sei();
  while(1);
}

void print_msg(void){
  for(int i=0;i<32;i++)
    t[i]='\0';
  frame_block_get(taula);
  print(taula);
}
