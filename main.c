#include "util.h"
#include "keypad.h"
#include "display.h"
#include "temp.h"
#include "photoresistor.h"
#include "servo.h"
#include "pressure.h"
#include "test.h"

extern volatile unsigned int if_interrupt;
extern uint temp_flag;
extern uchar key_value;
extern float temp_low_limit;
extern float temp_high_limit;

int main(void){
  SystemInit();
  // Setup SysTick Timer for 1us interrupts
  if (SysTick_Config(84000000 / 1000)) { // 84MHz, 1ms
    while (1) {} // If fail
  }

  init_keypad();
  init_display(TEXTMODE);
  init_temp_sensor();
  init_photoresistor();
  init_servo();
  init_pressure();
  
#ifdef TEST_MODE
  test_suite();
#endif
  while(1){
    show_menu();
    int key = read_key();
    key_value = key == 0 ? key_value : key;
    if (temp_flag) {
      record_temp(1);
#ifdef GRADE5
      record_pressure(1);
#endif
      temp_flag = 0;
    }
  }
}