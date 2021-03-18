#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "tasklist.h"
#include "card.h"

node* nodeNew(card* c, node* n) {
  node* q = (node*) malloc(sizeof(node));
  q->task = c;
  q->next = n;
  return q;
}

tasklist* listNew() {
  tasklist* l = (tasklist*) malloc(sizeof(tasklist));
  if (l != NULL) {
    l->size = 0;
    l->first = NULL;
  }
  return l;
}

int listIsEmpty(tasklist* l) {
  return l->size == 0;
}

void listDestroy(tasklist* l) {
  node* tmp;
  while (!listIsEmpty(l)) {
    tmp = l->first;
    l->first = l->first->next;
    free(tmp);
    l->size--;
  }
  free(l);
}

void listAddFirst(card* c, tasklist *l) {
  node *n = nodeNew(c, NULL);
  if (listIsEmpty(l)) {
    l->first = n;
    l->size++;
    return;
  }
  n->next = l->first;
  l->first = n;
  l->size++;
}

void listAddLast(card* c, tasklist *l) {
  node* n = nodeNew(c, NULL);
  if (listIsEmpty(l)) {
    l->first = n;
    l->size++;
    return;
  }
  node* q = l->first;
  while (q->next != NULL) {
    q = q->next;
  }
  q->next = n;
  l->size++;
}

card* listGetFirst(tasklist* l) {
  if (listIsEmpty(l)) { return NULL; }
  return l->first->task;
}

card* listGetLast(tasklist *l) {
  if (listIsEmpty(l)) { return NULL; }
  node *n = l->first;
  while (n->next != NULL) {
    n = n->next;
  }
  return n->task;
}

void listRemoveFirst(tasklist *l) {
  if (listIsEmpty(l)) {
    return;
  }
  node *n = l->first;
  l->first = l->first->next;
  l->size--;
  free(n);
}

void listRemoveLast(tasklist *l) {
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
  node* n = l->first;
  while (n->next->next != NULL) {
    n = n->next;
  }
  node* q = n->next;
  n->next = NULL;
  free(q);
  l->size--;
}

int listContains(card* c, tasklist *l) {
  node *n = l->first;
  while (n->next != NULL) {
    if (n->task->id == c->id) {
      return 1;
    }
    n = n->next;
  }
  if (n->task->id == c->id) {
    return 1;
  }
  return 0;
}

int listSize(tasklist* l) {
  return l->size;
}

void listAddByPriority(card* c, tasklist* l){

	node* newNode = nodeNew(c, NULL);

	// List is empty
	if (listIsEmpty(l)){listAddFirst(c,l); return;}

	// Node to add has more priority than first
	if(c -> priority > listGetFirst(l) -> priority ){ listAddFirst(c,l); return;}

	// Node to add has less priority than last
	if(c -> priority < listGetLast(l) -> priority ){ listAddLast(c,l); return;}

	// Node to add has same priority as first
	if(c -> priority == listGetFirst(l) -> priority){
		if(c -> dateCreated <= listGetFirst(l) -> dateCreated){ listAddFirst(c,l); return;}
		if(c -> dateCreated > listGetFirst(l) -> dateCreated){
			newNode -> next = l -> first -> next;
			l -> first -> next = newNode;
			l -> size++;
			return;
		}
	}

	// List with two or more tasks
	node* cur = l -> first;
	node* after = l -> first -> next;
	while(after -> next != NULL) {
		if(c -> priority < after -> task -> priority){ cur = after; after = after -> next;}
		if(c -> priority > after -> task -> priority){
			cur -> next = newNode;
			newNode -> next = after;
			l -> size++;
			return;
		}
		if(c -> priority == after -> task -> priority){
			if(c -> dateCreated <= after -> task -> dateCreated){
				cur -> next = newNode;
				newNode -> next = after;
				l -> size++;
				return;
			}
			if(c -> dateCreated > after -> task -> dateCreated){
				newNode -> next = after -> next;
				after -> next = newNode;
				l -> size++;
				return;
			}
		}
	}

	// Node to add has more priority than last
	if(c -> priority > after -> task -> priority){
		cur -> next = newNode;
		newNode -> next = after;
		l -> size++;
		return;
	}

	// Node to add has the same priority as last
	if(c -> priority == after -> task -> priority){
		if(c -> dateCreated <= after -> task -> priority){
			cur -> next = newNode;
			newNode -> next = after;
			l -> size++;
			return;
		}
		if(c -> dateCreated > after -> task -> priority){
			after -> next = newNode;
			l -> size++;
			return;
		}
	}
}

int listTaskExists(int i, tasklist* l){
	if(listIsEmpty(l)){return 0;}
	node* n = l -> first;
	while(n -> next != NULL){
		if(n -> task -> id == i){ return 1; }
		n = n -> next;
	}
	if(n -> task -> id == i){ return 1; }
	return 0;
}

card* listGetTaskByID(int i, tasklist* l){
	if(listIsEmpty(l)){return NULL;}
	node* n = l -> first;
	while(n -> next != NULL){
		if(n -> task -> id == i){ return n -> task; }
		n = n -> next;
	}
	if(n -> task -> id == i){ return n -> task; }
	return 0;
}

card* listRemoveTaskByID(int i, tasklist* l){

	// Emtpy list
	if(listIsEmpty(l)){ return NULL; }

	// Test first node
	if(l -> first -> task -> id == i){
		card* ret = l -> first -> task;
		listRemoveFirst(l);
		return ret;
	}

	node* cur = l -> first;
	node* after = l -> first -> next;
	if(after != NULL){
		while(after -> next != NULL){
			if(after -> task -> id == i){
				card* ret = after -> task;
				cur -> next = after -> next;
				l -> size--;
				return ret;
			}
			cur = after;
			after = after -> next;
		}

		// Test last node
		if(after -> task -> id == i){
			card* ret = after -> task;
			listRemoveLast(l);
			return ret;
		}
	}
	return NULL;
}

// Load from todo.txt
tasklist* loadToDo(){
	tasklist* ret = listNew();
	return ret;
}

// Load from doing.txt
tasklist* loadDoing(){
	tasklist* ret = listNew();
	return ret;
}

// Load from done.txt
tasklist* loadDone(){
	tasklist* ret = listNew();
	return ret;
}

// Load from all.txt
tasklist* loadAll(){
	tasklist* ret = listNew();
	return ret;
}

void listPrintToDo(tasklist *l) {
  if (listIsEmpty(l)) {
    return;
  }
  node* n = l->first;
  puts("To Do:");
  printf("\tID: %d\n\tPriority %d: %s", n->task->id,n->task->priority ,n->task->description);
  while (n->next != NULL) {
    n = n->next;
    printf("\n\tID: %d\n\tPriority %d: %s", n->task->id, n->task->priority ,n->task->description);
  }
  printf("\n");
}

void listPrintDoing(tasklist *l) {
  if (listIsEmpty(l)) {
    return;
  }
  node* n = l->first;
  puts("Working on:");
  printf("\n\tPriority %d: %s\n\tAssigned to: %s", n->task->priority ,n->task->description, n->task->person);
  while (n->next != NULL) {
    n = n->next;
    printf("\n\n\tPriority %d: %s\n\tAssigned to: %s", n->task->priority ,n->task->description, n->task->person);
  }
  printf("\n");
}

void listPrintDone(tasklist *l) {
  if (listIsEmpty(l)) {
    return;
  }
  node* n = l->first;
  puts("Tasks done");
  printf("\n\t%s\n\tCompleted by : %s", n->task->description, n->task->person);
  while (n->next != NULL) {
    n = n->next;
    printf("\n\n\t%s\n\tCompleted by: %s", n->task->description, n->task->person);
  }
  printf("\n");
}
