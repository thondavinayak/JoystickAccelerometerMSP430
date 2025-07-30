#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>

void Init_ADC_joystick(void);
uint16_t Capture_jx(void);
uint16_t Capture_jy(void);

void Init_ADC12(void);
uint16_t Capture_11(void);

float convert_to_volts(uint16_t data);

void Init_adc12_joystick(void);
void Capture_joystick(uint16_t *results);

void Capture_accelerometer(uint16_t *results);
void Init_adc12_accelerometer(void);

#endif
