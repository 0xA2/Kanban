#include <stdio.h>
#include "card.h"

void fillCard (Card *c, char *p, char *desc/*, Date st, Date end, int pr*/)
{
  c->person = p;
  c->description = desc;
  /*c->creationDate = st;
  c->finalDate = end;
  c->priority = pr;*/
}

void printCard (Card *c)
{
  puts (c->person);
  puts (c->description);
}