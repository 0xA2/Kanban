#include "core.h"

#define MAX_TASKS 10

board_t *board;

void initCore(board_t *board_init) {
  board = board_init;
}

/* CORE FUNCTIONALITY */

int getCurID() {
  int lastID = listSize(board->todo) + listSize(board->doing) + listSize(board->done);
  if (lastID == INT_MAX - 1) {
    puts("Error: Maximum number of tasks reached");
    exit(1);
  }
  return lastID + 1;
}

void addTask(int priority, char *description) {
  if (priority > 10)
    return;

  card *toAdd;
  int id = getCurID();

  if ((toAdd = cardNew(id, priority, time(NULL), description)) != NULL) {
    listAddByPriority(toAdd, board->todo);
    listAddByDate(toAdd, board->all);
  }
}

int workOnTask(int id, int d, int m, int y, char *worker) {

  // Check if MAX_TASKS has been reached
  if (board->doing->size >= MAX_TASKS) {
    return -1;
  }

  // Chech if task exists in todo
  if (!listTaskExists(id, board->todo)) {
    return 0;
  }

  card *toMove;

  // Remove task from todo
  if ((toMove = listRemoveTaskByID(id, board->todo)) != NULL) {
    long rawtime = dateToLong(y, m, d);

    // Set deadline
    cardSetDeadline(toMove, rawtime);

    // Set person working
    cardAssign(toMove, worker);

    // Add task to doing
    listAddByName(toMove, board->doing);
    return 1;
  }

  return 0;
}

int closeTask(int id) {
  card *toMove;

  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, board->doing)) != NULL) {
    // Set time of conclusion
    cardSetDateConcluded(toMove, time(NULL));
    // Add task to doing
    listAddByConclusion(toMove, board->done);
    return 1;
  }
  return 0;
}

int reassignTask(int id, char *worker) {
  // Chech if task exists in todo
  if (!listTaskExists(id, board->doing)) {
    return 0;
  }

  card *toChange;
  if ((toChange = listRemoveTaskByID(id, board->doing)) != NULL) {
    cardAssign(toChange, worker);
    listAddByName(toChange, board->doing);
    return 1;
  }
  return 0;
}

int reopenTask(int id) {
  // Chech if task exists in done
  if (!listGetTaskByID(id, board->done)) {
    return 0;
  }

  card *toMove;
  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, board->done)) != NULL) {
    // Reset task
    cardSetDeadline(toMove, 0);
    cardSetDateConcluded(toMove, 0);
    toMove->person = NULL;
    // Add task to done
    listAddByPriority(toMove, board->todo);
    return 1;
  }
  return 0;
}

/*
tasklist *tasksFromWorker(char* name) {
  tasklist *
  if (!listIsEmpty(board->doing)) {
    // Print tasks worker is currently doing
    node *n1 = board->doing->first;
    int flag1 = 1;
    if (n1 != NULL) {
      while (n1->next != NULL) {
        if (!strcmp(name, n1->task->person)) {
          flag1 = 0;

          n1 = n1->next;
        }
      }
      if (!strcmp(name, n1->task->person)) {
        flag1 = 0;
        printf("\n\tID: %d | Priority: %d\n\t> %s\n\tAssigned to %s",
               n1->task->id,
               n1->task->priority,
               n1->task->description,
               n1->task->person);
      }
      if (flag1) { printf("\t%s is not currently working any tasks\n", name); }
    }
  } else {
    puts("\nWorking on:");
    printf("\t%s is not currently working any tasks\n", name);
  }

  if (!listIsEmpty(board->done)) {
    // Print tasks worker completed
    node *n2 = board->done->first;
    int flag2 = 1;
    if (n2 != NULL) {
      puts("\nTasks completed:");
      while (n2->next != NULL) {
        if (!strcmp(name, n2->task->person)) {
          flag2 = 0;
          printf("\n\tID: %d | Priority: %d\n\t> %s\n\tAssigned to %s",
                 n2->task->id,
                 n2->task->priority,
                 n2->task->description,
                 n2->task->person);
          n2 = n2->next;
        }
      }
      if (!strcmp(name, n2->task->person)) {
        flag2 = 0;
        printf("\n\tID: %d | Priority: %d\n\t%s\n\tAssigned to %s",
               n2->task->id,
               n2->task->priority,
               n2->task->description,
               n2->task->person);
      }
      if (flag2) { printf("\t%s has not completed any tasks\n", name); }
    }
  } else {
    puts("\nTasks completed:");
    printf("\t%s has not completed any tasks\n", name);
  }
  getchar();
}
*/