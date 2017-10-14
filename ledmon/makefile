# see LICENSE file for copyright and license details.

DEV="/dev/ttyACM0"
MODEL="atmega328p"
NAME=ledmon
LIB=lib/usart
CC=avr-gcc

options:
	@echo ${NAME} make options:
	@echo "   clean:  clean temporary files"
	@echo "   hex  :  compile source to hex binary"
	@echo "   flash:  upload hex binary to arduino"

clean:
	@echo cleaning
	@rm -f ${NAME} *.o *.hex lib/*.o

${LIB}.o: ${LIB}.c
	@avr-gcc -Os -DF_CPU=16000000UL -mmcu=${MODEL} -c -o ${LIB}.o ${LIB}.c

hex: ${LIB}.o
	@echo compiling binary
	@avr-gcc -Os -DF_CPU=16000000UL -mmcu=${MODEL} -c -o ${NAME}.o ${NAME}.c
	@avr-gcc -mmcu=${MODEL} ${NAME}.o ${LIB}.o -o ${NAME}
	@avr-objcopy -O ihex -R .eeprom ${NAME} ${NAME}.hex

flash: hex
	@echo preparing to flash
	@avrdude -F -V -c arduino -p ${MODEL} -P ${DEV} -b 115200 -U flash:w:${NAME}.hex
