#include <stdio.h>
#include <stdbool.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "lan.h"

static uint8_t tx_a[32];
static block_morse tx_block = tx_a;


static void recepcio_msg(void);

uint8_t No;
bool var = true;
void main(void){
  
  uint8_t Nd,i,contingut;
  
  serial_open();
  sei();
  print("Node Origen?: ");
  while(!serial_can_read());
  No = serial_get();
  lan_init(No);
  on_lan_received(recepcio_msg);
  
  
  while(1){
    print("Node Desti:msg");
    
    var = true;
    serial_put(No);
    serial_put('-');
    serial_put('>');
    
    Nd = serial_get();
    serial_put(Nd);
    contingut = serial_get(); // en teoria es ':' 
    serial_put(contingut);
    
    contingut = serial_get();
    while(var){
      serial_put(contingut);
      if(contingut == 'r'){
	estat = esperant;
	i=0;
	print("RESET");
	var = false;
      }
      else if(contingut == '\r'){
	tx_block[i] = '\0';
	var = false;
      }
      else{
	tx_block[i++] = contingut;
	contingut = serial_get();
      }
    }
    
    while(!lan_can_put());//s'esta enviant un msg
    
    if(i > 0){
      i = 0;
      lan_block_put(tx_block, Nd);
      serial_put('\n');
    }    
  }
}

void recepcio_msg(void){
  uint8_t Nd;
  serial_put('\r');
  serial_put('\n');
  Nd = lan_block_get(rx_block_l);
  serial_put(Nd);
  serial_put('-');
  serial_put('>');
  serial_put(No);
  serial_put(':');
  print(rx_block_l);
  serial_put(No);
  serial_put('-');
  serial_put('>');
  //var=false;
}
