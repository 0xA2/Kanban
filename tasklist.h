#include "card.h"
#ifndef TASK_LIST_H
#define TASK_LIST_H

typedef struct lnode {
  card* task;
  struct lnode *next;
} node;

typedef struct {
  int size;
  node *first;
} tasklist;

node* nodeNew(card*, node*);
tasklist* listNew();
int listIsEmpty(tasklist*);
void listDestroy(tasklist*);
void listAddFirst(card*, tasklist*);
void listAddLast(card*, tasklist*);
card* listGetFirst(tasklist*);
card* listGetLast(tasklist*);
void listRemoveFirst(tasklist*);
void listRemoveLast(tasklist*);
int listSize(tasklist*);
int listContains(card*, tasklist*);
void listAddByPriority(card*, tasklist*);
int listTaskExists(int,tasklist*);
card* listGetTaskByID(int,tasklist*);
card* listRemoveTaskByID(int,tasklist*);
tasklist* loadToDo();
tasklist* loadDoing();
tasklist* loadDone();
tasklist* loadAll();
void listPrintToDo(tasklist*);
void listPrintDoing(tasklist*);
void listPrintDone(tasklist*);

#endif
