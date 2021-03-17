#ifndef _CARD_H_
/*
typedef struct {
  int day, month, year;
} date;
*/

typedef struct {
  // date with time???
  int id;
  int priority;
  char *description;
  char *person;
} card;

card *cardNew(int, int, char *);
void cardAssign(card *, char *);
void cardPrint(card *c);

#define _CARD_H_

#endif //_CARD_H_
