#include "card.h"
#ifndef LIST_H
#define LIST_H

typedef struct lnode {
  card* task;
  struct lnode *next;
} node;

typedef struct {
  int size;
  node *first;
} list;

node* nodeNew(card*, node*);
list* listNew();
int listIsEmpty(list*);
void listDestroy(list*);
void listAddFirst(card*, list*);
void listAddLast(card*, list*);
card* listGetFirst(list*);
card* listGetLast(list*);
void listRemoveFirst(list*);
void listRemoveLast(list*);
int listValueAtIndex(int, list*);
void listRemoveAtIndex(int, list*);
void listAddAtIndex(int, card*, list*);
void listChangeAtIndex(int, card*, list*);
int listSize(list*);
int listContains(card*, list*);
list* loadToDo();
list* loadDoing();
list* loadDone();
void listPrint(list*);

#endif
