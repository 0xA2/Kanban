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
    l->dateDue = 0;
    l->dateConclusion = 0;
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

void cardSetDateCreated(card* c, long d){
	c->dateCreated = d;
}

void cardSetDateDue(card* c, long d){
	c->dateDue=d;
}

void cardSetDateConcluded(card* c, long d){
	c->dateConclusion=d;
}
