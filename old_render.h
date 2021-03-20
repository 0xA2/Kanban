//
// Created by someone on 3/20/21.
//
#include "tasklist.h"

#ifndef KANBAN_BOARD__OLD_RENDER_H
void formatLine(char *str[], unsigned int sizeLimit);
void printLimit(char d, unsigned int sizeLimit);
void printLine(char *str[], unsigned int sizeLimit);
void printBoard(tasklist *todo, tasklist *doing, tasklist *done);
void printMenu(char *title, char *options[], int count, int ident);

#define KANBAN_BOARD__OLD_RENDER_H

#endif //KANBAN_BOARD__OLD_RENDER_H
