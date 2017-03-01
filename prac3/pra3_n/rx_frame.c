#include "frame.h"
#include "avr_checksum.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>

typedef enum {wait0, wait1,init} rx_states;
typedef enum {zero, one} received;

//generamos los estados de transnmission siempre esperamos un 0 o un 1.

static uint8_t rx_F[32];
static frame_callback_t funcio = NULL; //funcio descrita a la app "en teoria"
block_morse trama_rx = rx_F; //extern 

static void recepcio_msg(void);
static void canvi_estat(block_morse b);
rx_states rx_state;
received rec;

void frame_init(){
  timer_init();
  ether_init();
  on_message_received(recepcio_msg);
  rx_state = wait0;
}

void frame_block_get(block_morse b){
  int i;
  if(trama_rx[0]=='0' || trama_rx[0]=='1'){
    for(i=0;trama_rx[i+1]!='\0';i++)
      b[i] = trama_rx[i+1];
    b[i]='\0';
  }
  
}

void on_frame_received(frame_callback_t l){
  funcio = l;
  print("on frame rec");
}

void recepcio_msg(void){
  for(int i=0; i<32; i++)
    trama_rx[i]='\0';
  
  ether_block_get(trama_rx);
  
  if(test_crc_morse(trama_rx)){
    /**/
    if (trama_rx[0]=='1')
      rec = one;
    else if(trama_rx[0]=='0')
      rec = zero;

    canvi_estat(trama_rx);
  } 
  else {
    if(trama_rx[0]=='1'){
      ether_block_put("A18\0");
	}
    else if (trama_rx[0]=='0'){
      ether_block_put("BFA\0");
    }
  }
}

void canvi_estat(block_morse b){
  switch (rx_state){
  case wait0:
    switch(rec){
    case zero:
      if (funcio == NULL)
	print("perqueee");
      funcio();
      while(!ether_can_put);
      ether_block_put("A18\0");
      print(trama_rx);
      rx_state=wait1;
      break;
    case one:
      ether_block_put("BFA\0");
      break;
    }break;
  case wait1:
    switch(rec){
    case zero:
      ether_block_put("A18\0");
      break;
    case one:
      funcio();
      while(!ether_can_put());
      ether_block_put("BFA\0");
      print(trama_rx);
      rx_state=wait0;
      break;
    } break;
  }
}
