/*
 * File:   main.c
 * Author: jose5
 *
 * Created on 20 de agosto de 2024, 12:17 PM
 */

#include <xc.h> // Librerias globales
#include "newxc8_header.h" // Librerias locales

#define vref 5.00 // Definir el voltaje referencia como 5V

void delay(int val) {
    for ( int i = 0; i < val; i++ ) {
        for ( int j = 0; j <= 165; j++ ) {  }
    }
}

int read_a7(void) {
    int channel = 0;

    // ADCON0 & 0b11000011: Modifica los 1 del registro ADCON
    // (channel << 2): Mueve el canal dos bits para ocupar los bits 5,4,3,2 de ADCON0 
    // (channel << 2) & 0b00111100: Modifica los canales 5,4,3,2 de ADCON0
    // Une los cambios de todos los pines
    ADCON0 = (ADCON0 & 0b11000011)|((channel<<2) & 0b00111100);

    // ADON = 1: Activa el módulo ADC. Posición 0 del registro ADCON0
    // GO = 1: Activa la conversión. Posición 1 del registro ADCON0
    ADCON0 |= ((1<<ADON)|(1<<GO));
    // =
    // ADCON0bits.ADON = 1;
    // ADCON0bits.GO = 1;

    // ADCON0bits.GO_nDONE: Registro que monitorea el estado del módulo ADC.
    // Cuando DONE = 0 ya terminó
    while(ADCON0bits.GO_nDONE == 1);

    // ADRESH: Almacena los dos bits más significativos de la operación
    // ADRESL: Almacena los ocho bits menos significativos de la operación
    // ADRESH*256: Mueve los dos bit más significativos 8 posiciones a la izquierda
    // (ADRESH*256) | (ADRESL): Combina los 2 bits más significativos con los menos en un binario
    int digital = (ADRESH*256) | (ADRESL);

    return digital;
}

void loop(void) {
    int read = read_a7();

    // 1023: 2^10 por que el resultado se va a almacenar en 10 bits
    int vin = read * 0.2499;

    PORTB = vin; // Escribir en Puerto B

    return;
}

void main(void) {
    // Configuración de puertos y frecuencia de oscilador
    TRISB = 0; // Puerto B de salida
    TRISA = 255; // Puerto A de entrada
    OSCCON = 0x72; // Trabajar con 8MHz _ 01110010

    // ADCON0: Controla la operación ADC
    // Bit 7-6 (00): No se usan
    // Bits 5-2 CHS3:CHS0: Define en que canal se va a hacer la conversión
    // Bit 1 GO/DONE: Define empezar la conversion (1)
    // Bit 0 ADON: Define si la operación de conversión sigue en curso (1) o ya se detuvo

    // Bit 7-6 (00): No se usan
    // Controla los voltajes de referencia
    // Bits 5-4 (00) VCFG1 - VCFG0: 0 Para usar referencia de tierra y no negativa, Vref 0 para el voltaje de alimentación del micro y no externa
    // Bits 3-0 (1110) PCFG3:PCFG0: Define que pines son inputs ADC y que otros Ditital I/O
    ADCON1 = 0b00001110;

    // Controla aspectos como la velocidad de conversión del ADC, el tiempo de adquisición (TAD), y cómo se justifica el resultado en los registros ADRESH y ADRESL.
    // Bit 7 (1) ADFM: Justificación Derecha (Right Justified). Todos los menos significativos se ponen en ADRESL
    // Bit 6 (0): No se usa
    // Bits 5-3 (010) ADCS2:ADCS0: Tiempo de Adquisición (ACQT). Indica un tiempo de adquisición de 4 TAD. Más tiempo para adquirir datos más resolución
    // Bits 2-0 (010) ACQT2:ACQT0: Divisor de Reloj del ADC (ADCS). Fuente de reloj para el ADC 010 corresponde a FOSC/32. Tiempo que pasa de conversión de la señal a digital
    ADCON2 = 0b10010010;

    // Limpia el registro donde se van a almacenar los bits de la operación
    ADRESH = 0;
    ADRESL = 0;

    // LOOP
    while (1) {
        loop();
    }

    return;
}

// ACQT: Tiempo de Adquisición = Es el período que el módulo ADC necesita para cargar
// completamente el condensador de muestreo interno antes de realizar la conversión de la señal analógica a digital.
// 4 TAD se refiere a cuatro ciclos del reloj

// ADCS: Divisor de Reloj del ADC = Define la fuente del reloj que alimenta al ADC
// Para un oscilador de 8Mhz es Fosc/32 = 8MHz / 32
