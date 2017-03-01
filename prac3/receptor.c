#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "avr_checksum.h"
#include "frame.h"

//static uint8_t table[32];
//static block_morse rx_b_t;
static void printb(void);

int main(void){  
  //serial_open();
  sei();
  frame_init();
  on_frame_received(printb);
  
  while(1);
}


/*
void ch_msg(void){
  if (ether_can_get()){
    //rx_b_t = (block_morse)table;
    on_frame_received(printb);
    
      //(rx_block_l[1]==node_lan){
  }
}
*/
void printb(void){
  frame_block_get(rx_block_f);
  print(rx_block_f);
  //tenemos que vaciar la tabla
  
}
