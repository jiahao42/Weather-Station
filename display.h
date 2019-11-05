#ifndef DISPLAY_H_
#define DISPLAY_H_

#define CE (1 << 16)
#define WR (1 << 14)
#define RD (1 << 15)
#define CD (1 << 17)

#define SET_DIR_INPUT *AT91C_PIOC_SODR = (1 << 13)
#define SET_DIR_OUTPUT *AT91C_PIOC_CODR = (1 << 13)
#define ENABLE_CHIP *AT91C_PIOC_CODR = (1 << 12)
#define DISABLE_CHIP *AT91C_PIOC_SODR = (1 << 12);

#define STATUS_CHECK_STA2_READ (read_status_display() & 0x04)
#define STATUS_CHECK_STA3_WRITE (read_status_display() & 0x08)
#define STATUS_CHECK_1 ((read_status_display() & 0x03) != 0x03)
#define AUTO_MODE_STATUS_CHECK_2 (AUTO_MODE_STA2_READ == 0 && AUTO_MODE_STA3_WRITE == 0)

#define TEXTMODE 0
#define GRAPHMODE 1
#define DOUBLEMODE 2

#define FONT0 0
#define FONT1 1
 
extern const unsigned int databus; // databus PC2-9
extern uchar key_value;
extern uint current_mode;

typedef struct font {
  uchar data;
  uint index;
}font;

uchar read_status_display();
void write_command_2_display(uchar command);
void write_data_2_display(uchar data);
void init_display();
uint char_2_font(uchar data);
void write_char_2_display(uchar data);
void fill_one_line_with_char(int line, uchar c);
void write_str_2_display(const char* str);
void write_str_2_display_with_free(char* str);
void write_2_digits_2_display(int num);
void write_3_digits_2_display(int num);
void write_pressure_digits_2_display(float num);
void write_test();
void reset_screen(int line);
void set_line_on_display(int line);
void set_position_on_display(int x, int y);
void show_menu();
void switch_display_mode(int mode);
void switch_font_table(int mode);


#endif