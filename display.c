#include "util.h"
#include "keypad.h"
#include "temp.h"
#include "photoresistor.h"
#include "servo.h"
#include "display.h"
#include "graph.h"
#include "pressure.h"

const uint databus = 0x3fc; // databus PC2-9
uint current_mode;

uchar read_status_display() {
  unsigned char temp;
  *AT91C_PIOC_ODR = databus; // Make databus as input
  SET_DIR_INPUT;
  ENABLE_CHIP;
  *AT91C_PIOC_SODR = CD; // Set C/D
  *AT91C_PIOC_CODR = CE; // Clear chip select display
  *AT91C_PIOC_SODR = WR; // Set write display
  *AT91C_PIOC_CODR = RD; // Clear read display
  delay(200);
  temp = (*AT91C_PIOC_PDSR >> 2) & 0xff; // Read data bus and save it in temp
  *AT91C_PIOC_SODR = CE; // Set chip select display
  *AT91C_PIOC_CODR = WR; // Clear write display
  *AT91C_PIOC_SODR = RD; // Set read display
  DISABLE_CHIP; // Disable output
  SET_DIR_OUTPUT; // Set dir as output
  return temp;
}

void write_command_2_display(uchar command) {
  while (STATUS_CHECK_1);
  *AT91C_PIOC_CODR = databus; // Clear databus
  *AT91C_PIOC_SODR = (command << 2); // Set command to databus
  SET_DIR_OUTPUT; // Enable dir as output
  ENABLE_CHIP; // Enable output
  *AT91C_PIOC_OER = databus; // Set databus as output TODO
  *AT91C_PIOC_SODR = CD; // Set C/D as high
  *AT91C_PIOC_CODR = CE; // Clear chip select display
  *AT91C_PIOC_CODR = WR; // Clear write display
  delay(200);
  *AT91C_PIOC_SODR = CE; // Set chip enable display
  *AT91C_PIOC_SODR = WR; // Set write display
  DISABLE_CHIP; // Disable output
  *AT91C_PIOC_ODR = databus; // Set databus as input
}

void write_data_2_display(uchar data) {
  while (STATUS_CHECK_1);
  *AT91C_PIOC_CODR = databus; // Clear databus
  *AT91C_PIOC_SODR = (data << 2); // Set data to databus
  SET_DIR_OUTPUT; // Enable dir as output
  ENABLE_CHIP; // Enable output
  *AT91C_PIOC_OER = databus; // Set databus as output
  *AT91C_PIOC_CODR = CD; // Clear C/D
  *AT91C_PIOC_CODR = CE; // Clear chip enable
  *AT91C_PIOC_CODR = WR; // Clear write
  delay(200);
  *AT91C_PIOC_SODR = CE; // Set chip enable
  *AT91C_PIOC_SODR = WR; // Set write
  DISABLE_CHIP; // Disable output
  *AT91C_PIOC_ODR = databus; // Set databus as input
}

void set_line_on_display(int line) {
  
  if (line <= 8) {
    write_data_2_display(0x1e * line);    // low second line
    write_data_2_display(0x00);    // high
  } else {
    write_data_2_display(0x1e * (line - 9) + 0x0e);    // low second line
    write_data_2_display(0x01);    // high
  }
  delay(1000);
  write_command_2_display(0x24); // Set address pointer
  delay(1000);
}

/*
line 0: 0x0000 - 0x001e
line 8: 0x001e * 0x8 = 0x00f0 - 0x00ff  |  0x0100 - 0x010f
*/
void set_position_on_display(int x, int y) {
  if (y == 8) {
    if (x <= 0x0f) {
      write_data_2_display(0x1e * 8 + x);
      write_data_2_display(0x00);    // high
    } else {
      write_data_2_display(x - 0x10);
      write_data_2_display(0x01);    // high
    }
  } else if (y < 8) {
    write_data_2_display(0x1e * y + x);    // low second line
    write_data_2_display(0x00);    // high
  } else {
    write_data_2_display(0x1e * (y - 9) + 0x0e + x);    // low second line
    write_data_2_display(0x01);    // high
  }
  write_command_2_display(0x24); // Set address pointer
}

void init_display() {
  
  *AT91C_PMC_PCER = ((1 << 14) | (1 << 13)); // Start the peripheral clock for port C & D
  
  // for display
  *AT91C_PIOC_PER = databus; // Enable PIO for display
  *AT91C_PIOC_PPUDR = databus; // Disable the internal pull up resistor for display
  *AT91C_PIOC_ODR = databus; // Disable output for display
  
  // for display
  *AT91C_PIOD_PER = 1; // Enable PIO for PD0
  *AT91C_PIOD_PPUDR = 1; // Disable the internal pull up resistor for PD0
  *AT91C_PIOD_OER = 1; // Enable output for PD0
  *AT91C_PIOC_PER = 0xff000; // Enable PIO for PC12-19
  *AT91C_PIOC_PPUDR = 0xff000; // Disable the internal pull up resistor for PC12-19
  *AT91C_PIOC_OER = 0xff000; // Enable output for PC12-19
  
  *AT91C_PIOD_CODR = 1; // Clear Reset
  delay(100);
  *AT91C_PIOD_SODR = 1; // Set Reset
  
  write_data_2_display(0x00);
  write_data_2_display(0x00);
  write_command_2_display(0x40);//Set text home address
  write_data_2_display(0x00);
  write_data_2_display(0x00);
  write_command_2_display(0x42); //Set graphic home address
  write_data_2_display(0x1e);
  write_data_2_display(0x00);
  write_command_2_display(0x41); // Set text area
  write_data_2_display(0x1e);
  write_data_2_display(0x00);
  write_command_2_display(0x43); // Set graphic area
  write_command_2_display(0x80); // text mode "and mode" TODO
  switch_display_mode(TEXTMODE);
  
  reset_screen(0);
}

void switch_font_table(int mode) {
  if (mode == 0) {
    write_data_2_display(0x03);
    write_data_2_display(0x00);
    write_command_2_display(0x70);
  } else {
    write_data_2_display(0x02);
    write_data_2_display(0x00);
    write_command_2_display(0x70);
  }
}

void switch_display_mode(int mode) {
  if (mode == TEXTMODE) {
    write_command_2_display(0x94); // Text on graphic off
    current_mode = TEXTMODE;
  } else if (mode == GRAPHMODE) {
    write_command_2_display(0x98); // Text off graphic on
    current_mode = GRAPHMODE;
  } else if (mode == DOUBLEMODE) {
    write_command_2_display(0x9c); // Text on graphic on
    current_mode = DOUBLEMODE;
  }
}

const font font_table[] = {
  {'0', 0x10}, {'1', 0x11}, {'2', 0x12}, {'3', 0x13},
  {'4', 0x14}, {'5', 0x15}, {'6', 0x16}, {'7', 0x17},
  {'8', 0x18}, {'9', 0x19}, {'*', 0x0a}, {'#', 0x03},
  
  {'A', 0x21}, {'B', 0x22}, {'C', 0x23}, {'D', 0x24},
  {'E', 0x25}, {'F', 0x26}, {'G', 0x27}, {'H', 0x28},
  {'I', 0x29}, {'J', 0x2A}, {'K', 0x2B}, {'L', 0x2C},
  {'M', 0x2D}, {'N', 0x2E}, {'O', 0x2F}, {'P', 0x30},
  {'Q', 0x31}, {'R', 0x32}, {'S', 0x33}, {'T', 0x34},
  {'U', 0x35}, {'V', 0x36}, {'W', 0x37}, {'X', 0x38},
  {'Y', 0x39}, {'Z', 0x3A},
  
  {'a', 0x41}, {'b', 0x42}, {'c', 0x43}, {'d', 0x44},
  {'e', 0x45}, {'f', 0x46}, {'g', 0x47}, {'h', 0x48},
  {'i', 0x49}, {'j', 0x4A}, {'k', 0x4B}, {'l', 0x4C},
  {'m', 0x4D}, {'n', 0x4E}, {'o', 0x4F}, {'p', 0x50},
  {'q', 0x51}, {'r', 0x52}, {'s', 0x53}, {'t', 0x54},
  {'u', 0x55}, {'v', 0x56}, {'w', 0x57}, {'x', 0x58},
  {'y', 0x59}, {'z', 0x5A}, 
  
  {' ', 0x00}, {'.', 0x0e}, {'!', 0x01}, {':', 0x1a},
  {'%', 0x05},
};

uint char_2_font(uchar data) {
  int N = sizeof(font_table) / sizeof(font);
  int i;
  for (i = 0; i < N; i++) {
    if (data == font_table[i].data) {
      return font_table[i].index;
    }
  }
  return 0x00;
}

void write_char_2_display(uchar data) {
  delay(1000);
  write_data_2_display(char_2_font(data));
  write_command_2_display(0xc0);
}

void write_str_2_display(const char* str) {
  while (*str != '\0') {
    delay(1000);
    write_char_2_display(*str++);
  }
}

void write_2_digits_2_display(int num) {
  int first = num % 10;
  int second = (num / 10) % 10;
  write_char_2_display('0' + second);
  write_char_2_display('0' + first);
}

void write_3_digits_2_display(int num) {
  int first = num % 10;
  int second = (num / 10) % 10;
  int third = (num / 100) % 10;
  write_char_2_display('0' + third);
  write_char_2_display('0' + second);
  write_char_2_display('0' + first);
}

void write_pressure_digits_2_display(float num) {
  int integer = (int)num;
  int first = integer % 10;
  int second = (integer / 10) % 10;
  int third = (integer / 100) % 10;
  int forth = (integer / 1000) % 10;
  int fifth = (integer / 10000) % 10;
  int sixth = (integer / 100000) % 10;
  write_char_2_display('0' + sixth);
  write_char_2_display('0' + fifth);
  write_char_2_display('0' + forth);
  write_char_2_display('0' + third);
  write_char_2_display('0' + second);
  write_char_2_display('0' + first);
  write_char_2_display('.');
  int intf = (int)num;
  int decf = (int)((num - intf) * 100);
  write_2_digits_2_display(decf);
}


void write_str_2_display_with_free(char* str) {
  if (str == NULL) {
    write_str_2_display(memory_allocation_error);
  } else {
    write_str_2_display(str);
    free(str);
  }
}
  
void write_test() {
  char* test = "Hello world! ";
  while (*test != '\0') {
    write_data_2_display(char_2_font(*test++));
    write_command_2_display(0xc0);
  }
}

void fill_one_line_with_char(int line, uchar c) {
  set_line_on_display(line);
  for (int i = 0; i < 0x1e; i++) {
    write_char_2_display(c);
  }
}

void reset_screen(int line) {
  int i, j, k;
  if (current_mode == TEXTMODE) {
    set_line_on_display(line);
    for (i = 0; i < 1000; i++) {
      write_data_2_display(0x0e);
      write_command_2_display(0xc4);
      write_data_2_display(0x00);
      write_command_2_display(0xc0);
    }
  } else if (current_mode == GRAPHMODE) {
    for (i = 0; i < 0x10; i++) { // line
      for (j = 0; j < 0x1e; j++) {
        write_data_2_display(j);    // low second line
        write_data_2_display(i);    // high
        write_command_2_display(0x24); // Set address pointer
        for (k = 0; k < 8; k++) { // reset every bits in address pointer
          write_command_2_display(0xf0 + k);
        }
      }
    }
  }
}                

void show_menu() {
  int current_line = 0;
  switch (key_value) {
  case '*':
    set_line_on_display(current_line++);
    write_str_2_display("Hello World!                      ");
    set_line_on_display(current_line++);
    write_str_2_display("1. Show logging of temperature    ");
    set_line_on_display(current_line++);
    write_str_2_display("2. Find the sun                   ");
    set_line_on_display(current_line++);
    write_str_2_display("3. Draw graph of temperature      ");
    set_line_on_display(current_line++);
    write_str_2_display("4. Set low temperature limit      ");
    set_line_on_display(current_line++);
    write_str_2_display("5. Set high temperature limit     ");
#ifdef GRADE5
    set_line_on_display(current_line++);
    write_str_2_display("6. Draw graph of pressure         ");
#endif
#ifdef GRADE4
    set_line_on_display(current_line++);
    write_str_2_display("6. Check size of list             ");
#endif
#ifndef GRADE3
    set_line_on_display(current_line++);
    write_str_2_display("0. Set the time factor            ");
#endif
    alarm(current_line++);
    reset_screen(current_line);
    break;
  case '1':
    show_temp_record();
    delay(100000);
    break;
  case '2':
    track_sun_position();
    key_value = '*';
    delay(100000);
    break;
  case '3':
    draw_graph(temp_min, temp_max, temp_avg, day_total);
    delay(100000);
    break;
  case '4':
    set_temp_low_limit();
    key_value = '*';
    delay(2000000);
    break;
  case '5':
    set_temp_high_limit();
    key_value = '*';
    delay(2000000);
    break;
#ifdef GRADE5
  case '6':
    draw_graph(pressure_min, pressure_max, pressure_avg, pressure_day_total);
    key_value = '*';
    delay(100000);
    break;
#endif
    
#ifdef GRADE4
  case '6':
    show_temp_list();
    key_value = '*';
    delay(100000);
    break;
#endif
    
#ifndef GRADE3
  case '0':
    set_time_factor();
    key_value = '*';
    delay(200000);
    break;
#endif
  }
}