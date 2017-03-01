#include "frame.h"
#include "avr_checksum.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pbn.h>

//definimos los estados
typedef enum {esperando0,esperandoA,esperando1,esperandoB} tx_states;
typedef enum {recibidoA, recibidoB,error} check_states;
tx_states estado_tx;
check_states estado_ab;

static uint8_t tx_F[32],rx_ack[16];
static block_morse tx_block_f,rx_ack_block;
static bool msg_enviat;

static void make_pkt(char sequencia, block_morse data);
static void envia(block_morse t);
static void maquina_estats(block_morse tx_block_f);
static void canvi_estat(void);
static void recibe_mensaje(void);

void frame_init(){
  ether_init();
  estado_tx = esperando0;
  estado_ab = error;
  on_message_received(recibe_mensaje);
}

bool frame_can_put(void){
  //cuando retorna true el mensaje sera transmitido corectamente, 
  // si es true, se puede enviar otro mensaje
  //si es false 
  
  if (estado_tx==esperando0 || estado_tx==esperando1)
    return true;
  else
    return false;
      
}

void frame_block_put(const block_morse b){
  //pponemos en el tx_block_f el mensaje de la manera correcta que enviamos
  tx_block_f= (block_morse) tx_F;
  //si podemos transmitir
      //enviamos la trama correctamente al ether
  //si no podemos transmitir-> 
      // lo volvemos a intentar al cabo de un tiempo
  
  maquina_estats(b);
  
}

void make_pkt(char secuencia,block_morse data){
  //preparamos la trama para enviarla
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


void envia(block_morse tx_block_f){
  //falta emter los intentos. y el timeout.
  if(frame_can_put){
    print(tx_block_f);
    ether_block_put(tx_block_f);
    //reiniciamos el timeout
}
  else{

    //enviamos errror y encendemos led
    print("ERROR al enviar");
  }
}

void recibe_mensaje(void){
  rx_ack_block = (block_morse)rx_ack;
  ether_block_get(rx_ack_block);
  print(rx_ack_block);
  
  if(test_crc_morse(rx_ack)){
    if(rx_ack_block[0] == 'A')
      estado_ab = recibidoA;
    else if (rx_ack_block[0] == 'B')
      estado_ab = recibidoB;
  }else
    estado_ab = error;
  maquina_estats(tx_block_f);
}
void canvi_estat(void){
  switch(estado_tx){
  case esperando0:
    estado_tx = esperandoA;
    break;
  case esperandoA:
    estado_tx = esperando1;
    break;
  case esperando1:
    estado_tx = esperandoB;
    break;
  case esperandoB:
    estado_tx = esperando0;
    break;
  }
}
void maquina_estats(block_morse b){
  switch(estado_tx){
  case esperando0:
    make_pkt('0', b);
    envia(tx_block_f);
    //canvi_estat();
    estado_tx=esperandoA;
    break;
    
  case esperandoA:
    switch(estado_ab){
    case(recibidoA):
      canvi_estat();
      break;
    case(recibidoB):
      envia(tx_block_f);
      break;
    case(error):
      envia(tx_block_f);
      break;
    }
    break;
  case esperando1:
    make_pkt('1', b);
    envia(tx_block_f);
    canvi_estat();
    break;
    
  case esperandoB:
    switch(estado_ab){
    case(recibidoA):
      envia(tx_block_f);
      break;
    case(recibidoB):
      canvi_estat();
      break;
    case(error):
      envia(tx_block_f);
      break;
    }
    break;
  }
}
