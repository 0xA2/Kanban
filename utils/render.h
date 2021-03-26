#include <ncurses.h>
#include <stdlib.h>

#include "core.h"

#ifndef KANBAN_BOARD__RENDER_H
void clearMenu();
void title(WINDOW *, char *);
void initBoard();
void renderBoard();
void refreshInner();
void promptUser(char *);
void readInputInt(int *, char *);
void readInputString(char **, char *);
void printList(WINDOW *, tasklist *);
void printMenu(int);
void renderMenu(int);
void addChoice();
void getNextMenu(int);
void choiceLoop();
void render(board_t *);
#define KANBAN_BOARD__RENDER_H

#endif //KANBAN_BOARD__RENDER_H
