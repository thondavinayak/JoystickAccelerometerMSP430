#include <msp430.h>
#include "analog.h"
#include "display.h"
#include "time.h"
#include "system.h"
#include <stdio.h>  // for sprintf
#include <stdint.h>


void float_to_string(float value, uint8_t *buffer) {
    int16_t integer_part = (int16_t)value;  // Use int16_t as required by itoa
    uint16_t decimal_part = (uint16_t)((value - integer_part) * 100 + 0.5f);  // 2 decimals, rounded

    itoa_7_null_terminated(integer_part, buffer);  // convert integer part to string

    // find end of string
    uint16_t i = 0;
    while (buffer[i] != '\0') i++;

    buffer[i++] = '.';
    buffer[i++] = (decimal_part / 10) + '0';   // tens digit of decimal
    buffer[i++] = (decimal_part % 10) + '0';   // ones digit of decimal
    buffer[i++] = 'V';
    buffer[i] = '\0';  // null terminator
}




int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    Init_clock();
    Init_display();
    Init_adc12_joystick();
    Init_timer20ms_sampling();
    Init_adc12_accelerometer();

    __enable_interrupt();

    // Configure select button (P1.6) as input with pull-up resistor
    P1DIR &= ~BIT6;    // Input
    P1REN |= BIT6;     // Enable pull-up/down resistor
    P1OUT |= BIT6;     // Pull-up resistor enabled


    char msgVoltX[14];
    char msgVoltY[14];
    uint16_t adc_values[5];
    float x_volt, y_volt;

    uint8_t msgaccX[7];
    uint8_t msgaccY[7];
    uint8_t msgaccZ[7];

    uint8_t rectX = 60;  // Start in the middle
    uint8_t rectY = 60;
    const uint8_t RECT_SIZE = 20;  // Width/height

    // Circle parameters (Accelerometer controlled)
    int circleX = 100, circleY = 100;
    const int CIRCLE_RADIUS = 10;

    int oldCircleX = 100;
    int oldCircleY = 100;


    // Initial screen setup
    Filled_rectangle(0, 127, 0, 127, GRAPHICS_COLOR_WHITE);
    write_string("Joystick XY", 20, 0, GRAPHICS_COLOR_BLUE);

    P1DIR |= BIT0;   // Set P1.0 as output
    P1OUT &= ~BIT0;  // Start with LED off

    Filled_rectangle(0, 128, 48, 50, GRAPHICS_COLOR_YELLOW);
    Filled_rectangle(0, 128, 121, 123, GRAPHICS_COLOR_YELLOW);
    Filled_rectangle(7, 9, 51, 120, GRAPHICS_COLOR_YELLOW);
    Filled_rectangle(121, 123, 51, 120, GRAPHICS_COLOR_YELLOW);


    while (1) {
        if(check_and_clear_joystick_flag()) {
            Capture_joystick(adc_values);  // joystick[0] = X, joystick[1] = Y
            x_volt = convert_to_volts(adc_values[0]);
            y_volt = convert_to_volts(adc_values[1]);

            int i;
            for (i = 0; i < sizeof(msgVoltX); i++) msgVoltX[i] = '\0';
            for (i = 0; i < sizeof(msgVoltY); i++) msgVoltY[i] = '\0';

            // Convert float voltage to string (e.g. "1.23V")
            float_to_string(x_volt, (uint8_t*)msgVoltX);
            float_to_string(y_volt, (uint8_t*)msgVoltY);

            // Display voltages
            write_string("Vx: ", 20, 10, GRAPHICS_COLOR_RED);
            write_string((uint8_t*)msgVoltX, 40, 10, GRAPHICS_COLOR_RED);

            write_string("Vy: ", 20, 20, GRAPHICS_COLOR_GREEN);
            write_string((uint8_t*)msgVoltY, 40, 20, GRAPHICS_COLOR_GREEN);

            write_string("                ", 20, 30, GRAPHICS_COLOR_BLACK); // clear old
            if (x_volt < 0.8f)
                write_string("LEFT", 20, 30, GRAPHICS_COLOR_BLACK);
            else if (x_volt > 2.5f)
                write_string("RIGHT", 20, 30, GRAPHICS_COLOR_BLACK);
            else if (y_volt < 0.8f)
                write_string("DOWN", 20, 30, GRAPHICS_COLOR_BLACK);
            else if (y_volt > 2.5f)
                write_string("UP", 20, 30, GRAPHICS_COLOR_BLACK);


            // Clear old rectangle before updating position
            Filled_rectangle(rectX, rectX + RECT_SIZE - 1, rectY, rectY + RECT_SIZE - 1, GRAPHICS_COLOR_WHITE);

            // Update position based on joystick voltage
            if (x_volt < 0.8f && rectX > 10)
                rectX -= 5;  // Move left
            else if (x_volt > 2.5f && rectX < 120 - RECT_SIZE)
                rectX += 5;  // Move right

            if (y_volt < 0.8f && rectY < 120 - RECT_SIZE)
                rectY += 5;  // Joystick down → move rectangle down (screen Y increases downward)
            else if (y_volt > 2.5f && rectY > 50)
                rectY -= 5;  // Joystick up → move rectangle up

            // Draw new rectangle at updated position
            Filled_rectangle(rectX, rectX + RECT_SIZE - 1, rectY, rectY + RECT_SIZE - 1, GRAPHICS_COLOR_RED);





           Capture_accelerometer(adc_values);

           itoa_7_null_terminated(adc_values[2], msgaccX);
           write_string("AccX:", 0, 40, GRAPHICS_COLOR_BLUE);
           write_string(msgaccX, 25, 40, GRAPHICS_COLOR_BLUE);

           itoa_7_null_terminated(adc_values[3], msgaccY);
           write_string("AccY:", 70, 40, GRAPHICS_COLOR_GREEN);
           write_string(msgaccY, 95, 40, GRAPHICS_COLOR_GREEN);

           //itoa_7_null_terminated(adc_values[4], msgaccZ);
           //write_string("AccZ:", 80, 40, GRAPHICS_COLOR_RED);
           //write_string(msgaccZ, 90, 40, GRAPHICS_COLOR_RED);

           int16_t accX = (int16_t)adc_values[2] - 2048;
           int16_t accY = (int16_t)adc_values[3] - 2048;

           // Threshold to avoid jitter
           int16_t threshold = 100;

           // Move circle based on accelX
           if (accX < -threshold && circleX > (10 + CIRCLE_RADIUS))
               circleX -= 3;
           else if (accX > threshold && circleX < (120 - CIRCLE_RADIUS))
               circleX += 3;

           // Move circle based on accelY (note screen y increases down)
           if (accY < -threshold && circleY < (120 - CIRCLE_RADIUS))
               circleY += 3;
           else if (accY > threshold && circleY > (50 + CIRCLE_RADIUS))
               circleY -= 3;


           int closestX = circleX;
           if (circleX < rectX) closestX = rectX;
           else if (circleX > rectX + RECT_SIZE - 1) closestX = rectX + RECT_SIZE - 1;

           int closestY = circleY;
           if (circleY < rectY) closestY = rectY;
           else if (circleY > rectY + RECT_SIZE - 1) closestY = rectY + RECT_SIZE - 1;

           int dx = circleX - closestX;
           int dy = circleY - closestY;

           if ((dx*dx + dy*dy) >= (CIRCLE_RADIUS * CIRCLE_RADIUS)) {
               Filled_circle(oldCircleX, oldCircleY, CIRCLE_RADIUS, GRAPHICS_COLOR_WHITE);
               // No overlap, safe to draw circle
               Filled_circle(circleX, circleY, CIRCLE_RADIUS, GRAPHICS_COLOR_BLUE);
               oldCircleX = circleX;
               oldCircleY = circleY;
           } else {
               //Filled_circle(oldCircleX, oldCircleY, CIRCLE_RADIUS, GRAPHICS_COLOR_BLUE);
               circleX = oldCircleX;
               circleY = oldCircleY;
           }



           // Draw new circle
           //Filled_circle(circleX, circleY, CIRCLE_RADIUS, GRAPHICS_COLOR_BLUE);

        }
    }

}
