all:
	mkdir -p bin
	avr-gcc -Wall -mmcu=atmega168 -DGCC_MEGA_AVR -DF_CPU=20000000UL -Os -I. -I../FreeRTOS -I../FreeRTOS/ATmega168 *.c ../FreeRTOS/*.c ../FreeRTOS/ATmega168/*.c ../FreeRTOS/MemMang/heap_1.c -o bin/avrfrirtos -Wl,-Map=bin/avrfrirtos.map
	avr-size bin/avrfrirtos

isp:
	avr-objcopy -O ihex bin/avrfrirtos bin/avrfrirtos.hex
	avrdude -c stk500v2 -P avrdoper -p m168p -U flash:w:bin/avrfrirtos.hex:i

clean:
	rm -rf bin
