#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "frame.h"

static uint8_t table[32],c;
static block_morse tx_b_t= table;

int main(void){
  int i;
  sei();
  serial_open();
  frame_init();
  //tx_b_t = (block_morse)table;
  
  while(!serial_can_read());
  while(1){
    while(1){    
      for(i = 0 ;((c = serial_get()) != '\r');i++){
	table[i]=c;
      }
      table[i]='\0';
      
      //print(tx_b_t);
      while(!frame_can_put);
      frame_block_put(tx_b_t);
    }
  }
}
