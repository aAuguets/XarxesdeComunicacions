#include "avr_checksum.h"

#include <pbn.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/crc16.h> 


int main(void){
  val crc;
  uint8_t a=0,b=0;
  char entrada[32],c;
  int nofinaldecarro=1,i=0;
  crc.a=0;
  crc.b=0;
  serial_open();
  sei();
  while(1){
    entrada[0]= '\0';
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
    //readline(entrada,32);
    print(entrada);
    crc=crc_morse(entrada);
    serial_put(crc.a);
    serial_put(crc.b);
    serial_put('\r');
    serial_put('\n');
  }
  cli();
  serial_close();
  return 0;
}
