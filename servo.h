#ifndef SERVO_H_
#define SERVO_H_

#include "photoresistor.h"
#include "keypad.h"
#include "display.h"


void init_servo();
void servo_move_forward(int value);
void track_sun_position();


#endif