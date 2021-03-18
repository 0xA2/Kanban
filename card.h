#ifndef CARD_H
#define CARD_H

typedef struct {
	int id;
	int priority;
	long dateCreated;
	long dateDue;
	long dateConclusion;
	char *description;
	char *person;
} card;

card* cardNew(int, int, long, char*);
void cardAssign(card*, char *);
char* cardGetAssign(card*);
void cardSetDateCreated(card*, long);
void cardSetDateDue(card*, long);
void cardSetDateConcluded(card*, long);
#endif
