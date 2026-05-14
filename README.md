# 🌡️ Lab 8 — Temperature-Based Motor Control with FreeRTOS

> A real-time embedded system built on **ATmega168** using **FreeRTOS** that automatically controls a DC motor based on ambient temperature, with a push-button override and a live LCD display.

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Hardware Components](#-hardware-components)
- [System Architecture](#-system-architecture)
- [FreeRTOS Task Design](#-freertos-task-design)
- [Project Structure](#-project-structure)
- [Pin Configuration](#-pin-configuration)
- [FreeRTOS Configuration](#-freertos-configuration)
- [Simulation Screenshots](#-simulation-screenshots)
- [Eclipse IDE View](#-eclipse-ide-view)
- [How to Build & Flash](#-how-to-build--flash)
- [UART Output](#-uart-output)

---

## 🔍 Overview

This project implements a **multitasking embedded control system** using FreeRTOS on the ATmega168 microcontroller. The system continuously monitors temperature via the **LM35 analog sensor**, drives a **DC motor** through an L298P H-Bridge, displays real-time status on a **16×2 HD44780 LCD**, and allows the user to **override** motor operation via a push button.

The design demonstrates core RTOS concepts including:
- **Preemptive task scheduling** with priority levels
- **Queue-based inter-task communication**
- **Mutex-protected shared resources** (UART + LCD)
- **Edge-triggered button debouncing** via polling

---

## ✨ Features

| Feature | Description |
|---|---|
| 🌡️ Temperature Sensing | LM35 sensor read every 500 ms via ADC |
| 🔁 Automatic Motor Control | Motor runs `FORWARD` above 25 °C, `STOP` below |
| 🔘 Push-Button Override | Toggle override mode — forces motor OFF regardless of temperature |
| 📺 Live LCD Display | 16×2 LCD shows current temperature and motor state in real time |
| 🔒 Thread-Safe UART | Mutex ensures no garbled debug output from concurrent tasks |
| ⚡ Instant Override Response | Button task runs at **priority 2** — preempts temperature task immediately |

---

## 🔧 Hardware Components

| Component | Model / Details |
|---|---|
| Microcontroller | **ATmega168** @ 8 MHz |
| Temperature Sensor | **LM35** (analog, 10 mV/°C) |
| Motor Driver | **L298P H-Bridge** |
| DC Motor | Standard 5 V DC motor |
| Display | **HD44780 16×2 LCD** (4-bit mode) |
| Push Button | Normally-open, active-low pull-up |
| Simulator | **SimulIDE 1.1.0-SR1** |

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                       FreeRTOS Scheduler                    │
│  ┌──────────────────┐  ┌──────────────────────────────────┐ │
│  │  vButtonTask     │  │        vTemperatureTask          │ │
│  │  Priority: 2     │  │        Priority: 1               │ │
│  │                  │  │                                  │ │
│  │ • Polls button   │  │ • Reads LM35 every 500 ms        │ │
│  │   every 50 ms    │  │ • Compares vs. 25°C threshold    │ │
│  │ • Detects edges  │  │ • Sends cmd → motorQueue         │ │
│  │ • Toggles        │  │ • Updates LCD row 0              │ │
│  │   override flag  │  │ • Logs temp to UART              │ │
│  │ • Sends cmd →    │  └──────────────────────────────────┘ │
│  │   motorQueue     │                                        │
│  └──────────────────┘  ┌──────────────────────────────────┐ │
│                        │        vMotorTask                │ │
│                        │        Priority: 1               │ │
│                        │                                  │ │
│  ┌──────────────────┐  │ • Blocks on motorQueue           │ │
│  │   motorQueue     │→ │ • Drives L298P H-Bridge          │ │
│  │  (size 1, u8)    │  │ • Updates LCD row 1              │ │
│  └──────────────────┘  │ • Logs state change to UART      │ │
│                        └──────────────────────────────────┘ │
│  ┌──────────────────┐                                        │
│  │   uartMutex      │← shared by all 3 tasks                 │
│  └──────────────────┘                                        │
└─────────────────────────────────────────────────────────────┘
```

---

## 📌 FreeRTOS Task Design

### Task 1 — `vButtonTask` *(Priority 2 — Highest)*

- Polls `BUTTON_read()` every **50 ms** (debounce window)
- Detects **rising edge** (button press)
- Toggles the `override_mode` volatile flag
- When override is **ON** → immediately sends `MOTOR_CMD_STOP` via `xQueueOverwrite`
- When override is **OFF** → instantly re-reads temperature and restores correct motor state
- All UART output is guarded by `uartMutex`

### Task 2 — `vTemperatureTask` *(Priority 1)*

- Reads the **LM35** ADC value every **500 ms**
- Compares temperature against `TEMP_THRESHOLD` (25 °C)
- Only sends motor commands when **not** in override mode
- Updates **LCD row 0**: `Temp: XX C` (appends `OVR` suffix when override is active)
- Logs temperature and override status to **UART**

### Task 3 — `vMotorTask` *(Priority 1)*

- **Blocks indefinitely** on `xQueueReceive(motorQueue, ...)`
- Drives the motor only when the **state actually changes** (avoids redundant HAL calls)
- Updates **LCD row 1**: `Motor: FORWARD` or `Motor: STOPPED`
- Logs state changes to **UART** (mutex-protected)

---

## 📁 Project Structure

```
Lab8_RTOS/
│
├── main.c                  # Application entry point — task creation & scheduler
├── FreeRTOS.h              # FreeRTOS kernel header
├── FreeRTOSConfig.h        # Kernel configuration (tick rate, heap, priorities)
│
├── FreeRTOS/               # FreeRTOS kernel source (portable)
├── FreeRTOS-ATmega168/     # ATmega168 port layer
│
├── HAL/                    # Hardware Abstraction Layer
│   ├── lm35.c / lm35.h     # LM35 temperature sensor driver (ADC)
│   ├── motor.c / motor.h   # DC motor driver (L298P H-Bridge)
│   ├── button.c / button.h # Push-button driver (active-low, pull-up)
│   ├── lcd.c / lcd.h       # HD44780 16×2 LCD driver (4-bit mode)
│   └── led.c / led.h       # LED indicator driver
│
├── MCAL/                   # Microcontroller Abstraction Layer
│   ├── uart.c / uart.h     # UART driver (polling, configurable baud)
│
├── simulide/
│   └── lab8.sim1           # SimulIDE circuit schematic
│
├── screenshots/            # Simulation demo screenshots
├── build/                  # Compiled object files
├── main.elf                # ELF binary (debug)
├── main.hex                # Intel HEX file (flash target)
└── build.sh                # AVR-GCC build script
```

---

## 🔌 Pin Configuration

### LCD (HD44780 — 4-bit mode via PORTD)

| LCD Pin | ATmega168 Pin |
|---------|---------------|
| RS      | PD2           |
| EN      | PD3           |
| D4      | PD4           |
| D5      | PD5           |
| D6      | PD6           |
| D7      | PD7           |

### Motor (L298P H-Bridge via PORTB)

| Function | ATmega168 Pin |
|----------|---------------|
| IN1      | PB2           |
| IN2      | PB3           |

### Other Peripherals

| Peripheral | ATmega168 Pin |
|------------|---------------|
| Push Button | PB5 (active-low) |
| LM35 (ADC) | PC0 / ADC0    |
| UART TX    | PD1           |
| UART RX    | PD0           |

---

## ⚙️ FreeRTOS Configuration

| Parameter | Value |
|-----------|-------|
| CPU Clock | 8 MHz |
| Tick Rate | 1000 Hz (1 ms ticks) |
| Max Priorities | 4 |
| Total Heap | 600 bytes |
| Min Stack Size | 60 words |
| Preemption | Enabled |
| Mutexes | Enabled |
| Counting Semaphores | Enabled |
| 16-bit Ticks | Enabled |

> **Note:** The tiny heap (600 B) is intentional for the ATmega168's 1 KB SRAM. Each task uses an 80-word stack.

---

## 🖥️ Eclipse IDE View

The project is developed using **Eclipse IDE** with the **AVR plugin**. The screenshot below shows the `main()` function — peripheral initialisation, FreeRTOS object creation, task spawning, and scheduler launch — open in the editor alongside the Project Explorer.

![Eclipse IDE — main.c task creation and scheduler launch](screenshots/Screenshot%20from%202026-05-15%2000-30-48.png)

> The Project Explorer panel on the left shows the `Lab8_RTOS` project alongside other workspace projects. The console confirms no build errors at the time of capture.

---

## 📸 Simulation Screenshots

### 1. Normal Operation — Motor FORWARD (Temp > 25 °C)

The LM35 reads **64 °C**, exceeding the threshold. The motor runs forward and the LCD shows `Motor: FORWARD`.

![Motor running forward at 64°C](screenshots/Screenshot%20from%202026-05-15%2000-24-04.png)

---

### 2. Normal Operation — Motor STOPPED (Temp ≤ 25 °C)

The LM35 reads **16 °C**, below the threshold. The motor is stopped automatically.

![Motor stopped at 16°C](screenshots/Screenshot%20from%202026-05-15%2000-24-17.png)

---

### 3. Override Mode Active (Button Pressed)

Temperature is **52 °C** (would normally run motor), but the override button has been pressed. The LCD shows `Temp: 52C OVR` and `Motor: STOPPED`, confirming the button task preempted and forced the motor off.

![Override mode active at 52°C](screenshots/Screenshot%20from%202026-05-15%2000-24-23.png)

---

## 🛠️ How to Build & Flash

### Prerequisites

```bash
sudo apt install gcc-avr avr-libc avrdude
```

### Build

```bash
chmod +x build.sh
./build.sh
```

This produces `main.elf` and `main.hex`.

### Flash to Hardware

```bash
avrdude -c usbasp -p m168 -U flash:w:main.hex:i
```

### Simulate in SimulIDE

1. Open **SimulIDE 1.1.0-SR1**
2. Load `simulide/lab8.sim1`
3. The firmware path is pre-configured to `main.hex`
4. Press **▶ Run** — the LCD and motor will respond in real time

---

## 📡 UART Output

Connect a serial monitor at **9600 baud, 8N1** to observe real-time log output:

```
=== FreeRTOS Motor Control ===
[TEMP] 64 C
[MOTOR] Running FORWARD
[TEMP] 64 C
[BTN] Override ON  — motor forced OFF
[MOTOR] STOPPED
[TEMP] 52 C  (override active)
[BTN] Override OFF — sensor in control
[MOTOR] Running FORWARD
[TEMP] 16 C
[MOTOR] STOPPED
```

---

## 👨‍💻 Author

**Kareem** — Embedded Systems Lab, 2026

---

*Built with ❤️ using FreeRTOS, AVR-GCC, and SimulIDE*
