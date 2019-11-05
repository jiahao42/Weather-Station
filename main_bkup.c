#include "system_sam3x.h"
#include "at91sam3x8.h"

#define AT91C_PIOD_AIMER (AT91_CAST(AT91_REG *) 	0x400E14B0)
#define AT91C_PIOC_AIMER (AT91_CAST(AT91_REG *) 	0x400E12B0)
#define AT91C_PIOD_DIFSR (AT91_CAST(AT91_REG *) 	0x400E1484)
#define AT91C_PIOC_DIFSR (AT91_CAST(AT91_REG *)         0x400E1284)
#define AT91C_PIOD_SCDR (AT91_CAST(AT91_REG *) 	        0x400E148C)
#define AT91C_PIOC_SCDR (AT91_CAST(AT91_REG *) 	        0x400E128C)
#define PIO_ISR         (AT91_CAST(AT91_REG *) 	        0x0000004C) // (PIO_ISR) Interrupt Status Register
#define AT91C_PIOD_IER (AT91_CAST(AT91_REG *) 	        0x400E1440)
#define AT91C_PIOC_IER (AT91_CAST(AT91_REG *) 	        0x400E1240)

#define TURN_ON_LED (*AT91C_PIOD_SODR = (1 << 3))
#define TURN_OFF_LED (*AT91C_PIOD_CODR = (1 << 3))

static volatile uint32_t ms_ticks = 0;
static volatile int tick_1000ms_elapsed = 0;
void SysTick_Handler(void){
  ms_ticks++;

  if (ms_ticks % 1000 == 0) {
    tick_1000ms_elapsed = 1;
  }
}

static volatile unsigned int if_interrupt = 0;
void PIOC_Handler(void) {
  if (*AT91C_PIOC_ISR) {
    if_interrupt = 1;
  }
}

static unsigned int col = 0x380; // Column: PC7/8/9
static unsigned int row = 0x3c; // Row: PC2/3/4/5
int read_key() {
  
  *AT91C_PIOD_CODR = (1 << 2); // Clear OE Key bus
  
  *AT91C_PIOC_SODR = col; // Set as high
  
  static int col_pins[3] = {7, 8, 9};
  static int row_pins[4] = {2, 3, 4, 5};
  int i, j, val = 0;
  for (i = 0; i < 3; i++) {
    *AT91C_PIOC_CODR = (1 << col_pins[i]); // Clear one column
    for (j = 0; j < 4; j++) {
      unsigned int pc = *AT91C_PIOC_PDSR;
      unsigned int pd = *AT91C_PIOC_ODSR;
      unsigned int pe = *AT91C_PIOC_OSR;
      pc = pc + pd + pe;
      unsigned int row_status = *AT91C_PIOC_OSR;
      if ((row_status & (1 << row_pins[j])) == 0)
        val = j * 3 + i + 1;
    }
    *AT91C_PIOC_SODR = (1 << col_pins[i]); // Set the column again
  }
  *AT91C_PIOC_ODR = col; // Disable output
  
  return val;
}

int main(void){
  SystemInit();
  // Setup SysTick Timer for 1ms interrupts
  if (SysTick_Config(SystemCoreClock / 1000)) { // 84 MHz
    while (1) {} // If fail
  }
  
  *AT91C_PMC_PCER = ((1 << 14) | (1 << 13)); // Start the peripheral clock for port C & D
  
  // for OE
  *AT91C_PIOD_PER = (1 << 2); // Enable PIO for PD2
  *AT91C_PIOD_PPUDR = (1 << 2); // Disable the internal pull up resistor for PD2
  *AT91C_PIOD_OER = (1 << 2); // Enable output for PD2
  
  *AT91C_PIOC_PER = (row|col); // Enable PIO for keypad
  *AT91C_PIOC_PPUDR = (row|col); // Disable the internal pull up resistor for keypad
  *AT91C_PIOC_OER = row; // Enable output for row
  *AT91C_PIOC_ODR = col; // Disable output for column

  while(1){
    int val = 0;
    while ((val = read_key()) == 0);
    val += val;
  }
}