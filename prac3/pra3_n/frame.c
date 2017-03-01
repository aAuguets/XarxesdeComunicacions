#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "avr_checksum.h"
#include "frame.h"

//creamos una maquina de estados que controla los dos sistemas.

typedef enum {waitfor0, waitforA, waitfor1, waitforB} tx_states;
typedef enum {recA, recB, init} check_states;
typedef enum {wait0, wait1} rx_states;
typedef enum {rx, tx} global_states;
//int est=0;

static frame_callback_t funcio = NULL; //funcio descrita a la app "en teoria"

static void recepcio_msg(void);
static void canvi_estat(block_morse b);

static tx_states tx_state;
static check_states rx_ack_state;
rx_states rx_state;
static global_states g_state;

static uint8_t tx_F[32],rx_ack[32];
static uint8_t rx_F[32];
static block_morse tx_block_f=tx_F ,rx_ack_block=rx_ack;
block_morse trama_rx = rx_F; //extern 
static int n_send = 0, send_notrec = 0;
static timer_handler_t timeout_handler;
//static bool msg_enviat;

//pin
static pin_t pin;


static void make_pkt(char sequencia, block_morse data);//ok
static void send_pkt(void);//ok
static void maquina_estats(block_morse tx_block_f);
static void fun_timeout(void);//ok
static void recepcio_msg(void);

void frame_init(){
  ether_init();
  tx_state = waitfor0;
  rx_state= wait0;
  rx_ack_state = init;
  n_send = 0;
  on_message_received(recepcio_msg);
  on_finish_transmission(fun_timeout);
  g_state = rx;

  pin=pin_create(&PORTB,5,Output);
  pin_w(pin,false);
}

void on_frame_received(frame_callback_t l){
  //print("on frame rec");
  funcio = l;
}

bool frame_can_put(void){  
  if (tx_state==waitfor0 || tx_state==waitfor1)
    return true;
  else
    return false;
}

void frame_block_put(const block_morse b){
  g_state = tx;
  maquina_estats(b);
}

void frame_block_get(block_morse b){
  int i;
  if(trama_rx[0]=='0' || trama_rx[0]=='1'){
    for(i=0;trama_rx[i+1]!='\0';i++)
      b[i] = trama_rx[i+1];
    b[i]='\0';
  }
}

void make_pkt(char secuencia,block_morse data){
  //preparamos la trama para enviarla
  for(int i=0; i<32; i++)
    tx_block_f[i]='\0';
  
  int i=0;
  val crc;
  tx_block_f[i++]=secuencia;
  
  for(int x=0; data[x] !='\0';x++){
    tx_block_f[i++]=data[x];
  }
  crc=crc_morse(tx_block_f);
  tx_block_f[i++]=crc.a;
  tx_block_f[i++]=crc.b;
  tx_block_f[i]='\0';
}


void send_pkt(void){
  if((n_send<3) && (send_notrec<3)){
    if(frame_can_put()){
      ether_block_put(tx_block_f);
      n_send = 0;
      send_notrec++;
    }
    else{
      n_send++;
      timer_after(TIMER_MS((rand()%(10+1))*1000),send_pkt);
    }
  }
  else{
    pin_w(pin,true);
    n_send = 0;
    send_notrec = 0;
    timer_cancel(timeout_handler);
  }
}

void maquina_estats(block_morse b){
  
  switch(tx_state){
  case waitfor0:
    make_pkt('0', b);
    send_pkt();
    tx_state=waitforA;
    break;
    
  case waitforA:
    switch(rx_ack_state){
    case(recA):
      send_notrec = 0;
      tx_state = waitfor1;
      break;
    case(recB):
      //enviament de 3 vegades
      send_pkt();
      break;
    case(init):
      break;
    }
    break;
  case waitfor1:
    make_pkt('1', b);
    send_pkt();
    tx_state=waitforB;
    break;
    
  case waitforB:
    switch(rx_ack_state){
    case(recA):
      send_pkt();
      break;
    case(recB):
      send_notrec = 0;
      tx_state = waitfor0;
      break;
    case(init):
      break;
    }
    break;
  }
}




void canvi_estat(block_morse b){
  switch (rx_state){
  case wait0:
    switch(b[0]){
    case '0':
      
      while(!ether_can_put);
      funcio();
      ether_block_put("A18\0");
      rx_state=wait1;
      break;
    case '1':
      ether_block_put("BFA\0");
      break;
    }break;
  case wait1:
    switch(b[0]){
    case '0':
      ether_block_put("A18\0");
      break;
    case '1':
      //funcio();

      while(!ether_can_put());
      funcio();
      ether_block_put("BFA\0");
      rx_state=wait0;
      break;
    } break;
  }
}

void fun_timeout(void){
  if(g_state==tx)
    timeout_handler=timer_after(TIMER_MS(9000),send_pkt);
}

void recepcio_msg(void){
  for(int p=0; p<32; p++)
    trama_rx[p]='\0';
  
  ether_block_get(trama_rx);
  if((trama_rx[0]=='1') || (trama_rx[0]== '0')){
    if((tx_state != waitforA) || (tx_state != waitforB))
      g_state = rx;
    if(test_crc_morse(trama_rx)){
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
  else if (((trama_rx[0]=='A') ||(trama_rx[0]== 'B'))&& g_state==tx){
    //ether_block_get(rx_ack_block);
    //est=1;
    
    if(test_crc_morse(trama_rx)){
      if(trama_rx[0] == 'A'){
  	//tx_state = waitfor1;
  	timer_cancel(timeout_handler);
  	rx_ack_state= recA;
	maquina_estats(trama_rx);
      }
      else if (trama_rx[0] == 'B'){
  	//tx_state = waitfor0;
  	timer_cancel(timeout_handler);
  	rx_ack_state = recB;
	maquina_estats(trama_rx);
      }
      
    }
  }
}
