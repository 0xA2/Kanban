#include "card.h"
#ifndef _LIST_H_

typedef struct lnode {
  card task;
  struct lnode *next;
} node;

typedef struct {
  int size;
  node *first;
} list;

node *nodeNew(card, node *);
list *listNew();
int listIsEmpty(list *);
void listDestroy(list *);
void listAddFirst(card, list *);
void listAddLast(card, list *);
card listGetFirst(list *);
card listGetLast(list *);
void listRemoveFirst(list *);
void listRemoveLast(list *);
int listValueAtIndex(int, list *);
void listRemoveAtIndex(int, list *);
void listAddAtIndex(int, card, list *);
void listChangeAtIndex(int, card, list *);
int listSize(list *);
int listContains(card, list *);
void listPrint(list *);

#endif //_LIST_H_