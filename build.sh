#!/bin/bash
# ============================================================
#  FreeRTOS build script — ATmega168
#  Usage:  ./build.sh          (build only)
#          ./build.sh flash    (build + flash via avrdude)
#          ./build.sh clean    (remove build artifacts)
# ============================================================

# ---- Configuration ----
# MCU="atmega169pa"
MCU="atmega168"
F_CPU="8000000UL"
OPT="-Os"
OUTPUT="main"

PROGRAMMER="usbasp"        # change to your programmer
PORT="/dev/ttyACM0"        # change to your port
BAUD="115200"

# ---- Include paths ----
INCLUDES=(
    -I .
    -I ./FreeRTOS-ATmega168/FreeRTOS
    -I ./FreeRTOS-ATmega168/FreeRTOS/ATmega168
    -I ./HAL
    -I ./MCAL
)

# ---- Source files ----
SOURCES=(
    main.c

    # HAL drivers
    HAL/lm35.c
    HAL/motor.c
    HAL/button.c
    HAL/led.c
    HAL/lcd.c

    # MCAL drivers
    MCAL/uart.c

    # FreeRTOS kernel
    FreeRTOS-ATmega168/FreeRTOS/tasks.c
    FreeRTOS-ATmega168/FreeRTOS/queue.c
    FreeRTOS-ATmega168/FreeRTOS/list.c
    FreeRTOS-ATmega168/FreeRTOS/ATmega168/port.c
    FreeRTOS-ATmega168/FreeRTOS/MemMang/heap_1.c
)

# ---- Common compiler flags ----
CFLAGS="$OPT -DF_CPU=$F_CPU -mmcu=$MCU -std=gnu99 -Wall -Wextra -Wno-unused-parameter"

# ============================================================
#  Clean
# ============================================================
if [ "$1" == "clean" ]; then
    echo "Cleaning build artifacts..."
    rm -f build/*.o "$OUTPUT.elf" "$OUTPUT.hex"
    rmdir build 2>/dev/null
    echo "Done."
    exit 0
fi

# ============================================================
#  Build
# ============================================================
clear
mkdir -p build

echo "========================================"
echo "  Building for $MCU @ $F_CPU"
echo "========================================"

# 1. Compile each .c → .o
for src in "${SOURCES[@]}"; do
    obj="build/$(basename "${src%.c}.o")"
    echo "  CC  $src"
    avr-gcc $CFLAGS "${INCLUDES[@]}" -c "$src" -o "$obj"
    if [ $? -ne 0 ]; then
        echo "ERROR: Compilation failed on $src"
        exit 1
    fi
done

# 2. Link all .o → .elf
echo "  LD  $OUTPUT.elf"
avr-gcc -mmcu=$MCU build/*.o -o "$OUTPUT.elf"
if [ $? -ne 0 ]; then
    echo "ERROR: Linking failed"
    exit 1
fi

# 3. Create .hex
echo "  HEX $OUTPUT.hex"
avr-objcopy -O ihex -R .eeprom "$OUTPUT.elf" "$OUTPUT.hex"

# 4. Print size report
echo ""
avr-size --format=avr --mcu=$MCU "$OUTPUT.elf"
echo ""
echo "Build complete: $OUTPUT.hex"

# ============================================================
#  Flash (optional)
# ============================================================
if [ "$1" == "flash" ]; then
    echo ""
    echo "Flashing $OUTPUT.hex → $MCU ..."
    avrdude -p $MCU -c $PROGRAMMER -P $PORT -b $BAUD -U flash:w:$OUTPUT.hex:i
fi
