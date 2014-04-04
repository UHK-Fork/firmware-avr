#ifndef __UHK_LEFT_H__
#define __UHK_LEFT_H__

    /* Includes: */
        #include <avr/io.h>
        #include <avr/interrupt.h>
        #include <avr/wdt.h>
        #include <util/delay.h>
        #include <stdbool.h>
        #include "KeyMatrix.h"
        #include "USART.h"
        #include "ShiftRegister.h"
        #include "PCA9634.h"
        #include "LedMatrix.h"

    /* Macros: */
        #define KEY_MATRIX_SHIFT_REGISTER_COL_ID 2
        #define SHIFT_REGISTER_KEY_MATRIX_DRAIN_ID 7

#endif
