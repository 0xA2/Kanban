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
void addTask(int, char *);
int workOnTask(int, int, int, int, char *);
int closeTask(int);
int reassignTask(int, char *);
int reopenTask(int);
#define KANBAN_BOARD_UTILS_CORE_H_

#endif //KANBAN_BOARD_UTILS_CORE_H_
