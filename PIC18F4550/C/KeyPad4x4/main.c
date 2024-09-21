#include <xc.h>
#include <stdio.h>
#include <string.h>

#include "PICConfig.h"
#include "SPI.h"
#include "NokiaFont.h"

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
    for(i=0;i<6;i++) {   
        for(j=0;j<84;j++)
            SPI_Write(0); // Write 0 to clear display
    }      
    CS = 1;
}

// ========================================
int read_a0(void) {
    int channel = 0;
    ADCON0 = (ADCON0 & 0b11000011)|((channel<<2) & 0b00111100);
    ADCON0 |= ((1<<ADON)|(1<<GO));
    while(ADCON0bits.GO_nDONE == 1);
    int digital = (ADRESH*256) | (ADRESL);
    return digital;
}
// ========================================

void main() {
    OSCCON = 0X72; // Set internal oscillator frequency, 8 MHz*/
    TRISD = 0; // Set PORT as output port*/
    SPI_Init_Master(); // Initialize SPI master*/
    Nokia_Init();  // Initialize Nokia 5110 display */    
    Nokia_Clear();

    TRISAbits.TRISA0 = 1; // Puerto A de entrada
    ADCON1 = 0b00001110;
    ADCON2 = 0b10010010;
    ADRESH = 0;
    ADRESL = 0;

    char text[] = "";

    while(1){
        MSdelay(400);

        Nokia_Clear();

        int read = read_a0();

        Nokia_PositionXY(0,0);
        char resolution[10];
        sprintf(resolution, "%d", read);
        Nokia_SendString(resolution);

        Nokia_PositionXY(0,1);
        float voltageValue = read * 5 / 1023;
        char voltage[10];
        sprintf(voltage, "%.4f", voltageValue);  // Convert the integer 'read' to a string
        Nokia_SendString(voltage);

        if (read > 1020) { strcat(text, "1"); }
        else if (read > 954) { strcat(text, "4"); }
        else if (read > 830) { strcat(text, "7"); }
        else if (read > 741) { strcat(text, "*"); }
        else if (read > 682) { strcat(text, "2"); }
        else if (read > 640) { strcat(text, "5"); }
        else if (read > 599) { strcat(text, "8"); }
        else if (read > 552) { strcat(text, "0"); }
        else if (read > 517) { strcat(text, "3"); }
        else if (read > 494) { strcat(text, "6"); }
        else if (read > 470) { strcat(text, "9"); }
        else if (read > 440) { strcat(text, "#"); }
        else if (read > 410) { // A
            int len = strlen(text);
            for ( int i = 0; i < len; i++ ) {
                text[i] = '\0';
            }
        }
        else if (read > 390) { // B
            int len = strlen(text);
            
            if ( len != 0 ) {
                unsigned char lastChar = strlen(text) - 1; text[lastChar] = '\0';
            }
        }
        else if (read > 375) { strcat(text, "C"); }
        else if (read > 188) { strcat(text, "D"); }

        Nokia_PositionXY(0, 2);
        Nokia_SendString(text);
    };
}
