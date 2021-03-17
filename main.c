#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "card.h"
#include "list.h"

int readOption(int *op) {
  long toInt;
  char buffer[1024];

  // Exit if something goes wrong reading input
  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    printf("\n");
    exit(1);
  }

  // Convert string input into long
  char *ptr;
  errno = 0;
  toInt = strtol(buffer, &ptr, 10);


  // Check errors

  if (errno == ERANGE) {
    return 0;
  }

  if (ptr == buffer) {
    return 0;
  }

  if (*ptr && *ptr != '\n') {
    return 0;
  }

  if (toInt > INT_MAX || toInt < INT_MIN) {
    return 0;
  }

  *op = (int) toInt;
  return 1;
}

void printMenu(char *title, char *options[], int count, int ident) {
  // system("clear");
  for (int i = 0; i < strlen(title) + ident; i++) {
    printf("-");
  }
  printf("\n");
  for (int i = 0; i < count; i++) {
    printf("%d) ", i + 1);
    puts(options[i]);
  }
}

void printLine(char *string) {
  unsigned int sizeLimit = 20;
  unsigned int size = strlen(string);
  unsigned int cur = 0;

  printf("* ");
  // deve haver mais loops para o Doing e Done
  // ambos devem ser +- equivalentes a este
  for (cur = 1; cur <= size; ++cur) {
    printf("%c", string[cur - 1]);
    if (cur % sizeLimit == 0) {
      printf(" *\n");
      printf("* ");
    }
  }

  if (cur > size) {
    for (int i = 0; i <= sizeLimit - (cur % sizeLimit) + 1; ++i) {
      printf(" ");
    }
    printf("*");
  }

  printf("\n");
}

void printLimit(int sizeLimit, char d) {
  for (int i = 0; i < (sizeLimit * 3 + 10); ++i) {
    printf("%c", d);
  }
  printf("\n");
}

void printBoard(list *todo, list *doing, list *done) {
  printLimit(20, '*');
  printLine("TODO");
  printLimit(20, '*');

  for (int i = 0; i < 5; ++i) {
    // esta funçao vai receber 3 strings
    printLine("cenas bro, coisas acontecem");
    printLimit(20, '*');
  }

  printLimit(20, '*');
}

int main() {
  // lists used
  list *todo = listNew();
  list *doing = listNew();
  list *done = listNew();

  card *c = cardNew(1, 2, "lmao dude");
  cardAssign(c, "pedro");
  listAddFirst(*c, todo);

  // CLI stuff
  char title[] = "KanBan";
  char *options[] =
      {"Add Task", "Move to 'Doing'", "Change person", "Close task", "Reopen task", "View tasks by person",
       "View tasks by date", "Exit"};

  printBoard(todo, doing, done);
  printMenu(title, options, 8, 8);

  while (1) {
    // Manter o quadro sempre no do ecra
    //printBoard();
    printf("> ");
    int op = 0;
    readOption(&op);

    switch (op) {
    case 1:
      // add task
      listPrint(todo);
      break;

    case 2:
      // move to doing
      break;

    case 3:
      // change person
      break;

    case 4:
      // close task
      break;

    case 5:
      // reopen
      break;

    case 6:
      // view tasks by person
      break;

    case 7:
      // View tasks by date
      break;

    case 8: exit(0);
      break;

    default: puts("Invalid Option");
    }

  }
  return 0;
}

