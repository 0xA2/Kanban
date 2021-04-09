#include "./form_tools.h"

#ifndef KANBAN_BOARD__RENDER_H

typedef struct field_info {
  char *type;
  char *label;
  int number;
} FIELD_INFO;

void nuke();
void redrawBoards();
void redrawForms();
void driver(FORM *, FIELD **);

void getNextMenu(int);
void addChoice();
void startChoice();
void closeChoice();
void choiceLoop();
void reAssign();
void reopen();

FORM *renderForm(struct field_info *, int);
void renderPerson(char *);
void renderAll();
void renderMenu(int);
void renderBoard();
void render(board_t *);
#define KANBAN_BOARD__RENDER_H

#endif //KANBAN_BOARD__RENDER_H
