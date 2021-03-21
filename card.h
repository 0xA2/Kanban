#ifndef CARD_H
#define CARD_H

typedef struct {
	int id;
	int priority;
	long dateCreated;
	long deadline;
	long dateConcluded;
	char *description;
	char *person;
} card;

card* cardNew(int, int, long, char*);
void cardAssign(card*, char *);
char* cardGetAssign(card*);
void cardSetDeadline(card*, long);
void cardSetDateConcluded(card*, long);
#endif
