checksum_terminal:
	gcc -std=c99 -o checksum ex2_checksum.c
	./checksum 

test_checksum:
	gcc -std=c99 -o  test_checksum 3_error_morse.c
	./test_checksum

checksum_arduino:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c main_avr_checksum.c -o avr_checksum libpbn/libpbn.a
	avr-objcopy -Oihex avr_checksum avr_checksum.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U avr_checksum.hex
	picocom /dev/ttyACM0 -c

test_checksum_arduino:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c main_avr_testchecksum.c -o avr_testchecksum libpbn/libpbn.a
	avr-objcopy -Oihex avr_testchecksum avr_testchecksum.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U avr_testchecksum.hex
	picocom /dev/ttyACM0 -c	


crc:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c crc_main.c -o crc_main libpbn/libpbn.a
	avr-objcopy -Oihex crc_main crc_main.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U crc_main.hex
	picocom /dev/ttyACM0 -c


test-crc:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c crc_test_main.c -o crc_test_main libpbn/libpbn.a 
	avr-objcopy -Oihex crc_test_main crc_test_main.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U crc_test_main.hex
	picocom /dev/ttyACM0 -c


