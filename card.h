#ifndef _CARD_H_

typedef struct {
    int day, month, year;
} Date;

typedef struct {
    Date creationDate;
    Date finalDate;
    Date effectiveDate;
    int id;
    int priority;
    char *description;
    char *person;
} Card;

void fillCard (Card *c, char *p, char *desc/*, Date st, Date end, int pr*/);
void printCard (Card *c);

#define _CARD_H_

#endif //_CARD_H_
