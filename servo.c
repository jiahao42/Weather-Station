#include "util.h"
#include "servo.h"

void init_servo() {
  *AT91C_PMC_PCER1 = (1 << 4); // Start the peripheral clock for Pulse Width Modulation Controller, id = 36
  *AT91C_PMC_PCER1 = (1 << 5); // Start the peripheral clock for ADC Controller, id = 37
  *AT91C_PMC_PCER = (1 << 12); // Start the peripheral clock for port B

  *AT91C_PIOB_WPMR = (0x50494F << 8); // "PIO" in ASCII, disable write protect
  *AT91C_PIOB_PDR = (1 << 17); // Let peripheral control the pin REG_PIOB_PDR
  *AT91C_PIOB_ABMR = (1 << 17); // Activate peripheral B to control the pin in REG_PIOB_ABSR
  *AT91C_PIOB_OER = (1 << 17); // Enable output for PB17
  *AT91C_PWMC_WPCR = (0 |  // WPCMD, unlock write protection
                      0x50574d << 8); // WPKEY, "PWM" in ASCII
  *AT91C_PWMC_CH1_CMR = 0x05; // Set Pre-scaler to Master_CLK/32 in PWM_CMRx 84MHz/32 = 2.625MHz, 0 = left-aligned
  /*
   * 84 / 32 = 2.625MHz
   * 1 / 2.625 = 0.381 * 10^-6s
   * 20 * 10^-3 / (0.381 * 10^-6) = 52500 = 0xCD14
   */
  *AT91C_PWMC_CH1_CPRDR = 0xCD14; // Write a value to PWM_CPRD (20ms)
  /*
   * 84 / 32 = 2.625MHz
   * 1 / 2.625 = 0.381 * 10^-6s
   * 1 * 10^-3 / (0.381 * 10^-6) = 0xA41
   */
  *AT91C_PWMC_CH1_CDTYR = 0x72D; // Write a value to PWM_CDTY (0.7ms);
  *AT91C_PWMC_ENA = (1 << 1); // Enable PWM-channel in ENA_CHID1
}

void servo_move_forward(int value) {
  *AT91C_PWMC_CH1_CDTYUPDR = value; // Write a value to PWM_CDTY
  *AT91C_PWMC_UPCR = 0x01; // PWM Update Control Register
}

int left;
int right;
void track_sun_position() {
  int min_position = 0x72D;
  int max_position = 0x1893;
  int current_position = min_position;
#ifdef GRADE3
  reset_screen(0);
  set_line_on_display(0);
  write_str_2_display("Tracking sun position...");
  int i;
  int min_lux = 10000;
  int current_lux = 10000;
  for (i = 1; i < 18; i++) { // 0.7 - 2.3ms
    servo_move_forward(0x72D + 0x106 * i);
    current_lux = get_lux_3();
    if (current_lux < min_lux) {
      min_lux = current_lux;
      current_position = 0x72D + 0x106 * i;
    }
    delay(2000000);
  }
  servo_move_forward(current_position);
#else

  reset_screen(0);
  delay(2000000);
  set_line_on_display(0);
  write_str_2_display("Tracking sun position...");
  set_line_on_display(4);
  write_str_2_display("Press * to return");
  delay(2000000);
  uchar temp_key_value;
  while ((temp_key_value = read_key()) != '*') {
    left = get_lux_3() - 1800; // TODO: Adjust according to surrounding
    delay(200000);
    right = get_lux_4();
    if (right - left > 300) { // left < 2500
      current_position = (current_position - 0x106) < min_position ? min_position : current_position - 0x106;
      if (current_position <= min_position) {
        servo_move_forward(min_position);
      } else {
        servo_move_forward(current_position);
      }
    } else if (left - right > 300) { // (left - right > 500)
      current_position = (current_position + 0x106) > max_position ? max_position : current_position + 0x106;
      if (current_position >= max_position) {
        servo_move_forward(max_position);
      } else {
        servo_move_forward(current_position);
      }
    }
    delay(3000000);
  }
#endif
}