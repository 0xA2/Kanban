#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils/render.h"
#define MAX_TASKS 10

// --- Read Functions --- //
int readInt(int *op) {

  // Function to read user input and use it as an integer
  // Non digit characters will be ignored

  long toInt;
  char buffer[1024];

  if (fgets(buffer, 1024, stdin) != NULL) {
    size_t len = strlen(buffer);

    // Clear stdin if more than 1024 characters are read
    if (len > 0 && buffer[len - 1] != '\n') {
      int clear;
      while ((clear = getchar()) != '\n' && clear != EOF);
    }
  } else {
    printf("\n");
    exit(1);
  }

  // Convert string input into long and ignore not digit characters
  char *end;
  errno = 0;
  toInt = strtol(buffer, &end, 10);


  // Check errors

  if (errno == ERANGE) { return 0; }

  if (end == buffer) { return 0; }

  if (*end && *end != '\n') { return 0; }

  if (toInt > INT_MAX || toInt < INT_MIN) { return 0; }

  *op = (int) toInt;
  return 1;
}

void readString(char *buffer) {

  // Function to read user input and use it as a string

  if (fgets(buffer, 1024, stdin) != NULL) {
    size_t len = strlen(buffer);

    // Clear stdin if more than 1024 characters are read
    if (len > 0 && buffer[len - 1] != '\n') {
      int clear;
      while ((clear = getchar()) != '\n' && clear != EOF);
    }
    buffer[strcspn(buffer, "\n")] = 0;
    buffer[strcspn(buffer, "|")] = 0;
    return;
  }

  // Exit if something goes wrong while reading input
  exit(1);
}



// --- Print Functions --- //

void printBoard(tasklist *todo, tasklist *doing, tasklist *done) {

  // Function for keeping the board on screen

  listPrintToDo(todo);
  listPrintDoing(doing);
  listPrintDone(done);
}

void printMenu(char *title, char *options[], int count, int ident) {

  // Function for printing main menu

  for (int i = 0; i < strlen(title) + ident; i++) { printf("-"); }
  printf("\n");
  for (int i = 0; i < ident / 2; i++) { printf(" "); }
  printf("%s\n", title);
  for (int i = 0; i < strlen(title) + ident; i++) { printf("-"); }
  printf("\n");
  for (int i = 0; i < count; i++) {
    printf("%d) ", i + 1);
    puts(options[i]);
  }
}



// --- Functions for core operations --- //


int getCurID(tasklist *todo, tasklist *doing, tasklist *done) {
  int lastID = listSize(todo) + listSize(doing) + listSize(done);
  if (lastID == INT_MAX - 1) {
    puts("Error: Maximum number of tasks reached");
    exit(1);
  }
  return lastID + 1;
}

void addTask(tasklist *todo, tasklist *doing, tasklist *done, tasklist *all) {

  // Ask user for task priority
  printf("Task Priority[1-10] > ");
  int pri;
  readInt(&pri);

  // Ask user for task description
  printf("Task Description > ");
  char *desc = (char *) malloc(1024);
  readString(desc);

  card *toAdd;
  int id = getCurID(todo, doing, done);
  if ((toAdd = cardNew(id, pri, time(NULL), desc)) != NULL) {
    listAddByPriority(toAdd, todo);
    listAddByDate(toAdd, all);
  }
  return;
}

void workOnTask(tasklist *todo, tasklist *doing) {

  // Check if MAX_TASKS has been reached
  if (doing->size >= MAX_TASKS) {
    puts("\n----------------------------");
    puts("You can't work on more tasks");
    puts("----------------------------\n");
    getchar();
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
    getchar();
    return;
  }


  // Ask user for date
  puts("\nSet deadline:");
  printf("Year > ");
  int year;
  readInt(&year);
  if (year < 1970 || year >= 2038) {
    puts("Invalid date");
    exit(1);
  }
  printf("Month > ");
  int month;
  readInt(&month);
  if (month < 1 || month > 12) {
    puts("Invalid date");
    exit(1);
  }
  printf("Day > ");
  int day;
  readInt(&day);
  switch (month) {
    case 1:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 2:
      if (day < 1 || day > 29) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 3:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 4:
      if (day < 1 || day > 30) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 5:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 6:
      if (day < 1 || day > 30) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 7:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 8:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 9:
      if (day < 1 || day > 30) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 10:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 11:
      if (day < 1 || day > 30) {
        puts("Invalid date");
        exit(1);
      }
      break;
    case 12:
      if (day < 1 || day > 31) {
        puts("Invalid date");
        exit(1);
      }
      break;
  }

  card *toMove;

  // Remove task from todo
  if ((toMove = listRemoveTaskByID(id, todo)) != NULL) {

    // Set deadline
    cardSetDeadline(toMove, dateToLong(year, month - 1, day));

    // Ask user who to assign the task to
    printf("Assign task to > ");
    char *worker = (char *) malloc(1024);
    readString(worker);
    cardAssign(toMove, worker);

    // Add task to doing
    listAddByPriority(toMove, doing);
  }

}

void reassignTask(tasklist *doing) {

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in todo
  if (!listTaskExists(id, doing)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    getchar();
    return;
  }

  card *toChange;
  if ((toChange = listGetTaskByID(id, doing)) != NULL) {

    // Ask user who to assign the task to
    printf("Assign task to > ");
    char *worker = (char *) malloc(1024);
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
    getchar();
    return;
  }

  card *toMove;

  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, doing)) != NULL) {

    // Set time of conclusion
    cardSetDateConcluded(toMove, time(NULL));

    // Add task to doing
    listAddByConclusion(toMove, done);
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
    getchar();
    return;
  }

  card *toMove;

  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, done)) != NULL) {

    // Reset task
    cardSetDeadline(toMove, 0);
    cardSetDateConcluded(toMove, 0);
    toMove->person = NULL;

    // Add task to done
    listAddByPriority(toMove, todo);
  }

}

void tasksFromWorker(tasklist *doing, tasklist *done) {

  // Ask user for name of worker
  printf("Tasks by name > ");
  char *name = (char *) malloc(1024);
  readString(name);

  if (!listIsEmpty(doing)) {
    // Print tasks worker is currently doing
    node *n1 = doing->first;
    int flag1 = 1;
    if (n1 != NULL) {
      puts("\nWorking on:");
      while (n1->next != NULL) {
        if (!strcmp(name, n1->task->person)) {
          flag1 = 0;
          printf("\n\tID: %d | Priority: %d\n\t%s\n\tAssigned to %s",
                 n1->task->id,
                 n1->task->priority,
                 n1->task->description,
                 n1->task->person);
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

  if (!listIsEmpty(done)) {
    // Print tasks worker completed
    node *n2 = done->first;
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

void tasksByDate(tasklist *all) {
  if (listIsEmpty(all)) { return; }
  node *n = all->first;
  puts("All Tasks: ");
  printf("\tID: %d\n\tPriority %d: %s", n->task->id, n->task->priority, n->task->description);
  while (n->next != NULL) {
    n = n->next;
    printf("\n\tID: %d\n\tPriority %d: %s", n->task->id, n->task->priority, n->task->description);
  }
  printf("\n");
  getchar();
}

int main() {

  tasklist *all = listNew();
  tasklist *todo = loadToDo(all);
  tasklist *doing = loadDoing(all);
  tasklist *done = loadDone(all);

  char title[] = "Kanban";
  char *options[] = {"Add Task", "Work on Task", "Reassign task", "Close task", "Reopen task", "View tasks by worker",
                     "View tasks by date", "Exit"};

  while (1) {
    system("clear");
    printBoard(todo, doing, done);
    printMenu(title, options, 8, 8);
    printf("> ");
    int op = 0;
    readInt(&op);
    switch (op) {
      case 1:addTask(todo, doing, done, all);
        break;

      case 2:workOnTask(todo, doing);
        break;

      case 3:reassignTask(doing);
        break;

      case 4:closeTask(doing, done);
        break;

      case 5:reopenTask(todo, done);
        break;

      case 6:tasksFromWorker(doing, done);
        break;

      case 7:tasksByDate(all);
        break;

      case 8:saveTasks(todo, doing, done);
        exit(0);
        break;

      default:puts("Invalid Option");
    }

  }
  return 0;
}

