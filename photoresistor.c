#include "util.h"
#include "display.h"
#include "photoresistor.h"


void init_photoresistor() {
  *AT91C_PMC_PCER = (1 << 11); // Start the peripheral clock for port A
  *AT91C_PMC_PCER1 = (1 << 5); // Start the peripheral clock for ADC Controller, id = 37
  
  *AT91C_PIOA_PER = (PA3 | PA4); // Enable PIO for PA3/4
  // *AT91C_PIOB_PPUDR = (PA3 & PA4); // Disable the internal pull up resistor for PA3/4
  *AT91C_PIOA_ODR = (PA3 | PA4); // Enable input for PA3/4
  *AT91C_ADCC_WPMR = (0x414443 << 8); // Disable write protect in order to write in MR
  *AT91C_ADCC_MR = ((0x02 << 8) | // PRESCAL = 2, ADCClock = MCK / 14MHz
                    (0x07 << 24) | // TRACKTIM = 0x07, Tracking Time = (7 + 1) * ADCClock periods
                      (0x02 << 28)); // Transfer Period = (2 * 2 + 3) ADCClock periods
}

int get_lux_3() {
  *AT91C_ADCC_CHER = CHANNEL3;
  *AT91C_ADCC_CR = (1 << 1); // ADC Control Register
  // while(!ADC_CH3_DRDY); // Wait until data ready
  while(!ADC_DRDY); // Wait until data ready
  *AT91C_ADCC_CHDR = CHANNEL3;
  // return *AT91C_ADCC_CDR3 & 0xfff;
  return *AT91C_ADCC_LCDR & 0xfff;
}

int get_lux_4() {
  *AT91C_ADCC_CHER = CHANNEL4;
  *AT91C_ADCC_CR = (1 << 1); // ADC Control Register
  // while(!ADC_CH4_DRDY); // Wait until data ready
  while(!ADC_DRDY); // Wait until data ready
  *AT91C_ADCC_CHDR = CHANNEL4;
  // return *AT91C_ADCC_CDR4 & 0xfff;
  return *AT91C_ADCC_LCDR & 0xfff;
}

void lux_2_display() {
  delay(200000);
  write_data_2_display(0x3c); // low, thrid line on the screen
  write_data_2_display(0x00); // high
  write_command_2_display(0x24); // Set address pointer
  int lux3 = get_lux_3();
  char* lux3_str = lux_2_str(lux3);
  char* lux3_prompt = "The illuminance of AN3: ";
  write_str_2_display(lux3_prompt);
  if (lux3_str == NULL) {
    write_str_2_display("Memory allocation failed!");
  } else {
    write_str_2_display(lux3_str);
    write_str_2_display("  "); // Make sure to clean trailing number
    free(lux3_str);
  }
  delay(5000);
  int lux4 = get_lux_4();
  char* lux4_str = lux_2_str(lux4);
  write_data_2_display(0x5a); // low, fourth line on the screen
  write_data_2_display(0x00); // high
  write_command_2_display(0x24); // Set address pointers
  char* lux4_prompt = "The illuminance of AN4: ";
  write_str_2_display(lux4_prompt);
  if (lux4_str == NULL) {
    write_str_2_display("Memory allocation failed!");
  } else {
    write_str_2_display(lux4_str);
    write_str_2_display("  "); // Make sure to clean trailing number
    free(lux4_str);
  }
}

char* lux_2_str(int lux) {
  return int_2_str(lux);
}