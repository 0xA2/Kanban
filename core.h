#include "tasklist.h"

#ifndef KANBAN_BOARD__CORE_H

int getCurID(tasklist *todo, tasklist *doing, tasklist *done);
void addTask(tasklist *todo, tasklist *doing, tasklist *done);
void workOnTask(tasklist *todo, tasklist *doing);
void reAssignTask(tasklist *doing);
void closeTask(tasklist *doing, tasklist *done);
void reopenTask(tasklist *todo, tasklist *done);

#define KANBAN_BOARD__CORE_H

#endif //KANBAN_BOARD__CORE_H
