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

