napp0:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c nframe.c app.c -o app libpbn/libpbn.a
	avr-objcopy -Oihex app app.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U app.hex
	picocom /dev/ttyACM0

napp1:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c nframe.c app.c -o app libpbn/libpbn.a
	avr-objcopy -Oihex app app.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM1 -U app.hex
	picocom /dev/ttyACM1



app0:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c frame.c app.c -o app libpbn/libpbn.a
	avr-objcopy -Oihex app app.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U app.hex
	picocom /dev/ttyACM0

app1:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c frame.c app.c -o app libpbn/libpbn.a
	avr-objcopy -Oihex app app.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM1 -U app.hex
	picocom /dev/ttyACM1

tx:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c tx_frame.c transmisor.c -o transmisor libpbn/libpbn.a
	avr-objcopy -Oihex transmisor transmisor.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U transmisor.hex
	picocom /dev/ttyACM0

rx:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c rx_frame.c receptor.c -o receptor libpbn/libpbn.a
	avr-objcopy -Oihex receptor receptor.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U receptor.hex
	picocom /dev/ttyACM0

rx1:
	avr-gcc -std=c99 -Os -mmcu=atmega328p -fshort-enums -DF_CPU=16000000UL -Ilibpbn avr_checksum.c rx_frame.c receptor.c -o receptor libpbn/libpbn.a
	avr-objcopy -Oihex receptor receptor.hex
	avrdude -c arduino -p atmega328p -P /dev/ttyACM1 -U receptor.hex
	picocom /dev/ttyACM1
