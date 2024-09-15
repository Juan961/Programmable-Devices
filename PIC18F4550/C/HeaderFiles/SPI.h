/*
 * Interface Two PIC18F4550 microcontroller 
 * for communicating with each other using SPI Protocol
 * http://www.electronicwings.com
 */

#include <xc.h>
#include "PICConfig.h"

#define CS LATA5

// SPI (Serial Peripheral Interface) es un protocolo de comunicación serial síncrona utilizado para intercambiar datos
// Por que no I2C que es maestro - esclavo
// 1. RST (Reset): Reiniciar la pantalla. 0 se reinicia, 1 se desactiva el reinicio
// 2. CE (Chip Enable): Cuando CE está en un nivel lógico bajo, la pantalla está activa y lista para recibir comandos o datos
// 3. DC (Data/Command): Selecciona si los datos enviadosson comandos o datos de pantalla. DC = 0: Comando. DC = 1: Información para ser mostrada.
// 4. Din (Data In): envían los datos desde el microcontrolador (maestro) hacia la pantalla (esclavo).
// 5. SCK (Serial Clock): Es la señal de reloj generada por el maestro que sincroniza la transferencia de datos entre el maestro y el esclavo. Cada pulso del reloj SCK permite que se transmita un bit de datos.
// 6, 7: VCC and BL (backlight): Positivo y enciende la retroiluminación de la pantalla, lo que facilita la lectura en condiciones de poca luz
// 8. GND: GROUUUUUUUUUUND

void SPI_Write(unsigned char);
void SPI_Init_Master();
unsigned char SPI_Read();
void MSdelay(unsigned int);

void SPI_Init_Master() {
    // PORT definition for SPI pins
    TRISBbits.TRISB0 = 1; // RB0 as input(SDI)
    TRISBbits.TRISB1 = 0; // RB1 as output(SCK)
    TRISAbits.TRISA5 = 0; // RA5 as a output(SS')
    TRISCbits.TRISC7 = 0; // RC7 as output(SDO)

    /* To initialize SPI Communication configure following Register*/
    CS = 1;
    SSPSTAT=0x40;		/* Data change on rising edge of clk,BF=0*/
    SSPCON1=0x22;		/* Master mode,Serial enable,
				idle state low for clk, fosc/64 */ 
    PIR1bits.SSPIF=0;

    /* Disable the ADC channel which are on for multiplexed pin
    when used as an input */    
    ADCON0=0;			/* This is for de-multiplexed the SCL
				and SDI from analog pins*/
    ADCON1=0x0F;		/* This makes all pins as digital I/O */    
}

void SPI_Write(unsigned char x) {
    unsigned char data_flush;
    SSPBUF=x;			/* Copy data in SSBUF to transmit */

    while(!PIR1bits.SSPIF);	/* Wait for complete 1 byte transmission */
    PIR1bits.SSPIF=0;		/* Clear SSPIF flag */
    data_flush=SSPBUF;		/* Flush the data */
}

unsigned char SPI_Read() {
    SSPBUF=0xff;		/* Copy flush data in SSBUF */
    while(!PIR1bits.SSPIF);	/* Wait for complete 1 byte transmission */
    PIR1bits.SSPIF = 0;
    return(SSPBUF);		/* Return received data.*/
}

/*************************Delay Function****************************/
void MSdelay(unsigned int val)	/* Delay of 1 ms for 8MHz Freq. */
{
     unsigned int i,j;
        for(i=0;i<val;i++)
            for(j=0;j<165;j++);
}
