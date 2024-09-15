/*
 * File:   main.c
 * Author: jose5
 *
 * Created on 27 de agosto de 2024, 11:47 AM
 */

#include <xc.h>
#include "PICConfig.h"
#include "SPI.h"
#include "NokiaFont.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DC LATD0
#define RES LATD1 // Connected to reset

void Nokia_Init();
void Nokia_SendCommand(char);
void Nokia_SendData(char);
void Nokia_SendString(char *);
void MSdelay(unsigned int);
void Nokia_Clear();
void Nokia_PositionXY(char, char);

void Nokia_SendCommand(char cmd) {
    DC = 0; // Data/Command pin, D/C=1 - Data, D/C = 0 - Command
    CS = 0; // enable chip
    SPI_Write(cmd); // write command to the Nokia 5110
    CS = 1; // Disable chip
}

void Nokia_SendData(char dat) {
    char i;
    DC = 1; // Data/Command pin, D/C=1 - Data, D/C = 0 - Command
    CS = 0; // Enable chip
    for(i=0;i<5;i++)
        SPI_Write(font[(dat) - (0x20)][i]); // Write data to the Nokia 5110
    CS = 1; // Disable chip
}

void Nokia_SendString(char *data) {
    char i;

    while((*data)!=0) {
        Nokia_SendData(*data);
        data++;
    }
}

void Nokia_PositionXY(char X, char Y) {
    Nokia_SendCommand(0x80 | X); // Set X position
    Nokia_SendCommand(0x40 | Y); // Set Y position
}

void Nokia_Init() {
    // Apply 100 ms reset(low to high) pulse
    RES = 0;  // Enable reset
    MSdelay(100);
    RES = 1;  // Disable reset
    Nokia_SendCommand(0x21); // Display extended commands
    Nokia_SendCommand(0x13); // Select Bias voltage
    Nokia_SendCommand(0x07); // Set temperature coefficient
    Nokia_SendCommand(0xC0); // Set LCD Vop for contrast
    Nokia_SendCommand(0x20); // Display basic commands
    Nokia_SendCommand(0x0C); // Set normal mode
}

void Nokia_Clear() {
    char i,j;
    CS = 0;
    DC = 1;
    for(i=0;i<6;i++) { // 6 filas para la font seleccionada, cada letra ocupa 8 pixeles 8 * 6 = 48 pixeles de altura
        for(j=0;j<84;j++) // 84 columnas de pixeles por cada fila
            SPI_Write(0); // Write 0 to clear display
    }
    CS = 1;
}

// ========================================
int read_x(void) {
    int channel = 0;
    ADCON0 = (ADCON0 & 0b11000011)|((channel<<2) & 0b00111100);
    ADCON0 |= ((1<<ADON)|(1<<GO));
    while(ADCON0bits.GO_nDONE == 1);
    int digital = (ADRESH*256) | (ADRESL);
    return digital;
}
int read_y(void) {
    int channel = 1;
    ADCON0 = (ADCON0 & 0b11000011)|((channel<<2) & 0b00111100);
    ADCON0 |= ((1<<ADON)|(1<<GO));
    while(ADCON0bits.GO_nDONE == 1);
    int digital = (ADRESH*256) | (ADRESL);
    return digital;
}
// ========================================

int map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (int)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

const double easyMap[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10};

void main() {
    OSCCON = 0X72; // Set internal oscillator frequency 8 MHz
    TRISD = 0; // Set PORT as output port*/
    SPI_Init_Master(); // Initialize SPI master
    Nokia_Init();  // Initialize Nokia 5110 display

    TRISAbits.TRISA0 = 1; // Puerto A0 de entrada
    TRISAbits.TRISA1 = 1; // Puerto A1 de entrada

    ADCON1 = 0b00001101; // Configurar ADCON1
    ADCON2 = 0b10010010; // Configurar ADCON2
    ADRESH = 0; // Reset ADRESH
    ADRESL = 0; // Reset ADRESL

    while(1){
        MSdelay(300);
        
        char message1[10] = "X: ";
        char message2[10] = "Y: ";

        double x = easyMap[map( read_x(), 0, 1024, 0, 21 )];
        double y = map( read_y(), 0, 1024, 0, 21 );

        char number[10];

        Nokia_Clear(); // Clear Nokia display

        // ============================== Pimer punto
        char x_str[10];
        sprintf(x_str, "%.1f", x);  // Convert the integer 'read' to a string

        char y_str[10];
        sprintf(y_str, "%.1f", y);  // Convert the integer 'read' to a string

        strcat(message1, x_str);
        strcat(message2, y_str);

        Nokia_PositionXY(0, 0);  // Set X and Y position for printing
        Nokia_SendString(message1);
        Nokia_PositionXY(0, 1);  // Set X and Y position for printing
        Nokia_SendString(message2);

        double num = x * y;
        int numInt = num;
        double rounded = round(num * 10) / 10;

        sprintf(number, "%.1f", rounded);  // Convert the integer 'read' to a string

        Nokia_PositionXY(0, 2);  // Set X and Y position for printing
        Nokia_SendString(number);

        // ============================== Segundo punto
        Nokia_PositionXY(0, 3);  // Set X and Y position for printing
        if ( numInt % 2 == 0 ) {
            Nokia_SendString("Par");
        } else {
            Nokia_SendString("Impar");     
        }

        // ============================== Tercer punto
        Nokia_PositionXY(0, 4);  // Set X and Y position for printing
        if ( numInt == 0  ) {
            Nokia_SendString("Es 0 no joda");
        } else if ( numInt % 3 == 0 && numInt % 5 == 0 ) {
            Nokia_SendString("micros");
        } else if ( numInt % 3 == 0) {
            Nokia_SendString("usb");     
        } else if ( numInt % 5 == 0) {
            Nokia_SendString("bog");     
        }

        // ============================== Cuarto punto
        Nokia_PositionXY(0, 5);  // Set X and Y position for printing

        int prime = 1;
        
        for ( int i = 2; i < numInt; i++ ) {
            if ( numInt % i == 0 ) {
                prime = 0;
                break;
            }
        }

        if ( numInt == 0) {
            Nokia_SendString("Es 0 no joda");
        } else if ( prime == 1 ) {
            Nokia_SendString("Primo");
        } else {
            Nokia_SendString("No primo");
        }
    };
}
