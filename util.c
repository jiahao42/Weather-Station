#include "util.h"

const char* memory_allocation_error = "Memory allocation failed!";

volatile uint ms_ticks = 0;
volatile uint tick_1s_elapsed = 0;
extern uint temp_flag;
uint time_factor = 1; //  Each minute, temperature is recorded N times and averaged to give one number
void SysTick_Handler(void){
  ms_ticks++;
#ifdef GRADE3
  #ifdef TEST_MODE
    if (ms_ticks % 1000 == 0) { // 1000ms = 1min
       temp_flag = 1;
    }
  #else
    if (ms_ticks % 1000 == 0) { // 1000ms = 1s
      tick_1s_elapsed++;
    }
    if (tick_1s_elapsed == 60) { // 60s = 1min
      temp_flag = 1;
      tick_1s_elapsed = 0;
    }
  #endif
#else
  #ifdef TEST_MODE
    if (ms_ticks >= (1000 / time_factor)) { // (1000/N)ms = 1 measurement
       temp_flag = 1;
    }
  #else
    if (ms_ticks % 1000 == 0) { // 1000ms = 1s
      tick_1s_elapsed++;
    }
    if (tick_1s_elapsed >= (60 / time_factor)) { // (60 / N)s = 1 measurement
      temp_flag = 1;
      tick_1s_elapsed = 0;
    }
  #endif
#endif
}

volatile uint if_interrupt = 0;
void TC0_Handler(void) {
  *AT91C_TC0_IDR = (1 << 6); // Disable interrupt TC_IER_LDRBS
  int end = *AT91C_TC0_RB;
  int start = *AT91C_TC0_RA;
  temp = end - start;
  if_interrupt = 1;
}

void delay(int val){
  int i;
  for(i = 0; i < val; i++)
    asm("nop");
}

char* int_2_str(int num) {
  if (num == 0) {
    char* str = (char*)malloc(sizeof(char) * 2);
    str[0] = '0';
    str[1] = '\0';
    return str;
  }
  int temp = num;
  int digits = 0;
  while (temp) { // get digits
    digits++;
    temp /= 10;
  }
  char* str = (char*)malloc(sizeof(char) * (digits + 1));
  str[digits--] = '\0';
  while (num) {
    int reminder = num % 10;
    str[digits--] = reminder + '0';
    num /= 10;
  }
  return str;
}

int appro_equal(float f) {
  float dec = f - (int)f;
  return dec >= 0.5 ? (int)(f + 1) : (int)f;
}