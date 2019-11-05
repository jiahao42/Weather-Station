#include "util.h"
#include "display.h"
#include "pressure.h"
#include "test.h"
#include "temp.h"
#include "servo.h"

void test_suite() {
#ifdef GRADE4
  test_memory_management();
  test_temp();
#endif
#ifdef GRADE5
  test_keypad();
  test_display();
  test_temp();
  test_temp_graph();
  test_pressure();
  test_pressure_graph();
  test_sensors();
  test_track_sun_position();
#endif
}

void test_keypad() {
  uchar temp_key_value = '1';
  set_line_on_display(0);
  write_str_2_display("Press the right key to pass ");
  set_line_on_display(1);
  write_str_2_display("the keypad test: ");
  set_line_on_display(4);
  write_str_2_display("Press * to continue");
  while((temp_key_value = read_key()) != '*');
  reset_screen(0);
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 1 to pass the test: ");
  while((temp_key_value = read_key()) != '1');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 2 to pass the test: ");
  while((temp_key_value = read_key()) != '2');
  delay(20000);
 
  set_line_on_display(0);
  write_str_2_display("Press the 3 to pass the test: ");
  while((temp_key_value = read_key()) != '3');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 4 to pass the test: ");
  while((temp_key_value = read_key()) != '4');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 5 to pass the test: ");
  while((temp_key_value = read_key()) != '5');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 6 to pass the test: ");
  while((temp_key_value = read_key()) != '6');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 7 to pass the test: ");
  while((temp_key_value = read_key()) != '7');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 8 to pass the test: ");
  while((temp_key_value = read_key()) != '8');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 9 to pass the test: ");
  while((temp_key_value = read_key()) != '9');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the 0 to pass the test: ");
  while((temp_key_value = read_key()) != '0');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the # to pass the test: ");
  while((temp_key_value = read_key()) != '#');
  delay(20000);
  
  set_line_on_display(0);
  write_str_2_display("Press the * to pass the test: ");
  while((temp_key_value = read_key()) != '*');
  delay(20000);
  
}

void test_display() {
  uchar temp_key_value = 0x00;
  int current_line;
  reset_screen(0);
  
  for (current_line = 0; current_line < 16; current_line++) {
    set_line_on_display(current_line);
    delay(2000);
    write_str_2_display("Press any key to fill the line: ");
    delay(2000);
    while((temp_key_value = read_key()) == 0x00);
    fill_one_line_with_char(current_line, temp_key_value);
    delay(2000000);
  }
}

void test_temp() {
  reset_screen(0);
  int i;
  for (i = 0; i < 10080; i++) {
    record_temp(0);
    if (i % 100 == 0) {
      set_line_on_display(0);
      write_str_2_display("filling temp progress: ");
      srand(time(NULL));   // should only be called once
      float precentage = i / 10080.0 * 100.0;
      write_temp_2_display(precentage);
      write_char_2_display('%');
    }
  }
  set_line_on_display(0);
  write_str_2_display("filling temp progress: 100%       ");
  
  set_line_on_display(4);
  write_str_2_display("Press * to continue      ");
  
  uchar temp_key_value = 0x00;
  while((temp_key_value = read_key()) != '*');
}


void test_pressure() {
  reset_screen(0);
  int i;
  for (i = 0; i < 10080; i++) {
    record_pressure(0);
    if (i % 100 == 0) {
      set_line_on_display(0);
      write_str_2_display("filling pressure progress: ");
      srand(time(NULL));   // should only be called once
      float precentage = i / 10080.0 * 100.0;
      write_temp_2_display(precentage);
      write_char_2_display('%');
    }
  }
  set_line_on_display(0);
  write_str_2_display("filling pressure progress: 100%       ");

  set_line_on_display(4);
  write_str_2_display("Press * to continue      ");
  
  uchar temp_key_value = 0x00;
  while((temp_key_value = read_key()) != '*');
}

void test_temp_graph() {
  key_value = '0';
  draw_graph(temp_min, temp_max, temp_avg, day_total);
}

void test_pressure_graph() {
  key_value = '0';
  draw_graph(pressure_min, pressure_max, pressure_avg, pressure_day_total);
}

void test_sensors() {
  record_pressure(1);
  record_temp(1);
  delay(2000000);
  reset_screen(0);
  uchar temp_key_value = 0x00;
  while((temp_key_value = read_key()) != '*') {
    
    set_line_on_display(0);
    write_str_2_display("Temperature: ");
    write_temp_2_display(get_current_temp());
    
    set_line_on_display(1);
    write_str_2_display("Pressure: ");
    write_pressure_digits_2_display(pressure_record_array[pressure_record_index]);
    
    set_line_on_display(4);
    write_str_2_display("Press * to continue      ");
    record_pressure(1);
    record_temp(1);
    delay(2000000);
  }
}


void test_track_sun_position() {
  track_sun_position();
}

void test_memory_management() {
  int line = 0;
  delay(2000000);
  fill_temp_list(); // fill temp list
  reset_screen(0);
  set_line_on_display(line++);
  write_str_2_display("The list has been filled");
  set_line_on_display(line++);
  write_str_2_display("Press 1 to write a new value");
  
  set_line_on_display(line + 2);
  write_str_2_display("Press * to end the test");
  
  uchar temp_key_value = 0;
  while ((temp_key_value = read_key()) != '*') {
    delay(2000000);
    if (temp_key_value == '1') {
      record_temp(0);
    }
  }
}
