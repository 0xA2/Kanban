#include "./form_tools.h"

#ifndef KANBAN_BOARD__RENDER_H
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
