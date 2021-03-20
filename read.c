#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "read.h"

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