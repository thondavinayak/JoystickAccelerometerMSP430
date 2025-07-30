🎮 MSP430 Interactive Joystick and Accelerometer Demo
This project is a real-time graphical demo for the MSP430F5529 LaunchPad. It uses an analog joystick and a 3-axis accelerometer to control on-screen shapes drawn on a connected graphical LCD.

🧠 Features
🕹️ Joystick-controlled rectangle:

Move a red rectangle on-screen using the X and Y voltages from a joystick.

Display real-time voltages and direction (LEFT/RIGHT/UP/DOWN) on the screen.

🧭 Accelerometer-controlled circle:

Use X and Y accelerometer data to move a blue circle.

Avoid collision: if the circle overlaps the joystick-controlled rectangle, it freezes in place.

📟 Display integration:

Outputs are shown on a 128x128 graphical LCD.

All drawing is handled via simple graphics primitives like Filled_rectangle and Filled_circle.

⏲️ 20 ms periodic sampling:

Uses a timer interrupt to set a flag for ADC sampling every 20 ms, providing smooth updates.

🔋 Voltage readings:

Shows X and Y joystick voltages in volts (e.g. 1.23V).

Also prints raw accelerometer ADC values for X and Y axes.

📁 File Overview
File	Description
main.c	Main application loop: handles input sampling, graphics updates, voltage display, and collision logic.
analog.c	ADC initialization and data capture for joystick and accelerometer (12-bit precision).
time.c	Timer setup and ISR for 20 ms sampling loop (drives joystick sampling flag).
drawings.c	Contains simple drawing routines to clear or draw rectangles/circles.

🧰 Hardware Requirements
MSP430F5529 LaunchPad

Joystick module connected to analog inputs (P6.3 and P6.5)

3-axis analog accelerometer connected to P6.0, P6.1, and P6.2

128x128 pixel graphical LCD

Button on P1.6 (optional, configured with pull-up)

🚀 How It Works
Every 20 ms, a timer ISR sets a flag.

In the main loop:

Joystick voltages are sampled and converted to X/Y movement.

The rectangle moves accordingly and voltages are printed.

Accelerometer X/Y values determine the movement of a circle.

Collision is checked: if the circle overlaps the rectangle, it is stopped.

All graphics are drawn with custom functions like Filled_rectangle() and write_string().
