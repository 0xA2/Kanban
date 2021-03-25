#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "card.h"

card* cardNew(int i, int pri, long time ,char *desc) {
  card *l = (card*)malloc(sizeof(card));
  if (l != NULL) {
    l->id = i;
	if(pri > 10){ return NULL;}
	else if(pri < 1){ return NULL;}
    else{l->priority = pri;}
    l->dateCreated = time;
    l->deadline = 0;
    l->dateConcluded = 0;
    l->description = desc;
    l->person = NULL;
  }
  return l;
}

void cardAssign(card* c, char* p) {
  c->person = p;
}

char* cardGetAssign(card* c){
	return c -> person;
}

void cardSetDeadline(card* c, long d){
	c->deadline=d;
}

void cardSetDateConcluded(card* c, long d){
	c->dateConcluded=d;
}
