#include "lan.h"
#include "avr_checksum.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>


static uint8_t rx_L[32],tx_L[32];
static block_morse tx_block_l;
block_morse rx_block_l; //extern
state estat = esperant;
static lan_callback_t fun;
static pin_t pin;
uint8_t node_lan;
int intents_enviar = 0;

static void enviaTrama(void);
static void preparaTrama(const block_morse b, uint8_t nd);
static void check_message(void);


void lan_init(uint8_t no){
  ether_init();
  node_lan=no;
  on_message_received(check_message);
  estat = esperant;
  pin=pin_create(&PORTB,5,Output);
  pin_w(pin,false);
}

bool lan_can_put(){
  if(ether_can_put())
    return true;
  else
    return false;
}

void lan_block_put(const block_morse b, uint8_t nd){
  tx_block_l = (block_morse)tx_L;
  estat = pendent_enviar;
  //func que retorna [No:Nd:M:CRC]
  preparaTrama(b,nd);
  enviaTrama();
  for(uint8_t i=0; i<32;i++)
    tx_L[i]='\0';
  estat = esperant;
  intents_enviar = 0;
}

uint8_t lan_block_get(block_morse b){
  int i;
  uint8_t nodeOrigen;
  nodeOrigen = rx_block_l[0];
  for(i=0; rx_block_l[i+2]!='\0';i++)
    b[i]=rx_block_l[i+2];
  b[i]='\0';
  
  return nodeOrigen; //No
}

void on_lan_received(lan_callback_t l){
  fun = l;
}

//func privades del modul

static void preparaTrama(const block_morse b, uint8_t nd){
  val crc;
  int i=0,x;
  
  tx_block_l[i++] = node_lan;
  tx_block_l[i++] = nd;
  for(x=0; b[x]!='\0';x++){
    tx_block_l[i++]=b[x];
  }
  crc = crc_morse(tx_block_l);
  
  tx_block_l[i++]=crc.a;
  tx_block_l[i++]=crc.b;
  tx_block_l[i++]='\0';
}

static void enviaTrama(void){
  if(intents_enviar < MAX){
    if(lan_can_put())
      ether_block_put(tx_block_l);
    else{
      intents_enviar++;
      timer_after(TIMER_MS((rand()%(10+1))*1000),enviaTrama);
    }
  }
  else{
    pin_w(pin,true);
    print("ERROR EN ENVIAR MSG");
  } 
}

static void check_message(void){
  if (ether_can_get()){
      for(uint8_t i=0; i<32;i++)
	rx_L[i]='\0';
      rx_block_l = (block_morse)rx_L;
      ether_block_get(rx_block_l);
      
      if(test_crc_morse(rx_L))
	if(rx_block_l[1]==node_lan){
	  fun();
	}
    }
}
