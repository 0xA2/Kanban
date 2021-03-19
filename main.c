#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "card.h"
#include "tasklist.h"

#define MAX_TASKS 10

#define MAX_READ 1024 // Max number of characters that can be read into a single buffer

#define MAX2(x, y) (((x) >= (y)) ? (x) : (y))
#define MIN2(x, y) (((x) <= (y)) ? (x) : (y))

// --- Read Functions --- //

int readInt(int *op) {

  // Function to read user input and use it as an integer
  // Non digit characters will be ignored

  long toInt;
  char buffer[MAX_READ];

  if (fgets(buffer, MAX_READ, stdin) != NULL) {
    size_t len = strlen(buffer);

    // Clear stdin if more than MAX_READ characters are read
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

  if (errno == ERANGE) {
    return 0;
  }

  if (end == buffer) {
    return 0;
  }

  if (*end && *end != '\n') {
    return 0;
  }

  if (toInt > INT_MAX || toInt < INT_MIN) {
    return 0;
  }

  *op = (int) toInt;
  return 1;
}

void readString(char *buffer) {

  // Function to read user input and use it as a string

  if (fgets(buffer, MAX_READ, stdin) != NULL) {
    size_t len = strlen(buffer);

    // Clear stdin if more than MAX_READ characters are read
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
void formatLine(char *str[], unsigned int sizeLimit) {
  char *strTooLong[3];
  char fmtStr[(3 * sizeLimit) + 10];
  strcpy(fmtStr, "* ");

  for (int i = 0; i < 3; ++i) {
    // Append a string of spaces if string is null
    if (str[i] == NULL) {
      char nullStr[sizeLimit];
      strcpy(nullStr, " ");
      for (int j = 0; j < sizeLimit - 1; ++j) {
        strcat(nullStr, " ");
      }
      strcat(fmtStr, nullStr);
    } else {
      unsigned int curSize = strlen(str[i]) / sizeof(char *);
      // if string is bigger than the limit then cut it, next line will have the rest
      if (curSize > sizeLimit - 2) {
        strncat(fmtStr, str[i], sizeLimit - 3);
        strncpy(strTooLong[i], &str[i][sizeLimit], curSize - sizeLimit - 2);
      } else {

        // Append the string itself and delimiter
        strcat(fmtStr, str[i]);

        // If too small, ident until delimiter
        if (curSize < sizeLimit - 2) {
          for (int j = 0; j < sizeLimit - curSize - 4; ++j) {
            strcat(fmtStr, " ");
          }
          strcat(fmtStr, "* ");
        }
      }
    }
  }

  puts(fmtStr);
}

void printLimit(char d, unsigned int sizeLimit) {
  for (int i = 0; i < (sizeLimit * 3) + 10; ++i) {
    printf("%c", d);
  }
  printf("\n");
}

void printLine(char *str[], unsigned int sizeLimit) {
  formatLine(str, sizeLimit);
}

void printBoard(tasklist *todo, tasklist *doing, tasklist *done) {
  unsigned int sizeLimit = 20 - 2;
  // Strings to print at each line
  char *firstStr[3];

  // First line has title of each column
  firstStr[0] = "TODO";
  firstStr[1] = "DOING";
  firstStr[2] = "DONE";

  // Print titles
  printLimit('*', sizeLimit);
  printLine(firstStr, sizeLimit);
  printLimit('*', sizeLimit);

  // Get maximum size of given columns
  int maxSize = MAX2(MAX2(todo->size, doing->size), done->size);
  for (int i = 1; i <= maxSize; ++i) {
    char *curStr[3];
    curStr[0] = listPrintToDo(todo, i);
    curStr[1] = listPrintDoing(doing, i);
    curStr[2] = listPrintDone(done, i);
    printLine(curStr, sizeLimit);
    printLimit('*', sizeLimit);
  }
}

void printMenu(char *title, char *options[], int count, int ident) {

  // Function for printing main menu

  for (int i = 0; i < strlen(title) + ident; i++) {
    printf("-");
  }
  printf("\n");
  for (int i = 0; i < ident / 2; i++) {
    printf(" ");
  }
  printf("%s\n", title);
  for (int i = 0; i < strlen(title) + ident; i++) {
    printf("-");
  }
  printf("\n");
  for (int i = 0; i < count; i++) {
    printf("%d) ", i + 1);
    puts(options[i]);
  }
}


// --- Functions for core operations --- //


int getCurID(tasklist *todo, tasklist *doing, tasklist *done) {
  return listSize(todo) + listSize(doing) + listSize(done) + 1;
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

int main() {

  //tasklist* all;
  tasklist *todo = loadToDo();
  tasklist *doing = loadDoing();
  tasklist *done = loadDone();

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
  return 0;
}