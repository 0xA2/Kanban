#include "core.h"

#define MAX_TASKS 10

board_t *board;
void initCore(board_t *board_init) {
  board = board_init;
}

// --- Read Functions --- //
int readInt(int *op, char *buffer) {

  // Function to read user input and use it as an integer
  // Non digit characters will be ignored

  long toInt;

  // Convert string input into long and ignore not digit characters
  char *end;
  errno = 0;
  toInt = strtol(buffer, &end, 10);

  // Check errors

  if (errno == ERANGE
      || end == buffer
      || (*end && *end != '\n')
      || (toInt > INT_MAX || toInt < INT_MIN)) {
    return 0;
  } else {
    *op = (int) toInt;
    return 1;
  }
}

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

/*
int workOnTask(int id, int day, int month, int year, char *person) {

  // Check if MAX_TASKS has been reached
  if (board->doing->size >= MAX_TASKS) {
    return -1;
  }

  // Chech if task exists in todo
  if (!listTaskExists(id, board->todo)) {
    return 0;
  }

  char buffer[100];
  struct tm date;
  memset(&date, 0, sizeof(date));
  date.tm_mday = day;
  date.tm_mon = month;
  date.tm_mon = year;

  if (sprintf(buffer, "%d/%d/%d", date.tm_mday, date.tm_mon, date.tm_year) == 3)
  {
    const char *format;

    format = "Dated %A %dth of %B, %Y";
    if (strftime(buffer, sizeof(buffer), format, &date) > sizeof(buffer))
      fprintf(stderr, "there was a problem converting the string\n");
    else
      fprintf(stdout, "%s\n", buffer);
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

void reassignTask(int id) {
  // Ask user for task id
  printf("Task id > ");

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

void closeTask(int id) {

  // Ask user for task id
  printf("Task id > ");

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

void reopenTask(int id) {

  // Ask user for task id
  printf("Task id > ");

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

*/