#ifndef PRESSURE_H_
#define PRESSURE_H_

#include "util.h"
#include "display.h"
#include "keypad.h"
#include "temp.h"

extern volatile uint ms_ticks;
extern volatile uint tick_1s_elapsed;
extern volatile float pressure;
extern float pressure_record_array[MIN_PER_DAY * MAX_DAYS]; // temp record of last 7 days, using array
extern float pressure_min[MAX_DAYS];  // min pressure record of last 7 days
extern float pressure_max[MAX_DAYS];  // max pressure record of last 7 days
extern float pressure_avg[MAX_DAYS];  // avg pressure record of last 7 days
extern float pressure_sum_per_day; // accumulated pressure of the day
extern uint pressure_record_index; // total record index
extern uint pressure_day_record_index;  // record index of the day
extern uint pressure_flag; // is it ready to get time?
extern uint pressure_day_total; // how many days has passed?

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define MPL3115A2_ADDRESS                       (0x60)    // 1100000
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    #define MPL3115A2_REGISTER_STATUS               (0x00)
    #define MPL3115A2_REGISTER_STATUS_TDR 0x02
    #define MPL3115A2_REGISTER_STATUS_PDR 0x04
    #define MPL3115A2_REGISTER_STATUS_PTDR 0x08

    #define MPL3115A2_REGISTER_PRESSURE_MSB         (0x01)
    #define MPL3115A2_REGISTER_PRESSURE_CSB         (0x02)
    #define MPL3115A2_REGISTER_PRESSURE_LSB         (0x03)

    #define MPL3115A2_REGISTER_TEMP_MSB             (0x04)
    #define MPL3115A2_REGISTER_TEMP_LSB             (0x05)

    #define MPL3115A2_REGISTER_DR_STATUS            (0x06)

    #define MPL3115A2_OUT_P_DELTA_MSB               (0x07)
    #define MPL3115A2_OUT_P_DELTA_CSB               (0x08)
    #define MPL3115A2_OUT_P_DELTA_LSB               (0x09)

    #define MPL3115A2_OUT_T_DELTA_MSB               (0x0A)
    #define MPL3115A2_OUT_T_DELTA_LSB               (0x0B)

    #define MPL3115A2_WHOAMI                        (0x0C)

#define MPL3115A2_PT_DATA_CFG 0x13
#define MPL3115A2_PT_DATA_CFG_TDEFE 0x01
#define MPL3115A2_PT_DATA_CFG_PDEFE 0x02
#define MPL3115A2_PT_DATA_CFG_DREM 0x04

#define MPL3115A2_CTRL_REG1                     (0x26)
#define MPL3115A2_CTRL_REG1_SBYB 0x01
#define MPL3115A2_CTRL_REG1_OST 0x02
#define MPL3115A2_CTRL_REG1_RST 0x04
#define MPL3115A2_CTRL_REG1_OS1 0x00
#define MPL3115A2_CTRL_REG1_OS2 0x08
#define MPL3115A2_CTRL_REG1_OS4 0x10
#define MPL3115A2_CTRL_REG1_OS8 0x18
#define MPL3115A2_CTRL_REG1_OS16 0x20
#define MPL3115A2_CTRL_REG1_OS32 0x28
#define MPL3115A2_CTRL_REG1_OS64 0x30
#define MPL3115A2_CTRL_REG1_OS128 0x38
#define MPL3115A2_CTRL_REG1_RAW 0x40
#define MPL3115A2_CTRL_REG1_ALT 0x80
#define MPL3115A2_CTRL_REG1_BAR 0x00
#define MPL3115A2_CTRL_REG2                     (0x27)
#define MPL3115A2_CTRL_REG3                     (0x28)
#define MPL3115A2_CTRL_REG4                     (0x29)
#define MPL3115A2_CTRL_REG5                     (0x2A)

void init_pressure();
int read_pressure();
void IIC_reg_write(uchar reg, uchar val);
int IIC_reg_read(uchar reg);
void record_pressure(int type);
void show_pressure_record();
void write_pressure_2_display(float temp);
void write_num_2_display(int num);

#endif