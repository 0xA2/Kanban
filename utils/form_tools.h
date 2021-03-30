#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <menu.h>
#include <form.h>
#include <assert.h>

#include "core.h"

#ifndef KANBAN_BOARD__FORMS_H
int isLeap(int year);
int isValidDate(int, int, int);
int readInt(int *, char *);

char *trimWhitespaces(char *);

FIELD *newFieldPrompt(int, char *);
FIELD *newFieldInputInt(int, int, int);
FIELD *newFieldInputStr(int);
FIELD *newFieldInputDate(int);
FIELD *newFieldButton(int, char *);

void title(WINDOW **, char *);
void printList(WINDOW *, tasklist *, int);
void driver(FORM ***, WINDOW **);
void renderForm(FORM **, FIELD ***);
#define KANBAN_BOARD__FORMS_H

#endif //KANBAN_BOARD__FORMS_H
