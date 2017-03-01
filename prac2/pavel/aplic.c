#include <pbn.h>
#include "lan.h"
#include "gpio_device.h"
#include <avr/io.h>
//creamos dos tablas en las que guardamos los mensajes que nos llegan y que enviamos
//las taulas son de tipo block_morse
static block_morse rx;
static block_morse tx;

//generamos una funcion para cuando queramos transmitir
//recibimos "ND:mensaje" por el port serie
static void transmision(block_morse msg){
  int i=0;
  serial_put(NO);
  serial_put('-');
  serial_put('>');
  ND=serial_get();
//comprobamos que el nodo desti es correcto sino no hacemos nada  
  if (((ND <= 'Z') && (ND >='A')) || ((ND >='0') && (ND<='9'))){ 
    serial_get();//recibimos :
    serial_put(ND);
    serial_put(':');
    for(;(msg[i]=serial_get())!= '\r';i++){
      if(msg[i]=='r'){
	break;
      }
      //si es una letra cualquiera
      else
	serial_put(msg[i]);
    }
    msg[i]='\0';
    i=0;
  }
  else break;

}
static void recepcion(void){

  node_origen=lan_block_get(rx);
  serial_put(node_origen);
  serial_put('-');
  serial_put('>');
  for(int i=0;rx[i]/='\0';i++){
    serial_put(rx[i]);
  }
  serial_put('\0');
  serial_put('\r');
  serial_put('\n');
}

int main(void){
  
  lan_init(NO);
  on_lan_received(recepcion);
  sei();
  while (true){
    while(!serial_can_read());
    while(!lan_can_put());
    transmision(msg);
    lan_block_put(msg,ND);
    
  }

}
