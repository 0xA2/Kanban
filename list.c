#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "card.h"

node *nodeNew(card value, node *n) {
  node *q = (node *) malloc(sizeof(node));
  q->task = value;
  q->next = n;
  return q;
}

list *listNew() {
  list *l = (list *) malloc(sizeof(list));
  if (l != NULL) {
    l->size = 0;
    l->first = NULL;
  }
  return l;
}

int listIsEmpty(list *l) {
  return l->size == 0;
}

void listDestroy(list *l) {
  node *tmp;
  while (!listIsEmpty(l)) {
    tmp = l->first;
    l->first = l->first->next;
    free(tmp);
    l->size--;
  }
  free(l);
}

void listAddFirst(card value, list *l) {
  node *n = nodeNew(value, NULL);
  if (listIsEmpty(l)) {
    l->first = n;
    l->size++;
    return;
  }
  n->next = l->first;
  l->first = n;
  l->size++;
}

void listAddLast(card value, list *l) {
  node *n = nodeNew(value, NULL);
  if (listIsEmpty(l)) {
    l->first = n;
    l->size++;
    return;
  }
  node *q = l->first;
  while (q->next != NULL) {
    q = q->next;
  }
  q->next = n;
  l->size++;
}

card listGetFirst(list *l) {
  // huh que metemos aqui caso seja empty?
  //if (listIsEmpty(l)) { return INT_MIN / 2; }
  return l->first->task;
}

card listGetLast(list *l) {
  // caso seja empty?
  //if (listIsEmpty(l)) { return INT_MIN / 2; }
  node *n = l->first;
  while (n->next != NULL) {
    n = n->next;
  }
  return n->task;
}

void listRemoveFirst(list *l) {
  if (listIsEmpty(l)) {
    return;
  }
  node *n = l->first;
  l->first = l->first->next;
  l->size--;
  free(n);
}

void listRemoveLast(list *l) {
  // Empty List
  if (listIsEmpty(l)) {
    return;
  }
  // List with one element
  if (l->size == 1) {
    node *n = l->first;
    l->first = NULL;
    free(n);
    l->size--;
    return;
  }
  node *n = l->first;
  while (n->next->next != NULL) {
    n = n->next;
  }
  node *q = n->next;
  n->next = NULL;
  free(q);
  l->size--;
}

int listContains(card value, list *l) {
  node *n = l->first;
  while (n->next != NULL) {
    if (n->task.id == value.id) {
      return 1;
    }
    n = n->next;
  }
  if (n->task.id == value.id) {
    return 1;
  }
  return 0;
}

int listIdAtIndex(int index, list *l) {
  if (index < 0 || index >= l->size) {
    return INT_MIN / 2;
  }
  node *n = l->first;
  for (int i = 0; i < index; i++) {
    n = n->next;
  }
  return n->task.id;
}

void listRemoveAtIndex(int index, list *l) {
  if (index < 0 || index >= l->size) {
    return;
  }
  if (index == 0) {
    listRemoveFirst(l);
    return;
  }
  if (index == ((l->size) - 1)) {
    listRemoveLast(l);
    return;
  }
  node *n = l->first;
  for (int i = 0; i < index - 1; i++) {
    n = n->next;
  }
  node *tmp = n->next;
  n->next = n->next->next;
  free(tmp);
  l->size--;
}

void listAddAtIndex(int index, card value, list *l) {
  if (index < 0) {
    return;
  }
  if (index == 0) {
    listAddFirst(value, l);
    return;
  }
  if (index > l->size) {
    listAddLast(value, l);
    return;
  }
  node *newNode = nodeNew(value, NULL);
  node *n = l->first;
  for (int i = 0; i < index - 1; i++) {
    n = n->next;
  }
  newNode->next = n->next;
  n->next = newNode;
  l->size++;
}

void listChangeAtIndex(int index, card value, list *l) {
  if (index < 0 || index >= l->size) {
    return;
  }
  node *n = l->first;
  for (int i = 0; i < index; i++) {
    n = n->next;
  }
  n->task = value;
}

int listSize(list *l) {
  return l->size;
}

// needs work
void listPrint(list *l) {
  if (listIsEmpty(l)) {
    puts("{}");
    return;
  }
  printf("{ ");
  node *n = l->first;
  printf("Task:\n\tDescription: %s\n\tPerson assigned: %s", n->task.description, n->task.person);
  while (n->next != NULL) {
    n = n->next;
    printf("\nTask:\n\tDescription: %s\n\tPerson assigned: %s", n->task.description, n->task.person);
  }
  printf(" }\n");
}