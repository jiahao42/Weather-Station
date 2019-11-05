#ifndef TEMP_H_
#define TEMP_H_

#define PB25 (1 << 25)
#define TEMP_EOF -1000.0
#define MIN_PER_DAY 1440
#define MAX_DAYS 7

extern volatile float temp;
extern float temp_min[MAX_DAYS];
extern float temp_max[MAX_DAYS];
extern float temp_avg[MAX_DAYS];
extern float temp_sum_per_day;
extern uint temp_flag;
extern uint day_total;
extern uint day_index;
extern float temp_low_limit;
extern float temp_high_limit;
extern uint time_factor;

void init_temp_sensor();
void measure_temp();
void temp_2_display();
char* temp_2_str(float f); // limited, only for 2 digit decimal
void set_time_factor();
void record_temp(int type);
void show_temp_record();
void set_temp_low_limit();
void set_temp_high_limit();
void alarm(int line);
void write_temp_2_display(float temp);
float get_temp(int type);
void fill_temp_list();
void show_temp_list();
float get_current_temp();


#endif