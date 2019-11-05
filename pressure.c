#include "pressure.h"


extern volatile uint ms_ticks;
extern volatile uint tick_1s_elapsed;
volatile float pressure;
float pressure_record_array[MIN_PER_DAY * MAX_DAYS]; // temp record of last 7 days, using array
float pressure_min[MAX_DAYS];  // min pressure record of last 7 days
float pressure_max[MAX_DAYS];  // max pressure record of last 7 days
float pressure_avg[MAX_DAYS];  // avg pressure record of last 7 days
float pressure_sum_per_day; // accumulated pressure of the day
uint pressure_record_index; // total record index
uint pressure_day_record_index = 1;  // record index of the day
uint pressure_flag; // is it ready to get time?
uint pressure_day_total; // how many days has passed?

void init_pressure() {
  *AT91C_PMC_PCER = (1 << 12); // Start the peripheral clock for port B
  *AT91C_PMC_PCER = (1 << 23); // Start the peripheral clock for Two-Wire Interface 1
  
  *AT91C_PIOB_PDR = ((1 << 12) | (1 << 13));
  *AT91C_PIOB_PPUER = ((1 << 12) | (1 << 13));
  
  *AT91C_TWI1_CWGR = (1 << 18) | (1 << 10) | (1 << 3); // Set TWI Clock low = 0x08 high = 0x04 divider = 0x04
  *AT91C_TWI1_CR = 0x24; // Set MSEN & SVDIS
  
  int i;
  for (i = 0; i < MAX_DAYS; i++) {
    pressure_min[i] = 0x100000;
  }
  srand(time(NULL));   // should only be called once
}

void IIC_reg_write(uchar reg, uchar val) {
  *AT91C_TWI1_MMR = 0;
  *AT91C_TWI1_MMR = (1 << 8) | (0 << 12) | (MPL3115A2_ADDRESS << 16);
  *AT91C_TWI1_IADR = reg;
  *AT91C_TWI1_THR = val;

  *AT91C_TWI1_CR = 0x02; // Stop
  
  while ((*AT91C_TWI1_SR & 0x04) == 0) { // wait until TXRDY = 1
    int test = *AT91C_TWI1_SR;
    test += test;
  }
  while ((*AT91C_TWI1_SR & 0x01) == 0); // wait until TXCOMP = 1
}

int IIC_reg_read(uchar reg) {
  *AT91C_TWI1_MMR = (1 << 8) | (1 << 12) | (MPL3115A2_ADDRESS << 16);
  *AT91C_TWI1_IADR = reg;
  
  *AT91C_TWI1_CR = 0x03; // Start | Stop
  
  while ((*AT91C_TWI1_SR & 0x02) == 0); // RXRDY
  int res = *AT91C_TWI1_RHR;
  while ((*AT91C_TWI1_SR & 0x01) == 0); // TXCOMP
  return res;
}

/*
The Pressure data is arranged as 20-bit unsigned data in Pascals. 
The first 18 bits are located in OUT_P_MSB, OUT_P_CSB
and bits 7-6 of OUT_P_LSB. 
The 2 bits in position 5-4 of OUT_P_LSB represent the fractional component. 

OUT_P_LSB: PD0 - PD1 (fractional component) & PD2 - PD3
OUT_P_CSB: PD4 - PD11
OUT_P_MSB: PD12 - 19
*/
int read_pressure(int type) {
  int res = 0;
  if (type == 0) {
    res = rand() % 500 + 100000;      // returns a pseudo-random integer between 0 and RAND_MAX
  } else {
//    IIC_reg_write(MPL3115A2_CTRL_REG1,
//           MPL3115A2_CTRL_REG1_SBYB |
//           MPL3115A2_CTRL_REG1_OS128 |
//           MPL3115A2_CTRL_REG1_ALT);

    IIC_reg_write(MPL3115A2_PT_DATA_CFG, 
           MPL3115A2_PT_DATA_CFG_TDEFE |
           MPL3115A2_PT_DATA_CFG_PDEFE |
           MPL3115A2_PT_DATA_CFG_DREM);
    
    IIC_reg_write(MPL3115A2_CTRL_REG1, 
           MPL3115A2_CTRL_REG1_SBYB |
           MPL3115A2_CTRL_REG1_OS128 |
           MPL3115A2_CTRL_REG1_BAR);
    uchar sta = 0;
    while (! (sta & MPL3115A2_REGISTER_STATUS_PDR)) {
      sta = IIC_reg_read(MPL3115A2_REGISTER_STATUS);
      delay(10);
    }
    res = IIC_reg_read(0x01); // receive DATA from MSB
    res <<= 8;
    res |= IIC_reg_read(0x02); // receive DATA from CSB
    res <<= 8;
    res |= IIC_reg_read(0x03); // receive DATA from LSB
    res >>= 4;
    res /= 4.0;
  }
  return res;
}

void record_pressure(int type) {
  uint current_day = pressure_day_total % MAX_DAYS; // current day
  pressure_record_array[pressure_record_index] = read_pressure(type);
  if (pressure_record_array[pressure_record_index] > 0) { // temporary hack
    pressure_sum_per_day += pressure_record_array[pressure_record_index]; // accmulate
    pressure_max[current_day] = pressure_max[current_day] > pressure_record_array[pressure_record_index] ? 
      pressure_max[current_day] : pressure_record_array[pressure_record_index];
    pressure_min[current_day] = pressure_min[current_day] < pressure_record_array[pressure_record_index] ? 
      pressure_min[current_day] : pressure_record_array[pressure_record_index];
    pressure_avg[current_day] = pressure_sum_per_day / pressure_day_record_index++;
    pressure_record_index++;
  }
  if (pressure_record_index == (MIN_PER_DAY * MAX_DAYS))  // if the data is full, start over
    pressure_record_index = 0;
  if (pressure_record_index > 0 && pressure_record_index % MIN_PER_DAY == 0) {// one day has passed
    pressure_day_total++;
    pressure_day_record_index = 1;
    pressure_sum_per_day = 0;
  }
}

void show_pressure_record() {
  set_line_on_display(0);
  int i;
  if (pressure_day_total < 7) {
    for (i = 0; i <= pressure_day_total; i++) {
      set_line_on_display(i * 2);
      write_str_2_display("Day ");
      write_2_digits_2_display(i);
      write_str_2_display(": ");
      write_str_2_display("                         ");
      set_line_on_display(i * 2 + 1);
      write_str_2_display("Max:");
      write_temp_2_display(pressure_max[i]);
      write_str_2_display(" Min:");
      write_temp_2_display(pressure_min[i]);
      write_str_2_display(" Avg:");
      write_temp_2_display(pressure_avg[i]);
      write_str_2_display("        ");
    }
  } else {
    // int last_day = day_total % MAX_DAYS;
    for (i = 0; i < 7; i++) {
      set_line_on_display(2);
      write_str_2_display("Day1: ");
      write_str_2_display("Max: ");
      //TODO
    }
  }
  reset_screen(pressure_day_total * 2 + 2); // clear the remaining screen
}

void write_pressure_2_display(float temp) { // TODO
  int intf = (int)temp; // 20.45 -> 20
  int decf = (int)((temp - intf) * 100); // (20.45 - 20) * 100 = 45
  write_2_digits_2_display(intf);
  write_char_2_display('.');
  write_2_digits_2_display(decf);
}

void write_num_2_display(int num) { // TODO
  int first = num % 10;
  int second = (num / 10) % 10;
  write_char_2_display('0' + second);
  write_char_2_display('0' + first);
}

