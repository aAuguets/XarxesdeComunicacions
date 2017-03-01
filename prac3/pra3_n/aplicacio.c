#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <pbn.h>
#include "frame.h"

static uint8_t rx[32],a;
static block_morse trx=rx;
static void error(void);
static void pinta(void);
static void buidar(block_morse m);

static void buidar(block_morse b){
  for(int u=0;u<32;u++) b[u]='\0';
}

static void pinta(void){
  frame_block_get(trx);
  for(int p=0;trx[p]!='\0';p++) serial_put(trx[p]);
  serial_put('\r');
  serial_put('\n');
  buidar(trx);
}

static void error(void){
  serial_put('E');
  serial_put('R');
  serial_put('R');
  serial_put('O');
  serial_put('R');
  serial_put('\r');
  serial_put('\n');
}

int main(){
  uint8_t w,i,a,b;
  uint8_t block_tx[32];
  block_morse blocktx=block_tx;
  frame_init();
  serial_open();
  sei();
  buidar(blocktx);
  while(1){
    on_frame_received(pinta);
    while(!frame_can_put()){
      if (serial_can_read()){
	b=serial_get();
      }
    }
    if (serial_can_read()){
      a = serial_get();
      i=0;
      while (a!='\r'){
	if ((a>='0' && a<='9')||(a>='A' && a<='Z')||(a==' ')) {
	  blocktx[i] = a;
	  serial_put(a);
	  a = serial_get();
	  w=0;
	  i+=1;
	}
	else {
	  serial_put(a);
	  w=1;
	  break;
	}
      }
      if (w==0){
	blocktx[i]='\0';
	serial_put('\r');
	serial_put('\n');
	frame_block_put(blocktx);
      }
      else if (w==1){
	error();
      }
      w=0;
      buidar(blocktx);
    }
  }
  return 0;
}
