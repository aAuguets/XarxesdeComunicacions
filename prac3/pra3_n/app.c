#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "avr_checksum.h"
#include "frame.h"

static void print_msg(void);
uint8_t t[32];
block_morse taula=t;


void main(void){
  uint8_t i, c;
  serial_open();
  frame_init();
  on_frame_received(print_msg);
  sei();
  
  while(1){
  while(!serial_can_read());
  //while(1){
    serial_put('-');
    serial_put('>');
    for(i = 0 ;((c = serial_get()) != '\r');i++){
      taula[i]=c;
      serial_put(c);
    }
    taula[i]='\0';
    serial_put('\r');
    serial_put('\n');
    //print(tx_b_t);
    while(!frame_can_put);
    /*
     * to test ->
     * while(!ether_can_put);
     */
    frame_block_put(taula);
  }
}

void print_msg(void){
  frame_block_get(taula);
  serial_put('<');
  serial_put('-');
  print(taula);
  for(int i=0;i<32;i++)
    taula[i]='\0';
  
}
