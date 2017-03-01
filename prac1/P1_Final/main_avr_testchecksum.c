#include "avr_checksum.h"
#include <pbn.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

int main(void){
  
  int MAX_read = 32,i=0;
  char entrada[MAX_read],c;
  val cs;
  bool resultat;
  
  serial_open();
  sei();

  while(1){
    print("Escriu msg seguit del Checksum. 'Exemple: HOLADA': ");

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
    print(entrada);
    resultat = test_check_morse(entrada);
    
    if (resultat)
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

