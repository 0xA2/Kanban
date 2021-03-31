#include "./form_tools.h"
#include <form.h>
#include <ncurses.h>

#ifndef KANBAN_BOARD__RENDER_H
typedef struct field_info {
  char *type;
  char *label;
  int number;
} field_i;

void addChoice();
void startChoice();
void closeChoice();
void choiceLoop();
void reAssign();
void reopen();

void renderMenu(int);
void renderBoard();
void render(board_t *);
#define KANBAN_BOARD__RENDER_H

#endif //KANBAN_BOARD__RENDER_H
