#include "frame.h"
#include "avr_checksum.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>

//definimos los estados
typedef enum {waitfor0, waitforA, waitfor1, waitforB} tx_states;
typedef enum {recA, recB,init} check_states;
static tx_states tx_state;
static check_states rx_ack_state;

static uint8_t tx_F[32],rx_ack[16];
static block_morse tx_block_f=tx_F ,rx_ack_block=rx_ack;
static int n_send = 0;
static timer_handler_t timeout_handler;
//static bool msg_enviat;

static void make_pkt(char sequencia, block_morse data);//ok
static void send_pkt(void);//ok
static void maquina_estats(block_morse tx_block_f);
static void receive_msg(void);//ok
static void fun_timeout(void);//ok

void frame_init(){
  ether_init();
  tx_state = waitfor0;
  rx_ack_state = init;
  n_send = 0;
  on_message_received(receive_msg);
  on_finish_transmission(fun_timeout);
}

bool frame_can_put(void){  
  if (tx_state==waitfor0 || tx_state==waitfor1)
    return true;
  else
    return false;
      
}

void frame_block_put(const block_morse b){
  maquina_estats(b);
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
  if(n_send<3){
    if(frame_can_put){
      ether_block_put(tx_block_f);
      print("enviant: ");
      print(tx_block_f);
      n_send = 0;
    }
    else{
      n_send++;
      timer_after(TIMER_MS((rand()%(10+1))*1000),send_pkt);
    }
  }
  else{
    print("ENCENDRE LED");
    n_send = 0;
  }
}

void receive_msg(void){
  for(int i=0; i<16; i++)
    rx_ack_block[i]='\0';
  
  ether_block_get(rx_ack_block);
  print("rebent:");
  print(rx_ack_block);
  if(test_crc_morse(rx_ack)){
    if(rx_ack_block[0] == 'A'){
      //tx_state = waitfor1;
      timer_cancel(timeout_handler);
      rx_ack_state= recA;
    }else if (rx_ack_block[0] == 'B'){
      //tx_state = waitfor0;
      timer_cancel(timeout_handler);
      rx_ack_state = recB;
    }
    maquina_estats(tx_block_f);
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
      tx_state = waitfor1;
      break;
    case(recB):
      //enviament de 3 vegades
      send_pkt();
      break;
    case(init):
      print("test: no hi hauriem d'estrar mai");
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
      tx_state = waitfor0;
      break;
    case(init):
      print("Test; init.");
      break;
    }
    break;
  }
}

void fun_timeout(void){
  timeout_handler=timer_after(TIMER_MS(9000),send_pkt);
}
