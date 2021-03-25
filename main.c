#include "utils/core.h"

int main() {
  tasklist *all = listNew();
  tasklist *todo = loadToDo(all);
  tasklist *doing = loadDoing(all);
  tasklist *done = loadDone(all);
  board_t board = {
      all,
      todo,
      doing,
      done
  };
  initCore(&board);

  char title[] = "Kanban";
  char *options[] = {"Add Task", "Work on Task", "Reassign task", "Close task", "Reopen task", "View tasks by worker",
                     "View tasks by date", "Exit"};

  while (1) {
    system("clear");
    printBoard(&board);
    printMenu(title, options, 8, 8);
    printf("> ");

    int op = 0;
    readInt(&op);

    switch (op) {
      case 1:addTask();
        break;

      case 2:workOnTask();
        break;

      case 3:reassignTask();
        break;

      case 4:closeTask();
        break;

      case 5:reopenTask();
        break;

      case 6:tasksFromWorker();
        break;

      case 7:tasksByDate();
        break;

      case 8:
        //saveTasks();
        exit(0);

      default:puts("Invalid Option");
    }
  }

  return 0;
}

