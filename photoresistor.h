#ifndef PHOTORESISTOR_H_
#define PHOTORESISTOR_H_

#define PA3 (1 << 3) // Channel 3
#define PA4 (1 << 4) // Channel 4
#define CHANNEL3 (1 << 3)
#define CHANNEL4 (1 << 4)
#define ADC_DRDY ((*AT91C_ADCC_SR & (1 << 24)) == (1 << 24))
#define ADC_CH3_DRDY ((*AT91C_ADCC_SR & (1 << 3)) == (1 << 3))
#define ADC_CH4_DRDY ((*AT91C_ADCC_SR & (1 << 4)) == (1 << 4))

void init_photoresistor();
int get_lux_4();
int get_lux_3();
void lux_2_display();
char* lux_2_str(int lux);



#endif