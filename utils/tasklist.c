#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "card.h"
#include "tasklist.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))

node *nodeNew(card *c, node *n) {
  node *q = (node *) malloc(sizeof(node));
  q->task = c;
  q->next = n;
  return q;
}

tasklist *listNew() {
  tasklist *l = (tasklist *) malloc(sizeof(tasklist));
  if (l != NULL) {
    l->size = 0;
    l->first = NULL;
    return l;
  }
  return NULL;
}

int listIsEmpty(tasklist *l) {
  return l->size == 0;
}

void listDestroy(tasklist *l) {
  node *tmp;
  while (!listIsEmpty(l)) {
    tmp = l->first;
    l->first = l->first->next;
    free(tmp);
    l->size--;
  }
  free(l);
}

void listAddFirst(card *c, tasklist *l) {
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

void listAddLast(card *c, tasklist *l) {
  node *n = nodeNew(c, NULL);
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

card *listGetFirst(tasklist *l) {
  if (listIsEmpty(l)) {
    return NULL;
  }
  return l->first->task;
}

card *listGetLast(tasklist *l) {
  if (listIsEmpty(l)) {
    return NULL;
  }
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
  node *n = l->first;
  while (n->next->next != NULL) {
    n = n->next;
  }
  node *q = n->next;
  n->next = NULL;
  free(q);
  l->size--;
}

int listSize(tasklist *l) {
  return l->size;
}

// Adds new task to list in order of task priority
void listAddByPriority(card *c, tasklist *l) {

  node *newNode = nodeNew(c, NULL);

  // List is empty
  if (listIsEmpty(l)) {
    listAddFirst(c, l);
    return;
  }

  // Node to add has more priority than first
  if (c->priority > listGetFirst(l)->priority) {
    listAddFirst(c, l);
    return;
  }

  // Node to add has less priority than last
  if (c->priority < listGetLast(l)->priority) {
    listAddLast(c, l);
    return;
  }

  // Node to add has same priority as first
  if (c->priority == listGetFirst(l)->priority) {
    if (c->dateCreated <= listGetFirst(l)->dateCreated) {
      listAddFirst(c, l);
      return;
    }
    if (c->dateCreated > listGetFirst(l)->dateCreated) {
      newNode->next = l->first->next;
      l->first->next = newNode;
      l->size++;
      return;
    }
  }

  // List with two or more tasks
  node *cur = l->first;
  node *after = l->first->next;
  while (after->next != NULL) {
    if (c->priority < after->task->priority) {
      cur = after;
      after = after->next;
    }
    if (c->priority > after->task->priority) {
      cur->next = newNode;
      newNode->next = after;
      l->size++;
      return;
    }
    if (c->priority == after->task->priority) {
      if (c->dateCreated <= after->task->dateCreated) {
        cur->next = newNode;
        newNode->next = after;
        l->size++;
        return;
      }
      if (c->dateCreated >= after->task->dateCreated) {
        newNode->next = after->next;
        after->next = newNode;
        l->size++;
        return;
      }
    }
  }

  // Node to add has more priority than last
  if (c->priority > after->task->priority) {
    cur->next = newNode;
    newNode->next = after;
    l->size++;
    return;
  }

  // Node to add has the same priority as last
  if (c->priority == after->task->priority) {
    if (c->dateCreated <= after->task->priority) {
      cur->next = newNode;
      newNode->next = after;
      l->size++;
      return;
    }
    if (c->dateCreated > after->task->priority) {
      after->next = newNode;
      l->size++;
      return;
    }
  }
}

// Adds new task to list in order of date created
void listAddByDate(card *c, tasklist *l) {
  node *newNode = nodeNew(c, NULL);

  // List is empty
  if (listIsEmpty(l)) {
    listAddFirst(c, l);
    return;
  }

  // Node to add has earlier date than first
  if (c->dateCreated <= listGetFirst(l)->dateCreated) {
    listAddFirst(c, l);
    return;
  }

  // Node to add has later date than last
  if (c->dateCreated >= listGetLast(l)->dateCreated) {
    listAddLast(c, l);
    return;
  }

  // List with two or more tasks
  node *cur = l->first;
  node *after = l->first->next;
  while (after->next != NULL) {
    if (c->dateCreated > after->task->dateCreated) {
      cur = after;
      after = after->next;
    }
    if (c->dateCreated <= after->task->dateCreated) {
      cur->next = newNode;
      newNode->next = after;
      l->size++;
      return;
    }
  }

  // Node to add has date earlier than last
  if (c->dateCreated < after->task->dateCreated) {
    cur->next = newNode;
    newNode->next = after;
    l->size++;
  }
}

// Adds new task to list in order of name of worker
void listAddByName(card *c, tasklist *l) {
  node *newNode = nodeNew(c, NULL);

  // List is empty
  if (listIsEmpty(l)) {
    listAddFirst(c, l);
    return;
  }

  // List with one task
  if (strncmp(c->person, listGetFirst(l)->person, min(strlen(c->person), strlen(listGetFirst(l)->person))) <= 0) {
    listAddFirst(c, l);
    return;
  }

  if (strncmp(c->person, listGetLast(l)->person, min(strlen(c->person), strlen(listGetLast(l)->person))) > 0) {
    listAddLast(c, l);
    return;
  }

  // List with two or more tasks
  node *cur = l->first;
  node *after = l->first->next;
  while (after->next != NULL) {
    if (strncmp(c->person, after->task->person, min(strlen(c->person), strlen(after->task->person))) > 0) {
      cur = after;
      after = after->next;
    }
    if (strncmp(c->person, after->task->person, min(strlen(c->person), strlen(after->task->person))) <= 0) {
      cur->next = newNode;
      newNode->next = after;
      l->size++;
      return;
    }
  }

  // Check of node should be added before last
  if (strncmp(c->person, after->task->person, min(strlen(c->person), strlen(after->task->person))) <= 0) {
    cur->next = newNode;
    newNode->next = after;
    l->size++;
  }
}

// Adds new task to list in order of date of conclusion
void listAddByConclusion(card *c, tasklist *l) {
  node *newNode = nodeNew(c, NULL);

  // List is empty
  if (listIsEmpty(l)) {
    listAddFirst(c, l);
    return;
  }

  // Node to add has earlier date than first
  if (c->dateConcluded <= listGetFirst(l)->dateConcluded) {
    listAddFirst(c, l);
    return;
  }

  // Node to add has later date than last
  if (c->dateConcluded >= listGetLast(l)->dateConcluded) {
    listAddLast(c, l);
    return;
  }

  // List with two or more tasks
  node *cur = l->first;
  node *after = l->first->next;
  while (after->next != NULL) {
    if (c->dateConcluded > after->task->dateConcluded) {
      cur = after;
      after = after->next;
    }
    if (c->dateConcluded <= after->task->dateConcluded) {
      cur->next = newNode;
      newNode->next = after;
      l->size++;
      return;
    }
  }

  // Node to add has date earlier than last
  if (c->dateConcluded < after->task->dateConcluded) {
    cur->next = newNode;
    newNode->next = after;
  }
}

// Return 1 if task with give id exits
int listTaskExists(int i, tasklist *l) {
  if (listIsEmpty(l)) {
    return 0;
  }
  node *n = l->first;
  while (n->next != NULL) {
    if (n->task->id == i) {
      return 1;
    }
    n = n->next;
  }
  if (n->task->id == i) {
    return 1;
  }
  return 0;
}

// Return task with given id
card *listGetTaskByID(int i, tasklist *l) {
  if (listIsEmpty(l)) {
    return NULL;
  }
  node *n = l->first;
  while (n->next != NULL) {
    if (n->task->id == i) {
      return n->task;
    }
    n = n->next;
  }
  if (n->task->id == i) {
    return n->task;
  }
  return 0;
}

// Return pointer to card with given id after removing it from list
card *listRemoveTaskByID(int i, tasklist *l) {

  // Emtpy list
  if (listIsEmpty(l)) {
    return NULL;
  }

  // Test first node
  if (l->first->task->id == i) {
    card *ret = l->first->task;
    listRemoveFirst(l);
    return ret;
  }

  node *cur = l->first;
  node *after = l->first->next;
  if (after != NULL) {
    while (after->next != NULL) {
      if (after->task->id == i) {
        card *ret = after->task;
        cur->next = after->next;
        free(after);
        l->size--;
        return ret;
      }
      cur = after;
      after = after->next;
    }

    // Test last node
    if (after->task->id == i) {
      card *ret = after->task;
      listRemoveLast(l);
      return ret;
    }
  }
  return NULL;
}

// Load from todo.txt
tasklist *loadToDo(tasklist *all) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen("todo.txt", "r");
  if (fp == NULL) {
    return listNew();
  }
  tasklist *ret;
  if ((ret = listNew()) != NULL) {
    while ((read = getline(&line, &len, fp)) != -1) {
      line[strcspn(line, "\n")] = 0;
      char *cur;
      char *end;
      int id = -1;
      int priority = -1;
      long dateCreated = -1;
      char *desc;
      cur = strtok(line, "|");
      if (cur != NULL) {
        id = (int) strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        priority = (int) strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        dateCreated = strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        if (strlen(cur) >= 1024) {
          exit(1);
        }
        desc = (char *) malloc(strlen(cur) + 1);
        strcpy(desc, cur);
      } else {
        exit(1);
      }
      card *c = cardNew(id, priority, dateCreated, desc);
      listAddByPriority(c, ret);
      listAddByDate(c, all);

    }
    fclose(fp);
    return ret;
  }
  fclose(fp);
  return NULL;
}

// Load from doing.txt
tasklist *loadDoing(tasklist *all) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen("doing.txt", "r");
  if (fp == NULL) {
    return listNew();
  }
  tasklist *ret;
  if ((ret = listNew()) != NULL) {
    while ((read = getline(&line, &len, fp)) != -1) {
      line[strcspn(line, "\n")] = 0;
      char *cur;
      char *end;
      int id = -1;
      int priority = -1;
      long dateCreated = -1;
      long deadline = -1;
      char *desc;
      char *worker;
      cur = strtok(line, "|");
      if (cur != NULL) {
        id = (int) strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        priority = (int) strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        dateCreated = strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        deadline = strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        if (strlen(cur) >= 1024) {
          exit(1);
        }
        desc = (char *) malloc(strlen(cur) + 1);
        strcpy(desc, cur);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        if (strlen(cur) >= 1024) {
          exit(1);
        }
        worker = (char *) malloc(strlen(cur) + 1);
        strcpy(worker, cur);
      } else {
        exit(1);
      }
      card *c = cardNew(id, priority, dateCreated, desc);
      cardAssign(c, worker);
      cardSetDeadline(c, deadline);
      listAddByName(c, ret);
      listAddByDate(c, all);

    }
    fclose(fp);
    return ret;
  }
  fclose(fp);
  return NULL;
}

// Load from done.txt
tasklist *loadDone(tasklist *all) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  fp = fopen("done.txt", "r");
  if (fp == NULL) {
    return listNew();
  }
  tasklist *ret;
  if ((ret = listNew()) != NULL) {
    while ((read = getline(&line, &len, fp)) != -1) {
      line[strcspn(line, "\n")] = 0;
      char *cur;
      char *end;
      int id = -1;
      int priority = -1;
      long dateCreated = -1;
      long deadline = -1;
      long dateConcluded = -1;
      char *desc;
      char *worker;
      cur = strtok(line, "|");
      if (cur != NULL) {
        id = (int) strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        priority = (int) strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        dateCreated = strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        deadline = strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        dateConcluded = strtol(cur, &end, 10);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        if (strlen(cur) >= 1024) {
          exit(1);
        }
        desc = (char *) malloc(strlen(cur) + 1);
        strcpy(desc, cur);
      } else {
        exit(1);
      }
      cur = strtok(NULL, "|");
      if (cur != NULL) {
        if (strlen(cur) >= 1024) {
          exit(1);
        }
        worker = (char *) malloc(strlen(cur) + 1);
        strcpy(worker, cur);
      } else {
        exit(1);
      }
      card *c = cardNew(id, priority, dateCreated, desc);
      cardAssign(c, worker);
      cardSetDeadline(c, deadline);
      cardSetDateConcluded(c, dateConcluded);
      listAddByPriority(c, ret);
      listAddByDate(c, all);

    }
    fclose(fp);
    return ret;
  }
  fclose(fp);
  return NULL;
}

void saveTasks(tasklist *todo, tasklist *doing, tasklist *done) {

  // Save todo
  FILE *td;
  node *n1 = todo->first;
  td = fopen("todo.txt", "w");
  if (n1 != NULL) {
    while (n1->next != NULL) {
      fprintf(td, "%d|%d|%ld|%s\n", n1->task->id, n1->task->priority, n1->task->dateCreated, n1->task->description);
      n1 = n1->next;
    }
    fprintf(td, "%d|%d|%ld|%s", n1->task->id, n1->task->priority, n1->task->dateCreated, n1->task->description);
  }
  fclose(td);

  // Save doing
  FILE *dg;
  node *n2 = doing->first;
  dg = fopen("doing.txt", "w");
  if (n2 != NULL) {
    while (n2->next != NULL) {
      fprintf(dg,
              "%d|%d|%ld|%ld|%s|%s\n",
              n2->task->id,
              n2->task->priority,
              n2->task->dateCreated,
              n2->task->deadline,
              n2->task->description,
              n2->task->person);
      n2 = n2->next;
    }
    fprintf(dg,
            "%d|%d|%ld|%ld|%s|%s",
            n2->task->id,
            n2->task->priority,
            n2->task->dateCreated,
            n2->task->deadline,
            n2->task->description,
            n2->task->person);
  }
  fclose(dg);

  // Save done
  FILE *dn;
  node *n3 = done->first;
  dn = fopen("done.txt", "w");
  if (n3 != NULL) {
    while (n3->next != NULL) {
      fprintf(dn,
              "%d|%d|%ld|%ld|%ld|%s|%s\n",
              n3->task->id,
              n3->task->priority,
              n3->task->dateCreated,
              n3->task->deadline,
              n3->task->dateConcluded,
              n3->task->description,
              n3->task->person);
      n3 = n3->next;
    }
    fprintf(dn,
            "%d|%d|%ld|%ld|%ld|%s|%s",
            n3->task->id,
            n3->task->priority,
            n3->task->dateCreated,
            n3->task->deadline,
            n3->task->dateConcluded,
            n3->task->description,
            n3->task->person);
  }
  fclose(dn);
}

long dateToLong(int year, int month, int day) {
  struct tm t;
  time_t t_of_day;

  t.tm_year = year - 1900;    // Year - 1900
  t.tm_mon = month;           // Month, where 0 = jan
  t.tm_mday = day;            // Day of the month
  t.tm_hour = 0;
  t.tm_min = 0;
  t.tm_sec = 1;
  t.tm_isdst = -1;            // Is DST on? 1 = yes, 0 = no, -1 = unknown

  t_of_day = mktime(&t);

  return (long) t_of_day;
}

char *printDate(long time) {
  char *date = (char *) malloc(16 * sizeof(char));
  time_t rawtime = time;

  struct tm *timeinfo = localtime(&rawtime);
  strftime(date, 16, "%d/%m/%Y", timeinfo);

  return date;
}

char *listPrint(tasklist *l, int i, int option) {
  if (listIsEmpty(l)) {
    return NULL;
  }

  node *n = l->first;
  char *taskInfo = (char *) malloc(120 * sizeof(char));
  int cur = 0;

  while (n->next != NULL) {
    if (cur == i)
      break;
    n = n->next;
    ++cur;
  }

  switch (option) {
  case 1:
    sprintf(taskInfo,
            "[ID: %d | Priority: %d]\n%s",
            n->task->id,
            n->task->priority,
            n->task->description);
    break;

  case 2:
    sprintf(taskInfo,
            "[ID: %d | Priority: %d]\n[Assigned to: %s | Deadline: %s]\n%s",
            n->task->id,
            n->task->priority,
            n->task->person,
            printDate(n->task->deadline),
            n->task->description);
    break;

  case 3:
    sprintf(taskInfo,
            "[ID: %d | Priority: %d | Completed by: %s]\n%s",
            n->task->id,
            n->task->priority,
            n->task->person,
            n->task->description);
    break;

  case 4:
    sprintf(taskInfo,
            "[ID: %d | Date: %s]\n%s",
            n->task->id,
            printDate(n->task->dateCreated),
            n->task->description);
    break;

  default:break;
  }

  return taskInfo;
}

