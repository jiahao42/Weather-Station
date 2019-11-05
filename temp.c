#include "stdlib.h"
#include "util.h"
#include "display.h"
#include "keypad.h"
#include "temp_list.h"
#include "temp.h"

extern volatile uint ms_ticks;
extern volatile uint tick_1s_elapsed;
volatile float temp;
#ifdef GRADE3
float temp_record_array[MIN_PER_DAY * MAX_DAYS]; // temp record of last 7 days, using array
#else
temp_list* temp_record_list; // temp record, as many as possible
float accumulate_temp_per_min;
uint accumulate_count_per_min;
#endif
float temp_min[MAX_DAYS];  // min temp record of last 7 days
float temp_max[MAX_DAYS];  // max temp record of last 7 days
float temp_avg[MAX_DAYS];  // avg temp record of last 7 days
float temp_sum_per_day; // accumulated temp of the day
uint record_index; // total record index
uint day_record_index = 1;  // record index of the day
uint temp_flag; // is it ready to get time?
uint day_total; // how many days has passed?
float temp_low_limit = 0;
float temp_high_limit = 25;

void init_temp_sensor() {
  int i;
  *AT91C_PMC_PCER = (1 << 12); // Staert the peripheral clock for port B
  *AT91C_PMC_PCER = (1 << 27); // Start the peripheral clock for TC0
  *AT91C_TC_WPMR = (0x54494D << 8); // Disable write protect
  *AT91C_TC0_CMR &= ~0x07; // Select Timer_Clock1 as TCCLK, MCK / 2
  *AT91C_TC0_CCR = 0x05; // Enable counter and make a sw_reset in TC0_CCR0
  *AT91C_TC0_CMR |= ((0x02 << 16) | // Failing edge of A
                     (0x01 << 18)); // Rising edge of B
  *AT91C_PIOB_PER = PB25; // Enable PIO for PB25
  // *AT91C_PIOB_PPUDR = (1 << 25); // Disable the internal pull up resistor for PB25
  *AT91C_PIOB_OER = PB25; // Enable output for PB25
  *AT91C_PIOB_CODR = PB25; // Make PB25 as low for Reset

  NVIC_ClearPendingIRQ(TC0_IRQn);
  NVIC_SetPriority(TC0_IRQn, 0);
  NVIC_EnableIRQ(TC0_IRQn);
  
  *AT91C_TC0_CCR = (1 << 2); // make a sw_reset in TC0_CCR0
  for (i = 0; i < MAX_DAYS; i++) {
    temp_min[i] = 0x100;
  }
  
#ifndef GRADE3
  temp_record_list = create_list();
#endif
  
  measure_temp();
}

void measure_temp() {
  *AT91C_TC0_IER = (1 << 6); // Enable interrupt TC_IER_LDRBS
  *AT91C_PIOB_OER = PB25; // Enable output for PB25
  *AT91C_PIOB_CODR = PB25; // Make PB25 as low
  *AT91C_PIOB_ODR = PB25; // Enable input for PB25
  delay(25);
  *AT91C_TC0_CCR = (1 << 2); // make a sw_reset in TC0_CCR0
  delay(100);
}

float get_temp(int type) {
  if (type == 0) { // fake data
    delay(100);
    int r = rand() % 20 + 5;      // returns a pseudo-random integer between 0 and RAND_MAX
    return r;
  } else if (type == 1) {
    return temp / 42 / 5.0 - 273.15; 
  } else {
    return temp / 42 / 5.0 - 273.15; 
  }
}

void temp_2_display() {
  delay(2000000);
  write_data_2_display(0x1e);    // low second line
  write_data_2_display(0x00);    // high
  write_command_2_display(0x24); // Set address pointer
  char *prompt = "The temperature is: ";
  write_str_2_display(prompt);
  if (if_interrupt) {
    write_temp_2_display(get_temp(1));
    write_str_2_display("   ");
    measure_temp();
  }
}

void record_temp(int type) {
  uint current_day = day_total % MAX_DAYS; // current day
#ifdef GRADE3
  temp_record_array[record_index] = get_temp(type);
  if (temp_record_array[record_index] > 0) { // temporary hack
    temp_sum_per_day += temp_record_array[record_index]; // accmulate
    temp_max[current_day] = temp_max[current_day] > temp_record_array[record_index] ? temp_max[current_day] : temp_record_array[record_index];
    temp_min[current_day] = temp_min[current_day] < temp_record_array[record_index] ? temp_min[current_day] : temp_record_array[record_index];
    temp_avg[current_day] = temp_sum_per_day / day_record_index++;
    record_index++;
  }
  if (record_index == (1440 * 7))  // if the data is full, start over
    record_index = 0;
  if (record_index > 0 && record_index % 1440 == 0) {// one day has passed
    day_total++;
    day_record_index = 1;
    temp_sum_per_day = 0;
  }
#else // GRADE 4&5
  float ttemp = get_temp(type);
  accumulate_temp_per_min += ttemp;
  accumulate_count_per_min += 1;
  if (accumulate_count_per_min == time_factor) {
    if (accumulate_temp_per_min > 0 && temp_record_list->cursor->temp > 0) { // temporary hack
      append_temp(temp_record_list, accumulate_temp_per_min / time_factor);
      temp_sum_per_day += temp_record_list->cursor->temp; // accmulate
      temp_max[current_day] = temp_max[current_day] > temp_record_list->cursor->temp ? temp_max[current_day] : temp_record_list->cursor->temp;
      temp_min[current_day] = temp_min[current_day] < temp_record_list->cursor->temp ? temp_min[current_day] : temp_record_list->cursor->temp;
      temp_avg[current_day] = temp_sum_per_day / day_record_index++;
      record_index++;
    }
    if (record_index == (1440 * 7))  // if the data is full, start over
      record_index = 0;
    if (record_index > 0 && record_index % 1440 == 0) {// one day has passed
      day_total++;
      day_record_index = 1;
      temp_sum_per_day = 0;
    }
    accumulate_count_per_min = 0;
    accumulate_temp_per_min = 0;
  }
#endif

  measure_temp(); // start new measure
}

void show_temp_record() {
  set_line_on_display(0);
  int i;
  if (day_total < 7) {
    for (i = 0; i <= day_total; i++) {
      set_line_on_display(i * 2);
      write_str_2_display("Day ");
      write_2_digits_2_display(i);
      write_str_2_display(": ");
      write_str_2_display("                         ");
      set_line_on_display(i * 2 + 1);
      write_str_2_display("Max:");
      write_temp_2_display(temp_max[i]);
      write_str_2_display(" Min:");
      write_temp_2_display(temp_min[i]);
      write_str_2_display(" Avg:");
      write_temp_2_display(temp_avg[i]);
      write_str_2_display("        ");
      delay(1000);
    }
  } else {
    for (i = 0; i <= 6; i++) { // TODO, temp hack
      set_line_on_display(i * 2);
      write_str_2_display("Day ");
      write_2_digits_2_display(i);
      write_str_2_display(": ");
      write_str_2_display("                         ");
      set_line_on_display(i * 2 + 1);
      write_str_2_display("Max:");
      write_temp_2_display(temp_max[i]);
      write_str_2_display(" Min:");
      write_temp_2_display(temp_min[i]);
      write_str_2_display(" Avg:");
      write_temp_2_display(temp_avg[i]);
      write_str_2_display("        ");
      delay(1000);
    }
  }
  if (day_total < 7) {
    reset_screen(day_total * 2 + 2); // clear the remaining screen
  }
#ifndef GRADE3
  /* indication of full memory */
  if (temp_record_list->full_flag == 1) {
    set_line_on_display(0x0e);
    write_str_2_display("The memory is full!!!     ");
  }
#endif
  
}

void alarm(int line) {
  delay(1000);
#ifdef GRADE3
  float current_temp = temp_record_array[record_index - 2];
#else
  float current_temp = temp_record_list->current_temp;
#endif
  if (current_temp < temp_low_limit) {
    set_line_on_display(line);
    write_str_2_display("Alarm!!! Temperature too low!!!");
  } else if (current_temp > temp_high_limit) {
    set_line_on_display(line);
    write_str_2_display("Alarm!!! Temperature too high!!!");
  } else {
    set_line_on_display(line);
    write_str_2_display("                                ");
  }
}

float keypad_2_float() {
  float integer = 0;
  float decimal = 0;
  uchar temp_key_value = 0;
  int dot_flag = 0;
  while ((temp_key_value = read_key()) != '#') {
    delay(2000000);
    if (temp_key_value == '*') {
      dot_flag = 1;
      write_char_2_display('.');
    }
    if (dot_flag == 0) { // integer
      if (temp_key_value >= '0' && temp_key_value <= '9') {
        integer += temp_key_value - '0';
        integer *= 10;
        write_char_2_display(temp_key_value);
      }
    } else {  // decimal
      if (temp_key_value >= '0' && temp_key_value <= '9') {
        decimal += temp_key_value - '0';
        decimal *= 10;
        write_char_2_display(temp_key_value);
      }
    }
  }
  decimal /= 10;
  integer /= 10;
  return integer + decimal / 100;
}

int keypad_2_int() {
  int integer = 0;
  uchar temp_key_value = 0;
  while ((temp_key_value = read_key()) != '#') {
    if (temp_key_value == '*')
      return time_factor;
    delay(2000000);
    if (temp_key_value >= '0' && temp_key_value <= '9') {
      integer += temp_key_value - '0';
      integer *= 10;
      write_char_2_display(temp_key_value);
    }
  }
  integer /= 10;
  return integer;
}

void set_time_factor() {
  delay(2000000);
  set_line_on_display(0);
  write_str_2_display("The time factor is:                       ");
  set_line_on_display(1);
  // write_str_2_display_with_free(int_2_str(time_factor));
  write_2_digits_2_display(time_factor);
  write_str_2_display("                                       ");
  set_line_on_display(2);
  write_str_2_display("Please input the time factor: ");
  reset_screen(3);
  set_line_on_display(3);
  int res = keypad_2_int();
  if (res >= 1 && res <= 10) { // valid range
    time_factor = res;
  }
}

void set_temp_low_limit() {
  delay(2000000);
  set_line_on_display(0);
  write_str_2_display("The low limit is:                       ");
  set_line_on_display(1);
  // write_str_2_display_with_free(temp_2_str(temp_low_limit));
  write_temp_2_display(temp_low_limit);
  write_str_2_display("                                       ");
  set_line_on_display(2);
  write_str_2_display("Please input the low limit: ");
  reset_screen(3);
  set_line_on_display(3);
  temp_low_limit = keypad_2_float();
}
                      
void set_temp_high_limit() {
  delay(2000000);
  set_line_on_display(0);
  write_str_2_display("The high limit is:                       ");
  set_line_on_display(1);
  // write_str_2_display_with_free(temp_2_str(temp_high_limit));
  write_temp_2_display(temp_high_limit);
  write_str_2_display("                                       ");
  set_line_on_display(2);
  write_str_2_display("Please input the high limit: ");
  reset_screen(3);
  set_line_on_display(3);
  temp_high_limit = keypad_2_float();
}                 


char* temp_2_str(float f) { // limited, only for 2 digit decimal
  int intf = (int)f;
  char* s_int = int_2_str(intf);
  int decf = (int)((f - intf) * 100);
  char* s_dec = int_2_str(decf);
  char* str = (char*)malloc(2 + 2 + 1 + 1); // for '.' and '\0'
  if (str == NULL) {
    free(s_int);
    free(s_dec);
    return NULL;
  }
  if (intf < 10) {
    memcpy(str + 1, s_int, 2);
    str[0] = '0';
  } else {
    memcpy(str, s_int, 2);
  }
  str[2] = '.';
  if (decf < 10) {
    memcpy(str + 2 + 2, s_dec, 2);
    str[3] = '0';
  } else {
    memcpy(str + 2 + 1, s_dec, 2);
  }
  
  str[2 + 2 + 1] = '\0';
  free(s_int);
  free(s_dec);
  return str;
}


void write_temp_2_display(float temp) {
  int intf = (int)temp; // 20.45 -> 20
  int decf = (int)((temp - intf) * 100); // (20.45 - 20) * 100 = 45
  write_2_digits_2_display(intf);
  write_char_2_display('.');
  write_2_digits_2_display(decf);
}

#ifndef GRADE3

void fill_temp_list() {
  while (temp_record_list->full_flag == 0) { // until it is full
    record_temp(0);
  }
}

void show_temp_list() {
  reset_screen(0);
  set_line_on_display(0);
  write_str_2_display("The total size of list is: ");
  write_3_digits_2_display(temp_record_list->size);
  
  set_line_on_display(4);
  write_str_2_display("Press * to return");
  uchar temp_key_value;
  while ((temp_key_value = read_key()) != '*');
}

float get_current_temp() {
  return temp_record_list->current_temp;
}

#endif
