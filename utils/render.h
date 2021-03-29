#include <ncurses.h>
#include <form.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "core.h"

#ifndef KANBAN_BOARD__RENDER_H
void title(WINDOW *, char *);
void initBoard();
void renderBoard();
void printList(WINDOW *, tasklist *, int);
void renderMenu(int);
void addChoice();
void startChoice();
void closeChoice();
void getNextMenu(int);
void choiceLoop();
void render(board_t *);
#define KANBAN_BOARD__RENDER_H

#endif //KANBAN_BOARD__RENDER_H
