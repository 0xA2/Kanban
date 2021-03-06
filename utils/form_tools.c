#include "form_tools.h"

//////////////////////////////////////////////////////
const int MAX_VALID_YR = 2038;
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////

/* UTILS */

int isLeap(int year) {
  return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
}

int isValidDate(int y, int m, int d) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);

  if (y > MAX_VALID_YR) {
    return 0;
  }

  if (y < (tm.tm_year + 1900)) {
    return 0;
  }

  if (y == (tm.tm_year + 1900) && m < (tm.tm_mon + 1)) {
    return 0;
  }

  if ((y == (tm.tm_year + 1900) && m == (tm.tm_mon + 1)) && d < tm.tm_mday) {
    return 0;
  }

  if (m < 1 || m > 12) {
    return 0;
  }

  if (d < 1 || d > 31) {
    return 0;
  }

  if (m == 2) {
    if (isLeap(y)) {
      return (d <= 29);
    } else
      return (d <= 28);
  }

  if (m == 4 || m == 6 || m == 9 || m == 11) {
    return (d <= 30);
  }

  return 1;
}

int readInt(int *op, char *buffer) {

  // Function to read user input and use it as an integer
  // Non digit characters will be ignored

  long toInt;

  // Convert string input into long and ignore not digit characters
  char *end;
  errno = 0;
  toInt = strtol(buffer, &end, 10);

  // Check errors
  if (errno == ERANGE || end == buffer || (*end && *end != '\n') || (toInt > INT_MAX || toInt < INT_MIN)) {
    return 0;
  } else {
    *op = (int) toInt;
    return 1;
  }
}

char *trimWhitespaces(char *str) {
  char *end;

  // Trim leading space
  while (isspace((unsigned char) *str))
    str++;

  // All spaces?
  if (*str == 0)
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char) *end))
    end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

/* BOARD STUFF */

void title(WINDOW *win, char *title) {
  int startx = (getmaxx(win) - (int) strlen(title)) / 2;
  wmove(win, 0, startx);
  wattron(win, A_REVERSE);
  waddstr(win, title);
  wattroff(win, A_REVERSE);
  wrefresh(win);
}

void printList(WINDOW *win, tasklist *list, int option) {
  char *string;
  wmove(win, 0, 0);

  whline(win, ACS_HLINE, getmaxx(win));
  for (int i = 0; i < listSize(list); ++i) {
    string = listPrint(list, i, option);
    wmove(win, getcury(win) + 1, 0);
    waddstr(win, string);
    wmove(win, getcury(win) + 1, 0);
    whline(win, ACS_HLINE, getmaxx(win));
  }
}

void printListByPerson(WINDOW *win, tasklist *list, char *name) {
  char *string;
  wmove(win, 0, 0);

  whline(win, ACS_HLINE, getmaxx(win));
  int counter = 0;
  if (!listIsEmpty(list)) {
    // Print tasks worker is currently doing
    node *n1 = list->first;
    int flag1 = 1;

    if (n1 != NULL) {

      // list has one elements
      if (strcmp(name, n1->task->person) == 0 && n1->next == NULL) {
        flag1 = 0;
        string = listPrint(list, counter, 1);

        wmove(win, getcury(win) + 1, 0);
        waddstr(win, string);
        wmove(win, getcury(win) + 1, 0);
        whline(win, ACS_HLINE, getmaxx(win));
        return;
      }

      // list has more than one element
      while (n1->next != NULL) {
        if (strcmp(name, n1->task->person) != 0) {
          flag1 = 0;
          n1 = n1->next;
          counter++;
        } else {
          string = listPrint(list, counter, 1);

          wmove(win, getcury(win) + 1, 0);
          waddstr(win, string);
          wmove(win, getcury(win) + 1, 0);
          whline(win, ACS_HLINE, getmaxx(win));
          n1 = n1->next;
          counter++;
        }

      }

      // Check for last element
      if (strcmp(name, n1->task->person) == 0) {
        string = listPrint(list, counter, 1);

        wmove(win, getcury(win) + 1, 0);
        waddstr(win, string);
        wmove(win, getcury(win) + 1, 0);
        whline(win, ACS_HLINE, getmaxx(win));
      }

      if (flag1) {
        string = "None found.";
        wmove(win, getcury(win) + 1, 0);
        waddstr(win, string);
        wmove(win, getcury(win) + 1, 0);
        whline(win, ACS_HLINE, getmaxx(win));
      }
    }

  } else {
    string = "None found.";
    wmove(win, getcury(win) + 1, 0);
    waddstr(win, string);
    wmove(win, getcury(win) + 1, 0);
    whline(win, ACS_HLINE, getmaxx(win));
  }
}

