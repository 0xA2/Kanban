#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils/tasklist.h"

#ifndef KANBAN_BOARD_UTILS_CORE_H_
typedef struct board {
  tasklist *all;
  tasklist *todo;
  tasklist *doing;
  tasklist *done;
} board_t;

void initCore(board_t *board_init);

int readInt(int *, char *);
void readString(char *);

/*
void printBoard();
void printMenu(char *title, char *options[], int count, int ident);
*/

void addTask(int, char *);
int workOnTask(int, int, int, int, char *);
void reassignTask();
// void closeTask(int id);
void reopenTask();
void tasksFromWorker();
void tasksByDate();
#define KANBAN_BOARD_UTILS_CORE_H_

#endif //KANBAN_BOARD_UTILS_CORE_H_
