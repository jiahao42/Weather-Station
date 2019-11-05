#ifndef GRAPH_H_
#define GRAPH_H_

#include "util.h"
#include "temp.h"
#include "display.h"

extern uint day_total;

typedef enum {
  min_graph = '7',
  max_graph = '8',
  avg_graph = '9',
}graph_type;

void draw_x_labels();
void draw_y_labels(int high, int low);
void draw_content(float* arr_min, float* arr_max, float* arr_avg, int day_total, float max, float min);

void draw_graph(float* arr_min, float* arr_max, float* arr_avg, int day_total) { // 3 types: min/max/avg
  switch_font_table(FONT0); // switch font table
  reset_screen(0);
  int i;
  int key;
  int day = day_total > 6 ? 6 : day_total;
  delay(1000000);
  while (1) {
    float max = -1000000, min = 10000000;
    key = read_key();
    key_value = key == 0 ? key_value : key;
    if (key_value == '*')
        break;
    
    draw_x_labels();
    set_position_on_display(11, 0);
    switch (key_value) {
    default:  // min graph
      write_str_2_display("MIN GRAPH");
      for (i = 0; i <= day; i++) {
        max = max > arr_min[i]? max : arr_min[i];
        min = min < arr_min[i]? min : arr_min[i];
      }
      break;
    case max_graph:
      write_str_2_display("MAX GRAPH");
      for (i = 0; i <= day; i++) {
        max = max > arr_max[i]? max : arr_max[i];
        min = min < arr_max[i]? min : arr_max[i];
      }
      break;
    case avg_graph:
      write_str_2_display("AVG GRAPH");
      for (i = 0; i <= day; i++) {
        max = max > arr_avg[i]? max : arr_avg[i];
        min = min < arr_avg[i]? min : arr_avg[i];
      }
      break;
    }
    float low_point;
    float high_point;
    if (max > 10000) { // It is pressure
      low_point = min - 10; // of graph
      high_point = max + 10; // of graph
      // draw_y_labels((int)high_point, (int)low_point);
    } else {
      low_point = min - 3; // of graph
      high_point = max + 3; // of graph
      draw_y_labels((int)high_point, (int)low_point);
    }
    draw_content(arr_min, arr_max, arr_avg, day_total, high_point, low_point);
  }
  switch_font_table(FONT1); // switch font table back
}

void draw_content(float* arr_min, float* arr_max, float* arr_avg, int day_total, float max, float min) {
  int range = appro_equal(max - min);
  int i, j;
  int day = day_total > 6 ? 6 : day_total;
  for (i = 0; i <= day; i++) {
    float current_value;
    switch(key_value) {
    default:
      current_value = (int)arr_min[i];
      break;
    case max_graph:
      current_value = (int)arr_max[i];
      break;
    case avg_graph:
      current_value = (int)arr_avg[i];
      break;
    }
    
    int height = appro_equal((current_value - min) * 14 / range ); // height of the bar
    height = 0x0e - height;
    for (j = 1; j < 0x0f; j++) {
      int indent = 4 * i + 3;
      set_position_on_display(indent, j);
      if (j >= height)
        write_char_2_display('e');
      else 
        write_char_2_display(' ');
      write_char_2_display(' ');
    }
  }
}

void draw_x_labels() {
  set_line_on_display(0x0f);
  write_str_2_display("  ");
  int i;
  for (i = 0; i < 7; i++) {
    write_str_2_display(" ");
    write_str_2_display("D");
    write_char_2_display(i + '0');
    write_str_2_display(" ");
  }
}


void draw_y_labels(int high, int low) {
  set_line_on_display(0x01);
  write_2_digits_2_display(high);
  set_line_on_display(0x0e);
  write_2_digits_2_display(low);
  set_line_on_display(0x08);
  write_2_digits_2_display((low + high) / 2);
}









#endif