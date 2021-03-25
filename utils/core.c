#include "core.h"

#define MAX_TASKS 10

board_t *board;
void initCore(board_t *board_init) {
  board = board_init;
}

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
/*
void printBoard() {
  // Function for keeping the board on screen
  listPrintToDo(board->todo);
  listPrintDoing(board->doing);
  listPrintDone(board->done);
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
*/

// --- Functions for core operations --- //

int getCurID() {
  int lastID = listSize(board->todo) + listSize(board->doing) + listSize(board->done);
  if (lastID == INT_MAX - 1) {
    puts("Error: Maximum number of tasks reached");
    exit(1);
  }
  return lastID + 1;
}

void addTask(int priority, char *description) {
  card *toAdd;
  int id = getCurID();

  if ((toAdd = cardNew(id, priority, time(NULL), description)) != NULL) {
    listAddByPriority(toAdd, board->todo);
    listAddByDate(toAdd, board->all);
  }
}

void workOnTask() {

  // Check if MAX_TASKS has been reached
  if (board->doing->size >= MAX_TASKS) {
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
  if (!listTaskExists(id, board->todo)) {
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
  if ((toMove = listRemoveTaskByID(id, board->todo)) != NULL) {

    // Set deadline
    cardSetDeadline(toMove, dateToLong(year, month - 1, day));

    // Ask user who to assign the task to
    printf("Assign task to > ");
    char *worker = (char *) malloc(1024);
    readString(worker);
    cardAssign(toMove, worker);

    // Add task to doing
    listAddByPriority(toMove, board->doing);
  }

}

void reassignTask() {
  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in todo
  if (!listTaskExists(id, board->doing)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    getchar();
    return;
  }

  card *toChange;
  if ((toChange = listGetTaskByID(id, board->doing)) != NULL) {

    // Ask user who to assign the task to
    printf("Assign task to > ");
    char *worker = (char *) malloc(1024);
    readString(worker);
    cardAssign(toChange, worker);
  }
}

void closeTask() {

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in todo
  if (!listTaskExists(id, board->doing)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    getchar();
    return;
  }

  card *toMove;

  // Remove task from doing
  if ((toMove = listRemoveTaskByID(id, board->doing)) != NULL) {

    // Set time of conclusion
    cardSetDateConcluded(toMove, time(NULL));

    // Add task to doing
    listAddByConclusion(toMove, board->done);
  }

}

void reopenTask() {

  // Ask user for task id
  printf("Task id > ");
  int id;
  readInt(&id);

  // Chech if task exists in done
  if (!listGetTaskByID(id, board->done)) {
    puts("\n--------------------------");
    puts("No task found for given id");
    puts("--------------------------\n");
    getchar();
    return;
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
  }

}

void tasksFromWorker() {

  // Ask user for name of worker
  printf("Tasks by name > ");
  char *name = (char *) malloc(1024);
  readString(name);

  if (!listIsEmpty(board->doing)) {
    // Print tasks worker is currently doing
    node *n1 = board->doing->first;
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

void tasksByDate() {
  if (listIsEmpty(board->all)) { return; }
  node *n = board->all->first;
  puts("All Tasks: ");
  printf("\tID: %d\n\tPriority %d: %s", n->task->id, n->task->priority, n->task->description);
  while (n->next != NULL) {
    n = n->next;
    printf("\n\tID: %d\n\tPriority %d: %s", n->task->id, n->task->priority, n->task->description);
  }
  printf("\n");
  getchar();
}