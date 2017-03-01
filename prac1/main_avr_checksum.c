#include "avr_checksum.h"
#include <pbn.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

int main(void){
  
  int MAX_read = 15,i=0;
  char entrada[MAX_read],c;
  val cs;
  
  serial_open();
  sei();

  while(1){
    print("Escriu msg: ");
     entrada[0]= '\0';
    i=0;
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
    //while(!serial_can_read());
    //readline(entrada,MAX_read);
    cs = check_morse(entrada);
    print(entrada);
    serial_put(cs.a);
    serial_put(cs.b);
    serial_put('\r');
    serial_put('\n');
  }
  cli();
  serial_close();
  return 0;
}

