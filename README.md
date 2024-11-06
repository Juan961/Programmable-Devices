# 🤖 Programmable Devices

This repository contains various projects for ESP32 and PIC18F4550 microcontrollers. The projects are organized into different directories based on the microcontroller and the functionality.

## 🖲️ PIC18F4550 Projects

Some of the code to integrate the screen (Nokia 5110) and manage the ADC module of the PIC are based on the tutorials of [Electronic Wings](https://www.electronicwings.com/pic)

### Assembler

- [Delay/main.asm](PIC18F4550/Assembler/Delay/main.asm): Assembly code for creating delays.
- [LEDs/main.asm](PIC18F4550/Assembler/LEDs/main.asm): Assembly code for controlling LEDs.

### C

- [Exam1/main.c](PIC18F4550/C/Exam1/main.c): Main file for the Exam1 project.
- [HeaderFiles/NokiaFont.h](PIC18F4550/C/HeaderFiles/NokiaFont.h): Header file for Nokia font.
- [HeaderFiles/PicConfig.h](PIC18F4550/C/HeaderFiles/PicConfig.h): Header file for PIC configuration.
- [HeaderFiles/SPI.h](PIC18F4550/C/HeaderFiles/SPI.h): Header file for SPI communication.
- [KeyPad4x4/main.c](PIC18F4550/C/KeyPad4x4/main.c): Main file for the KeyPad4x4 project.
- [KeyPad4x4SingleCalculator/main.c](PIC18F4550/C/KeyPad4x4SingleCalculator/main.c): Main file for the KeyPad4x4 Single Calculator project.
- [MoveLetterJoyStick/main.c](PIC18F4550/C/MoveLetterJoyStick/main.c): Main file for the Move Letter Joystick project.
- [ReadADC/main.c](PIC18F4550/C/ReadADC/main.c): Main file for the Read ADC project.
- [TextPositionBasedOnJoystick/main.c](PIC18F4550/C/TextPositionBasedOnJoystick/main.c): Main file for the Text Position Based on Joystick project.
- [VirtualKeyboardJoyStick/main.c](PIC18F4550/C/VirtualKeyboardJoyStick/main.c): Main file for the Virtual Keyboard Joystick project.
- [VoltageADC/main.c](PIC18F4550/C/VoltageADC/main.c): Main file for the Voltage ADC project.
- [XYJoyStickValues/main.c](PIC18F4550/C/XYJoyStickValues/main.c): Main file for the XY Joystick Values project.


## 💽 ESP32 Projects

### Control

- [encoder.c](ESP32/Control/encoder.c): Handles the encoder functionality, including calculating velocity and handling Bluetooth communication.
- [servo.c](ESP32/Control/servo.c): Controls a servo motor using PWM signals and handles Bluetooth communication.
- [timer.c](ESP32/Control/timer.c): Manages timer interrupts and handles Bluetooth communication.

### Exam2

- [main.c](ESP32/Exam2/main.c): Contains the main logic for the Exam2 project, including MPU6050 sensor data processing and servo control.

### PWM

- [main.c](ESP32/PWM/main.c): Main file for the PWM project, setting up and controlling PWM signals.
- [servo.c](ESP32/PWM/servo.c): Controls a servo motor using PWM signals.
