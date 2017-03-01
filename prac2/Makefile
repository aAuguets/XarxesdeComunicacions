xarxa_local:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c lan.c aplicacio.c -o xarxa libpbn/libpbn.a
	avr-objcopy -Oihex xarxa xarxa.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U xarxa.hex
	picocom /dev/ttyACM0

xarxa_local2:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c lan.c aplicacio.c -o xarxa libpbn/libpbn.a
	avr-objcopy -Oihex xarxa xarxa.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM1 -U xarxa.hex
	picocom /dev/ttyACM1
