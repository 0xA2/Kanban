#include <stdio.h>
#include <stdlib.h>
#include "card.h"

card *cardNew(int id, int priority, char *desc) {
  card *l = (card *) malloc(sizeof(card));
  if (l != NULL) {
    l->id = id;
    l->priority = priority;
    l->description = desc;
    l->person = NULL; // not assigned yet
  }
  return l;
}

void cardAssign(card *c, char *p) {
  c->person = p;
}

void cardPrint(card *c) {
  puts(c->person);
  puts(c->description);
}