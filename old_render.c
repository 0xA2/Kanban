#include <stdio.h>
#include <string.h>

#include "old_render.h"
#include "tasklist.h"

#define MAX2(x, y) (((x) >= (y)) ? (x) : (y))

void formatLine(char *str[], unsigned int sizeLimit) {
  char strTooLong[3][sizeLimit + 2];
  char fmtStr[(3 * sizeLimit) + 7];
  strcpy(fmtStr, "");

  for (int i = 0; i < 3; ++i) {
    strcat(fmtStr, "* ");
    // Append a string of spaces if string is null
    if (str[i] == NULL) {
      char nullStr[sizeLimit];
      strcpy(nullStr, " ");
      for (int j = 0; j < sizeLimit - 1; ++j) {
        strcat(nullStr, " ");
      }
      strcat(fmtStr, nullStr);
    } else {

      unsigned int curSize = strlen(str[i]);
      // if string is bigger than the limit then cut it, next line will have the rest
      if (curSize > sizeLimit - 2) {
        strncat(fmtStr, str[i], sizeLimit - 1);
        strcat(fmtStr, " ");
        strncpy(strTooLong[i], &str[i][sizeLimit - 1], curSize - sizeLimit);
      } else {
        // Append the string itself and delimiter
        strcat(fmtStr, str[i]);

        // If too small, ident until delimiter
        if (curSize < sizeLimit - 2) {
          for (int j = 0; j < sizeLimit - curSize; ++j) {
            strcat(fmtStr, " ");
          }
        }
      }
    }
  }

  puts(fmtStr);
}

void printLimit(char d, unsigned int sizeLimit) {
  for (int i = 0; i < (sizeLimit * 3) + 7; ++i) {
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