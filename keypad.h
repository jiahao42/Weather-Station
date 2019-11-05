#ifndef KEYPAD_H_
#define KEYPAD_H_

extern const uint col; // Column: PC7/8/9
extern const uint row; // Row: PC2/3/4/5
extern uchar key_value;

void init_keypad();
uchar read_key();
void keypad_2_display();

#endif
