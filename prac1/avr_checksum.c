#include "avr_checksum.h"

#include <stdbool.h>
#include <stdint.h>
#include <util/crc16.h>

static val byte2hex(uint8_t byte){
  uint8_t a,b;
  val hex;
  a = byte>>4;
  b = byte & 0x0F;
  
  if (a>0x09 && a<= 0x0F)
    a+=55;
  else
    a+=48;
  
  if (b>0x09 && b<= 0x0F)
    b+=55;
  else
    b+=48;

  hex.a = a;
  hex.b = b;

  return hex;
}

static uint8_t hex2byte(val hex){
  val h;
  uint8_t a,b;

  if (hex.a>=0x41 && hex.a<= 0x5A)
    hex.a-=55;
  else
    hex.a-=48;
  
  if (hex.b>=0x41 && hex.b<=0x5A)
    hex.b-=55;
  else
    hex.b-=48;

  a = hex.a << 0x04;
  b = hex.b;

  return a|b;
}


val check_morse(char d[]){
  uint16_t suma=0,res1=0,carry=0;
  uint8_t sumafinal=0;
  val resultat;
  resultat.a=0;
  resultat.b=0;
  
  for(int i=0;d[i] != 0 ;i++){
    //si es un caracter que se puede transmitis por morse.
    if( (('0'<=d[i])&&(d[i]<='9')) || (('A'<=d[i]) &&(d[i]<='Z')) ||(d[i]==' ')){
      suma+=d[i];}
  }

  carry = suma & 0xFF00;
  res1 = suma & 0x00FF;
  carry=carry>>8;

  return byte2hex(~(res1 + carry));
}

bool test_check_morse(char d[]){
  uint16_t  carry=0,res1=0,suma=0;
  uint8_t sumafinal=0,final=0;
  int index = 0;
  val checksum;
  checksum.a=0;
  checksum.b=0;
  
  for(int i=0; d[i]!=0;i++){
    if(((d[i]>='0')&&(d[i]<='9')) || ((d[i]>='A') &&(d[i]<='Z'))||(d[i]==' ')){
      suma += d[i];
      index += 1;}}

  suma-=(d[index-2]+d[index-1]);
  carry = suma & 0xFF00;
  res1 = suma & 0x00FF;
  carry=carry>>8;
  sumafinal = res1 + carry;
  
  checksum.a=d[index-2];
  checksum.b=d[index-1];
  
  final=sumafinal+hex2byte(checksum);
  if (final == 0xFF)
    return true;
  else
    return false;
}

val crc_morse(char d[]){
  uint8_t crc=0;
  val c_crc;
  
  for(int i=0; d[i]!= 0;i++){
    if(((d[i]>='0')&&(d[i]<='9')) || ((d[i]>='A') &&(d[i]<='Z'))||(d[i]==' ')){
       crc = _crc_ibutton_update(crc, d[i]);
     }
  }
  
  c_crc = byte2hex(crc);
  return c_crc;
}

bool test_crc_morse(char d[]){
  int i=0;
  val c_crc, n_crc;
  while(d[i] != 0 ){
    i++;
  }
  c_crc.a=d[i-2];
  c_crc.b=d[i-1];
  d[i-2]='\0';
  
  n_crc=crc_morse(d);
  
  if((c_crc.a == n_crc.a) && (c_crc.b == n_crc.b))
    return true;
  else return false;
}


