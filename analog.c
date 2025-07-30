#include <msp430.h>
#include "analog.h"

void Init_ADC_joystick(void) {
    ADC12CTL0 &= ~ADC12ENC;         // Disable ADC for config

    ADC12CTL0 = ADC12SHT0_7 | ADC12ON;  // Sample time 128 cycles, ADC ON
    ADC12CTL1 = ADC12SHP;

    ADC12CTL2 = ADC12RES_2;              // 12-bit resolution

    P6SEL |= BIT5 | BIT3;          // Enable ADC function on P6.5 (A6), P6.3 (A3)
    P6DIR &= ~(BIT5 | BIT3);       // Set pins as input

    ADC12CTL0 |= ADC12ENC;         // Enable ADC
}

uint16_t Capture_jx(void) {
    ADC12CTL0 &= ~ADC12ENC;              // Disable ADC before changing MCTL
    ADC12MCTL0 = ADC12INCH_5;           //  (P6.5) X
    ADC12CTL0 |= ADC12ENC;               // Enable ADC
    ADC12IFG &= ~ADC12IFG0;              // Clear interrupt flag
    ADC12CTL0 |= ADC12SC;                // Start conversion

    while (!(ADC12IFG & ADC12IFG0));    // Wait for conversion complete
    return ADC12MEM0;
}

uint16_t Capture_jy(void) {
    ADC12CTL0 &= ~ADC12ENC;              // Disable ADC before changing MCTL
    ADC12MCTL0 = ADC12INCH_3;            // (P6.3) Y
    ADC12CTL0 |= ADC12ENC;               // Enable ADC
    ADC12IFG &= ~ADC12IFG0;              // Clear interrupt flag
    ADC12CTL0 |= ADC12SC;                // Start conversion

    while (!(ADC12IFG & ADC12IFG0));    // Wait for conversion complete
    return ADC12MEM0;
}



float convert_to_volts(uint16_t data) {
    return (3.3f * data) / 4095.0f;
}







void Init_ADC12(void) {
    // Disable ADC before configuring
    ADC12CTL0 &= ~ADC12ENC;

    // Turn on ADC12, SHT0 = 128 cycles
    ADC12CTL0 = ADC12SHT0_7 | ADC12ON;

    ADC12CTL1 = ADC12SHP;  // Pulse sample mode

    ADC12CTL2 = ADC12RES_2;  // 12-bit resolution

    // ADC12MCTL0: Vr+ = AVcc, Vr- = AVss (GND), INCH = 11
    ADC12MCTL0 = ADC12INCH_11;  // Channel A11

}

uint16_t Capture_11(void) {
    ADC12MCTL0 = ADC12INCH_11;

    ADC12CTL0 |= ADC12ENC;                 // Enable ADC
    ADC12IFG &= ~ADC12IFG0;                // Clear interrupt flag for MEM0
    ADC12CTL0 |= ADC12SC;                  // Start conversion

    while (!(ADC12IFG & BIT0));

    return ADC12MEM0;
}








void Init_adc12_joystick(void) {
    ADC12CTL0 &= ~ADC12ENC;                  // Disable ADC before configuration

    ADC12CTL0 = ADC12SHT0_7 | ADC12ON;      // Sample time 128 cycles, ADC ON
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;   // Pulse sample mode, sequence-of-channels
    ADC12CTL2 = ADC12RES_2;                  // 12-bit resolution
    ADC12CTL0 |= ADC12MSC;                   // Enable multiple sample and conversion

    // Configure memory control registers:
    // ADC12MCTL0: Channel 5 (P6.5), Vref+ = AVcc, Vref- = AVss
    ADC12MCTL0 = ADC12INCH_5;

    // ADC12MCTL1: Channel 3 (P6.3), set end of sequence bit here
    ADC12MCTL1 = ADC12INCH_3;

    // Configure pins for ADC function
    P6SEL |= BIT5 | BIT3;
    P6DIR &= ~(BIT5 | BIT3);

    ADC12CTL0 |= ADC12ENC;                   // Enable ADC
}



void Capture_joystick(uint16_t *results) {
    ADC12CTL0 &= ~ADC12SC;                   // Clear start bit just in case

    ADC12IFG &= ~(BIT0 | BIT1);              // Clear interrupt flags for MEM0 and MEM1

    ADC12CTL0 |= ADC12SC;                    // Start conversion

    while (!(ADC12IFG & BIT1));              // Wait for end of sequence (MEM1 interrupt flag)

    results[0] = ADC12MEM0;                  // First channel result (X axis)
    results[1] = ADC12MEM1;                  // Second channel result (Y axis)
}








void Init_adc12_accelerometer(void) {
    ADC12CTL0 &= ~ADC12ENC;                  // Disable ADC during config

    ADC12CTL0 = ADC12SHT0_7 | ADC12ON;      // Sample-and-hold time 128, ADC ON
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1;   // Use sample timer, sequence-of-channels
    ADC12CTL2 = ADC12RES_2;                  // 12-bit resolution
    ADC12CTL0 |= ADC12MSC;                   // Multiple sample and conversion

    // Configure MCTL registers for accelerometer channels
    ADC12MCTL2 = ADC12INCH_0;               // A0 - Accel X
    ADC12MCTL3 = ADC12INCH_1;               // A1 - Accel Y
    ADC12MCTL4 = ADC12INCH_2 | ADC12EOS;   // A2 - Accel Z, End of sequence

    // Enable ADC function on pins P6.0, P6.1, P6.2
    P6SEL |= BIT0 | BIT1 | BIT2;
    P6DIR &= ~(BIT0 | BIT1 | BIT2);         // Set pins as inputs

    ADC12CTL0 |= ADC12ENC;                   // Enable ADC
}



void Capture_accelerometer(uint16_t *results) {

    ADC12CTL0 &= ~ADC12SC;

    ADC12IFG &= ~(BIT2 | BIT3 | BIT4);      // Clear interrupt flags for MEM2, MEM3, MEM4
    ADC12CTL0 |= ADC12SC;                    // Start conversion (single sequence)
    while (!(ADC12IFG & BIT4));              // Wait for end of sequence (MEM4)

    results[2] = ADC12MEM2;                  // Accel X
    results[3] = ADC12MEM3;                  // Accel Y
    results[4] = ADC12MEM4;                  // Accel Z
}




