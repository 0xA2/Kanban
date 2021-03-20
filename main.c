#include <stdio.h>
#include <stdlib.h>

#include "tasklist.h"
#include "render.h"
#include "core.h"
#include "read.h"

int main() {

  //tasklist* all;
  tasklist *todo = loadToDo();
  tasklist *doing = loadDoing();
  tasklist *done = loadDone();

  render();
  /*
  char title[] = "Kanban";
  char *options[] = {"Add Task", "Work on Task", "Assign task", "Close task", "Reopen task", "View tasks by worker",
                     "View tasks by date", "Exit"};

  while (1) {
    printBoard(todo, doing, done);
    printMenu(title, options, 8, 8);
    printf("> ");
    int op = 0;
    readInt(&op);
    switch (op) {
    case 1: addTask(todo, doing, done);
      break;

    case 2: workOnTask(todo, doing);
      break;

    case 3: reAssignTask(doing);
      break;

    case 4: closeTask(doing, done);
      break;

    case 5: reopenTask(todo, done);
      break;

    case 6:
      //tasksFromWorker(doing,done);
      break;

    case 7:
      //tasksByDate(all);
      break;

    case 8: exit(0);
      break;

    default: puts("Invalid Option");
    }

  }
   */
  return 0;
}