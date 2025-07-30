#include <msp430.h>
#include <stdint.h>

static uint16_t red_new_duty = 0;
static uint8_t red_pending_update = 0;

static uint16_t green_new_duty = 0;
static uint8_t green_pending_update = 0;

static uint16_t blue_new_duty = 0;
static uint8_t blue_pending_update = 0;

static uint16_t count = 0;

volatile uint8_t joystick_sample_flag = 0;

uint8_t check_and_clear_joystick_flag(void) {
    uint16_t gie = __get_SR_register() & GIE;
    __disable_interrupt();

    uint8_t flag = joystick_sample_flag;
    joystick_sample_flag = 0;

    __bis_SR_register(gie);
    return flag;
}





void Init_timer20ms_sampling(void) {
    TA1CCTL0 = CCIE;                     // Enable interrupt
    TA1CCR0 = 9999;                     // 40ms at 1 MHz (SMCLK/8)
    TA1CTL = TASSEL_2 | ID_3 | MC_1 | TACLR;  // SMCLK, /8, Up mode, clear timer
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR(void) {
    joystick_sample_flag = 1;
    if(count == 50) {
        P1OUT ^= BIT0;
        count = 0;
    }
    count = count + 1;
}

/*
void Init_timer20ms_sampling(void) {
    TA0CCTL0 = CCIE;                      // Enable interrupt for CCR0
    TA0CCR0 = 39999;
    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR;  // SMCLK, /8 divider, Up mode
}




#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR(void) {
    joystick_sample_flag = 1;
    if(count == 50) {
        P1OUT ^= BIT0;
        count = 0;
    }
    count = count + 1;
}
*/





/*

void init_pwm_red(void) {
    // Timer2_A for Red LED on P2.4 (TA2.1)
    P2DIR |= BIT4;
    P2SEL |= BIT4;

    TA2CCR0 = 1249;                       // Period for 800 Hz PWM (1.25ms)
    TA2CCTL1 = OUTMOD_7;                  // Reset/Set mode
    TA2CCR1 = 0;                         // Start with LED off
    TA2CTL = TASSEL_2 | ID_3 | MC_1 | TACLR;  // SMCLK, /8, Up mode, clear timer
}

void init_pwm_green_blue() {
    // Timer0_A - green and blue LED PWM on P1.5 (TA0.4) and P1.4 (TA0.3)
    TA0CTL = TASSEL_2 | ID_3 | MC_1 | TACLR; // SMCLK/8, Up mode, clear timer
    TA0CCR0 = 1249; // Same PWM period (~800Hz)

    TA0CCR3 = 600;  // Green duty cycle (~48%) on P1.4 (TA0.3)
    TA0CCTL3 = OUTMOD_7; // Reset/Set PWM

    TA0CCR4 = 600;  // Blue duty cycle (~48%) on P1.5 (TA0.4)
    TA0CCTL4 = OUTMOD_7; // Reset/Set PWM

    P1DIR |= BIT4 | BIT5;   // P1.4 and P1.5 outputs
    P1SEL |= BIT4 | BIT5;   // Select TA0.3 and TA0.4 functions for P1.4 and P1.5
}






uint8_t pwm_red(int8_t duty) {
    if (duty > 100 || duty < 0) return 1;

    if (duty < 5) {
        TA2CCTL1 = 0;   // Off
        TA2CCTL1 &= ~OUT;
    } else if (duty > 95) {
        TA2CCTL1 = 0;
        TA2CCTL1 |= OUT;  // Fully ON
    } else {
        red_new_duty = ((uint32_t)duty * TA2CCR0) / 100;
        red_pending_update = 1;
        TA2CCTL1 |= CCIE;   // Enable CCR1 interrupt for update
        TA2CCTL1 |= OUTMOD_7;
    }
    return 0;
}

uint8_t pwm_green(int8_t duty) {
    if (duty > 100 || duty < 0) return 1;

    if (duty < 5) {
        TA0CCTL2 = 0;   // Off
        TA0CCTL2 &= ~OUT;
    } else if (duty > 95) {
        TA0CCTL2 = 0;
        TA0CCTL2 |= OUT;  // Fully ON
    } else {
        green_new_duty = ((uint32_t)duty * TA0CCR0) / 100;
        green_pending_update = 1;
        TA0CCTL2 |= CCIE;  // Enable CCR2 interrupt for update
        TA0CCTL2 |= OUTMOD_7;
    }
    return 0;
}

uint8_t pwm_blue(int8_t duty) {
    if (duty > 100 || duty < 0) return 1;

    if (duty < 5) {
        TA0CCTL1 = 0;   // Off
        TA0CCTL1 &= ~OUT;
    } else if (duty > 95) {
        TA0CCTL1 = 0;
        TA0CCTL1 |= OUT;  // Fully ON
    } else {
        blue_new_duty = ((uint32_t)duty * TA0CCR0) / 100;
        blue_pending_update = 1;
        TA0CCTL1 |= CCIE;  // Enable CCR1 interrupt for update
        TA0CCTL1 |= OUTMOD_7;
    }
    return 0;
}

// Timer2_A ISR for Red LED CCR1 update
#pragma vector = TIMER2_A1_VECTOR
__interrupt void Timer2_A1_ISR(void) {
    switch(__even_in_range(TA2IV, TA2IV_TAIFG)) {
        case TA2IV_NONE: break;
        case TA2IV_TACCR1:
            if (red_pending_update) {
                TA2CCR1 = red_new_duty;
                red_pending_update = 0;
                TA2CCTL1 &= ~CCIE;
            }
            break;
        case TA2IV_TAIFG: break;
        default: break;
    }
}

// Timer0_A ISR for Blue (CCR4) and Green (CCR3) updates
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void) {
    switch(__even_in_range(TA0IV, TA0IV_TAIFG)) {
        case TA0IV_NONE: break;
        case TA0IV_TACCR1:
            if (blue_pending_update) {
                TA0CCR4 = blue_new_duty;
                blue_pending_update = 0;
                TA0CCTL4 &= ~CCIE;
            }
            break;
        case TA0IV_TACCR2:
            if (green_pending_update) {
                TA0CCR3 = green_new_duty;
                green_pending_update = 0;
                TA0CCTL3 &= ~CCIE;
            }
            break;
        case TA0IV_TAIFG: break;
        default: break;
    }
}


*/
