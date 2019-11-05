#ifndef TEMP_LIST_H_
#define TEMP_LIST_H_

#include "stdio.h"
#include "stdlib.h"

typedef struct temp_node {
  float temp;  // 4 bytes
  struct temp_node *next;  // 4 bytes
} temp_node;

typedef struct temp_list {
  temp_node *head; // 4 bytes
  temp_node *tail; // 4 bytes
  temp_node *cursor; // 4 bytes
  float current_temp; // 4 bytes
  int full_flag; // 4 bytes
  int size; // 4 bytes
} temp_list; // 24 bytes

void init_node(temp_node *node, float temp) {
  node->temp = temp;
  node->next = NULL;
}

temp_list *create_list() {
  temp_list *list = (temp_list *)malloc(sizeof(temp_list));
  list->head = NULL;
  list->tail = NULL;
  list->cursor = NULL;
  list->full_flag = 0;
  list->size = -1;
  list->current_temp = 0;
  return list;
}

temp_node *create_node(float temp) {
  temp_node *node = (temp_node *)malloc(sizeof(temp_node));
  if (node == NULL) {
    return NULL;
  }
  init_node(node, temp);
  return node;
}

void destroy_node(temp_node *node) {
  free(node);
  node = NULL;
}

// TODO, imperfect method
void append_temp(temp_list *list, float temp) {
  if (list->full_flag == 0) { // memory not full
    temp_node *node = create_node(temp);
    if (node == NULL) { // if memory allocation failed, then rewrite from start
      list->cursor = list->head;
      list->full_flag = 1;
    } else { // if success then append
      if (list->head == NULL) {
        list->head = node;
        list->tail = node;
        list->cursor = node;
        list->current_temp = temp;
      } else {
        list->tail->next = node;
        list->tail = node;
        list->cursor = node;
        list->current_temp = temp;
      }
      list->size++;
    }
  } else { // memory is full
    list->cursor->temp = temp;
    list->current_temp = temp;
    if (list->cursor == list->tail) {
      list->cursor = list->head; // rewrite from start
    } else {
      list->cursor = list->cursor->next; // move to the next position
    }
  }
};

void destroy_list(temp_list *list) {
  temp_node *cursor = list->head; // start from head
  temp_node *next = cursor;
  for (; next != NULL; cursor = next) {
    next = cursor->next;
    destroy_node(cursor);
  }
}

void print_list(temp_list *list) {
  if (list->full_flag == 1) {
    temp_node *cursor = list->cursor; // start from cursor
    printf("%f ", cursor->temp);
    cursor = cursor->next; // next node
    for (; cursor != list->cursor; cursor = cursor->next) {
      if (cursor == NULL) { // if meet the end, start from the head
        cursor = list->head;
      }
      printf("%f ", cursor->temp);
    }
  } else {
    temp_node* cursor = list->head;
    for (; cursor != NULL; cursor = cursor->next) {
      printf("%f ", cursor->temp);
    }
  }
  printf("\n");
}



#endif