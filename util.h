#ifndef UTIL_H_
#define UTIL_H_

#include "config.h"
#include "at91sam3x8.h"
#include "system_sam3x.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>
#include <math.h>

#define AT91C_PIOD_AIMER (AT91_CAST(AT91_REG *) 	0x400E14B0)
#define AT91C_PIOC_AIMER (AT91_CAST(AT91_REG *) 	0x400E12B0)
#define AT91C_PIOD_DIFSR (AT91_CAST(AT91_REG *) 	0x400E1484)
#define AT91C_PIOC_DIFSR (AT91_CAST(AT91_REG *)         0x400E1284)
#define AT91C_PIOD_SCDR (AT91_CAST(AT91_REG *) 	        0x400E148C)
#define AT91C_PIOC_SCDR (AT91_CAST(AT91_REG *) 	        0x400E128C)
#define PIO_ISR         (AT91_CAST(AT91_REG *) 	        0x0000004C) // (PIO_ISR) Interrupt Status Register
#define AT91C_PIOD_IER (AT91_CAST(AT91_REG *) 	        0x400E1440)
#define AT91C_PIOC_IER (AT91_CAST(AT91_REG *) 	        0x400E1240)
#define AT91C_ADCC_WPSR (AT91_CAST(AT91_REG *)          0x400C00E8)
#define AT91C_ADCC_WPMR (AT91_CAST(AT91_REG *)          0x400C00E4)
#define AT91C_PIOB_WPMR (AT91_CAST(AT91_REG *)          0x400E10E4)
#define AT91C_PIOB_WPSR (AT91_CAST(AT91_REG *)          0x400E10E8)
#define AT91C_TC_WPMR   (AT91_CAST(AT91_REG *)          0x400800E4)

typedef unsigned int uint;
typedef unsigned char uchar;

extern const char* memory_allocation_error;

extern volatile uint ms_ticks;
extern volatile uint tick_1s_elapsed;
void SysTick_Handler(void);

extern volatile unsigned int if_interrupt;
extern volatile float temp;
void TC0_Handler(void);
void PIOB_Handler(void);

void delay(int value);
char* int_2_str(int num);
int appro_equal(float f);

extern uint time_factor;


#endif