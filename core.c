#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "card.h"
#include "read.h"
#include "core.h"

int getCurID (tasklist *todo, tasklist *doing, tasklist *done)
{
  return listSize (todo) + listSize (doing) + listSize (done) + 1;
  int lastID = listSize (todo) + listSize (doing) + listSize (done);
  if (lastID == INT_MAX - 1)
    {
      puts ("Error: Maximum number of tasks reached");
      exit (1);
    }
  return lastID + 1;
}

void addTask(tasklist *todo, tasklist *doing, tasklist *done) {

  // Ask user for task priority
  printf("Task Priority[1-10] > ");
  int pri;
  readInt(&pri);

  // Ask user for task description
  printf("Task Description > ");
  char *desc = (char *) malloc(MAX_READ);
  readString(desc);

  card *toAdd;
  int id = getCurID(todo, doing, done);
  if ((toAdd = cardNew(id, pri, time(NULL), desc)) != NULL) {
    listAddByPriority(toAdd, todo);
  }
}

void workOnTask(tasklist *todo, tasklist *doing) {

  // Check if MAX_TASKS has been reached
  if (doing->size >= MAX_TASKS) {
    puts("\n----------------------------");
    puts("You can't work on more tasks");
    puts("----------------------------\n");
    return;
  }

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in todo
  if (!listTaskExists(id, todo)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    return;
  }

  card *toMove;

  // Remove task from todo
  if ((toMove = listRemoveTaskByID(id, todo)) != NULL) {

    // Ask user who to assign the task to
    printf("Assign task to > ");
    char *worker = (char *) malloc(MAX_READ);
    readString(worker);
    cardAssign(toMove, worker);

    // Add task to doing
    listAddByPriority(toMove, doing);
  }

}

void reAssignTask(tasklist *doing) {

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in todo
  if (!listTaskExists(id, doing)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    return;
  }

  card *toChange;
  if ((toChange = listGetTaskByID(id, doing)) != NULL) {

    // Ask user who to assign the task to
    printf("Assign task to > ");
    char *worker = (char *) malloc(MAX_READ);
    readString(worker);
    cardAssign(toChange, worker);
  }
}

void closeTask(tasklist *doing, tasklist *done) {

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in todo
  if (!listTaskExists(id, doing)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    return;
  }

  card *toMove;

  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, doing)) != NULL) {

    // Set time of conclusion
    cardSetDateConcluded(toMove, time(NULL));

    // Add task to doing
    listAddByPriority(toMove, done);
  }

}

void reopenTask(tasklist *todo, tasklist *done) {

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in done
  if (!listGetTaskByID(id, done)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    return;
  }

  card *toMove;

  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, done)) != NULL) {
    // Reset task
    cardSetDateDue(toMove, 0);
    cardSetDateConcluded(toMove, 0);
    toMove->person = NULL;

    // Add task to done
    listAddByPriority(toMove, todo);
  }

}