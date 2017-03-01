#include "avr_checksum.h"

#include <pbn.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/crc16.h> 


int main(void){
  bool crc;
  uint8_t a=0,b=0, i=0;
  char entrada[32],c;
  serial_open();
  sei();
  while(1){
    entrada[0]='\0';
    i=0;
    print("Mensaje: ");
    while(!serial_can_read());
    while(1){
      c=serial_get();
      if(c != '\r'){
	entrada[i]=c;
	i++;
      }
      else 	
	break;
    }
    entrada[i]='\0';
    print(entrada);
    crc=test_crc_morse(entrada);
    if(crc)
      print("true");
    else 
      print("false");
    serial_put('\r');
    serial_put('\n');
  }
  cli();
  serial_close();
  return 0;
}
