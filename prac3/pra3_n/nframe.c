#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>
#include <avr/interrupt.h>

#include "avr_checksum.h"
#include "frame.h"

#define TIMEOUT 9000
#define TRYAGAIN 5000

typedef enum {sApp, sTimeout, sReceived, again} states;
typedef enum {send, receive} estats;
typedef enum {no, ok} finishtransmission; 

static frame_callback_t funcio = NULL; 

static estats estat; // estat de la maquina.
static states state; // estats de control de msgs.
static finishtransmission finisht; //estat que controla si fem timeout o no.

static uint8_t tx_F[32],rx_ack[32],rx_trama[32];
static uint8_t received[32]="";
static uint8_t tramaZeroU,estat_ack,crida;
static block_morse tx_block_f=tx_F ,rx_ack_block=rx_ack, rx_block=rx_trama, received_block = received;
static int n_send = 0;
static timer_handler_t timeout_handler;
static bool test;

//pin
static pin_t pin;

static void recepcio_msg(void);
static void canvi_estat(block_morse b);
static void fun_timeout(void);
static void send_pkt(void);
static void make_pkt(block_morse data);
static void automat_rx(void);
static void automat_tx(states E);
static bool trama_ok(void);
static void recepcio_msg(void);
static void cridatime(void);
static bool rec_ok(void);

void frame_init(){
  ether_init();
  estat = send;
  n_send = 0;
  crida = 0;
  tramaZeroU = 0;
  estat_ack = 0;
  on_message_received(recepcio_msg);
  on_finish_transmission(fun_timeout);
  pin=pin_create(&PORTB,5,Output);
  pin_w(pin,false);
}

void on_frame_received(frame_callback_t l){
  funcio = l;
}

bool frame_can_put(void){  
  if (estat == send)
    return true;
  else
    return false;
}

void frame_block_put(const block_morse b){
  make_pkt(b);
  automat_tx(sApp);
}

void frame_block_get(block_morse b){
  int i;
  if(rx_ack_block[0]=='0' || rx_ack_block[0]=='1'){
    for(i=0;rx_ack_block[i+1]!='\0';i++)
      b[i] = rx_ack_block[i+1];
    b[i]='\0';
  }
}

static void make_pkt(block_morse data){
  //preparamos la trama para enviarla
  for(int i=0; i<32; i++)
    tx_block_f[i]='\0';
  
  int i=0;
  val crc;
  if(tramaZeroU == 0)
    tx_block_f[i++] = '0';
  else
    tx_block_f[i++] = '1';
  
  
  for(int x=0; data[x] !='\0';x++){
    tx_block_f[i++]=data[x];
  }
  crc=crc_morse(tx_block_f);
  tx_block_f[i++]=crc.a;
  tx_block_f[i++]=crc.b;
  tx_block_f[i]='\0';
}

static void control(void){
  if(n_send < 3){
    automat_tx(sTimeout);
    n_send ++;
  }
  else{
    timer_cancel(timeout_handler);
    n_send=0;
    estat = send;
  }
}

static void fun_timeout(void){
  if (finisht==ok)
    timeout_handler=timer_after(TIMER_MS(TIMEOUT),control);
}

/*Noves funcions de recepcio*/

static bool trama_ok(void){
  bool ok = false;
  switch(estat_ack){
  case 0:
    if (rx_ack_block[0] == '0')
	ok = true;
      else
	ok = false;
      break;
    case 1:
      if (rx_ack_block[0] == '1')
	ok = true;
      else
	ok = false;
      break;
  }
  return ok;
}

static void automat_rx(void){
 switch(estat_ack){
 case 0:
   if(trama_ok() && test_crc_morse(rx_ack_block)){
     finisht = no;
     ether_block_put("A18\0");
     funcio();
     estat_ack ^= 1;
   }
   else{
     finisht = no;
     ether_block_put("BFA\0");}
   break;
 case 1:
   if(trama_ok() && test_crc_morse(rx_ack_block)){
     finisht = no;
     ether_block_put("BFA\0");
     funcio();
     estat_ack ^= 1;
   }
   else{
     ether_block_put("A18\0");
     finisht = no;}
    break;
 }
}
/*fi funcions de transmisio*/

/*noves func transmisio*/
static bool rec_ok(void){
  if ((((tramaZeroU == 0) && (rx_block[0] == 'A')) || ((tramaZeroU == 1) && (rx_block[0] == 'B'))) && test_crc_morse(rx_block)){
    return true;}
  else{
    return false;}						 
}

static void cridatime(void){
  automat_tx(again);
}
static void automat_tx(states E){
  switch(estat){
  case send:
    if( ether_can_put() && (E == sApp)){
      finisht = ok;
      ether_block_put(tx_block_f);
      estat = receive;
    }
    else if(!ether_can_put() && (E == sApp)){
      timer_after(TIMER_MS(TRYAGAIN),cridatime);
    }
    else if(!ether_can_put() && (E == again)){
      timer_after(TIMER_MS(TRYAGAIN),cridatime);
    }
    else if(ether_can_put() && (E = again)){
      finisht = ok;
      ether_block_put(tx_block_f);
      estat = receive;
    }
    else if(E = sTimeout){
      pin_w(pin,true);}
    break;
  case receive:
    if((E == sTimeout)&& ether_can_put()){
      ether_block_put(tx_block_f);
    }
    else if(rec_ok() && (E == sReceived)){
      print("control: msg sent.");
      n_send=0;
      timer_cancel(timeout_handler);
      tramaZeroU ^= 1; 
      estat = send;
    }
    else if (E == sApp){
      pin_w(pin,true);}
    break;
  }
}
/*fi funcions de transmisio*/

void recepcio_msg(void){
  ether_block_get(received_block);

  if((received_block[0] == '0') || (received_block[0] == '1')){
    rx_ack_block = received_block;
    automat_rx();
  }
  else if((received_block[0] == 'A') || (received_block[0] == 'B')){
    rx_block = received_block;
    automat_tx(sReceived);
    }
}
