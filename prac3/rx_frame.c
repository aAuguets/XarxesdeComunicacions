#include "frame.h"
#include "avr_checksum.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>

typedef enum {wait_for_0, wait_for_1} rx_states;

static uint8_t rx_F[32];
static frame_callback_t fun_app; //funcio descrita a la app "en teoria"
block_morse rx_block_f; //extern 
static void check_message(void);
static void test_rx(void);
static void test(void);
rx_states rx_state;

void frame_init(){
  serial_open();
  ether_init();
  rx_state = wait_for_0;
  //timer_after(5000,test);
  on_message_received(check_message);
}

/* void test(void){ */
/*   print("ook"); */
/*   on_message_received(check_message); */
/* } */

/* void test_rx(void){ */
/*   print("E"); */
}

void frame_block_get(block_morse b){
  //lo que tengamos en la cola de recepcion rx_block lo pasamos
  // a nuestra tabla b
  int i;
  if(rx_block_f[0]=='0' || rx_block_f[0]=='1'){
    for(i=0; rx_block_f[i+1]!='\0'; i++)
      b[i] = rx_block_f[i+1];
    b[i-2] = '\0';
  }
}

void on_frame_received(frame_callback_t l){
  fun_app = l;
}

static void check_message(void){
  rx_block_f = (block_morse)rx_F;
  ether_block_get(rx_block_f);
  print(rx_block_f);
  if(test_crc_morse(rx_block_f)){
    //fun_app();
    if((rx_state == wait_for_0) && (rx_block_f[0] == '0')){
      fun_app();
      ether_block_put("A18\0");
      rx_state = wait_for_1;//ara canviem espera de 0 a 1.
    }
    else if((rx_state == wait_for_1) && (rx_block_f[0] == '1')){
      fun_app();
      ether_block_put("BFA\0");
      rx_state = wait_for_0;//ara canviem espera de 1 a 0.
    }
    else{ //crc ok pero msg repetit
      if(rx_state == wait_for_0){
  	ether_block_put("BFA\0");
      }
      else{
  	ether_block_put("A18\0");
      }
    }
  }
  else{ //crc not ok
    if(rx_state == wait_for_0)
      ether_block_put("BFA\0");
    else
      ether_block_put("A18\0");
  }
}
