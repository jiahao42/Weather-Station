#include "util.h"
#include "display.h"
#include "keypad.h"
#include "servo.h"

const uint col = 0x380; // Column: PC7/8/9
const uint row = 0x3c; // Row: PC2/3/4/5
uchar key_value = '*';

void init_keypad() {
  *AT91C_PMC_PCER = ((1 << 14) | (1 << 13)); // Start the peripheral clock for port C & D

  // for OE of keypad
  *AT91C_PIOD_PER = (1 << 2); // Enable PIO for PD2
  *AT91C_PIOD_PPUDR = (1 << 2); // Disable the internal pull up resistor for PD2
  *AT91C_PIOD_OER = (1 << 2); // Enable output for PD2
  
  // for keypad
  *AT91C_PIOC_PER = (col|row); // Enable PIO for keypad
  *AT91C_PIOC_PPUDR = (col|row); // Disable the internal pull up resistor for keypad
  *AT91C_PIOC_ODR = (col|row); // Disable output for keypad
  
}
uchar read_key() {
  *AT91C_PIOD_CODR = (1 << 2); // Clear OE Key bus
  
  *AT91C_PIOC_OER = col; // Enable output for column
  *AT91C_PIOC_SODR = col; // Set as high
  
  static const int col_pins[3] = {7, 8, 9};
  static const int row_pins[4] = {2, 3, 4, 5};
  int i, j, val = 0;
  for (i = 0; i < 3; i++) {
    *AT91C_PIOC_CODR = (1 << col_pins[i]); // Clear one column
    for (j = 0; j < 4; j++) {
      uint row_status = *AT91C_PIOC_PDSR;
      if ((row_status & (1 << row_pins[j])) == 0) {
        val = j * 3 + i + 1;
        if (val == 10) return '*';
        else if (val == 11) return '0';
        else if (val == 12) return '#';
        else return val + '0';
      }
    }
    *AT91C_PIOC_SODR = (1 << col_pins[i]); // Set the column again
  }
  *AT91C_PIOC_ODR = col; // Disable output

  return 0;
}

void keypad_2_display() {
  delay(2000000);
  write_data_2_display(0x10); // low 
  write_data_2_display(0x00); // high
  write_command_2_display(0x24); // Set address pointer
  unsigned char val = 0x00;
  while ((val = read_key()) == 0x00); // wait for input
  write_char_2_display(val);
}