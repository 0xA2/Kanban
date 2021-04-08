#include <ncurses.h>
#include <ctype.h>
#include <form.h>
#include <assert.h>

#include "./core.h"

#ifndef KANBAN_BOARD__FORMS_H
int isLeap(int year);
int isValidDate(int, int, int);
int readInt(int *, char *);
char *trimWhitespaces(char *);
void title(WINDOW *, char *);
void printList(WINDOW *, tasklist *, int);
void printListByPerson(WINDOW*, tasklist*, char*);
#define KANBAN_BOARD__FORMS_H

#endif //KANBAN_BOARD__FORMS_H
