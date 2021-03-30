#include "form_tools.h"

//////////////////////////////////////////////////////
const int MAX_VALID_YR = 2038;
const int MIN_VALID_YR = 1970;
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////

/* UTILS */

int isLeap(int year) {
  return (((year % 4 == 0) &&
      (year % 100 != 0)) ||
      (year % 400 == 0));
}

int isValidDate(int d, int m, int y) {

  if (y > MAX_VALID_YR || y < MIN_VALID_YR)
    return 0;

  if (m < 1 || m > 12)
    return 0;

  if (d < 1 || d > 31)
    return 0;

  if (m == 2) {
    if (isLeap(y))
      return (d <= 29);
    else
      return (d <= 28);
  }

  if (m == 4 || m == 6 || m == 9 || m == 11)
    return (d <= 30);

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
  while (isspace((unsigned char) *str)) str++;

  // All spaces?
  if (*str == 0)
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char) *end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

FIELD *newFieldPrompt(int pos, char *prompt) {
  FIELD *newField;
  newField = new_field(1, 25, pos * 2, 0, 0, 0);
  set_field_buffer(newField, 0, prompt);
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

  return newField;
}

FIELD *newFieldButton(int pos, char *prompt) {
  FIELD *newField;
  newField = new_field(1, 7, pos * 2, 0, 0, 0);
  set_field_buffer(newField, 0, prompt);
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_ACTIVE);

  return newField;
}

FIELD *newFieldInputInt(int pos, int min, int max) {
  FIELD *newField;
  newField = new_field(1, 40, pos * 2, 25, 0, 0);
  set_field_type(newField, TYPE_NUMERIC, min, max);
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(newField, A_UNDERLINE);

  return newField;
}

FIELD *newFieldInputStr(int pos) {
  FIELD *newField;
  newField = new_field(1, 40, pos * 2, 25, 0, 0);
  set_field_type(newField, TYPE_ALPHA, 1);
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(newField, A_UNDERLINE);

  return newField;
}

FIELD *newFieldInputDate(int pos) {
  FIELD *newField;
  newField = new_field(1, 40, pos * 2, 25, 0, 0);
  set_field_type(newField, TYPE_REGEXP, "[0-9][0-9]\\/[0-9][0-9]\\/[0-9][0-9][0-9][0-9]");
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(newField, A_UNDERLINE);

  return newField;
}

/* BOARD STUFF */

void title(WINDOW **win, char *title) {
  int startx = (getmaxx(*win) - (int) strlen(title)) / 2;
  wmove(*win, 0, startx);
  wattron(*win, A_REVERSE);
  waddstr(*win, title);
  wattroff(*win, A_REVERSE);
  wrefresh(*win);
}

void printList(WINDOW *win, tasklist *list, int option) {
  char *string = (char *) malloc(80 * sizeof(char));
  wmove(win, 0, 0);

  whline(win, ACS_HLINE, getmaxx(win));
  for (int i = 0; i < listSize(list); ++i) {
    string = listPrint(list, i, option);
    wmove(win, getcury(win) + 1, 0);
    waddstr(win, string);
    wmove(win, getcury(win) + 1, 0);
    whline(win, ACS_HLINE, getmaxx(win));
  }

  free(string);
}

//////////////////////////////////////////////////////

/* FORM STUFF */

void driver(FORM ***form, WINDOW **fieldsWin) {
  FIELD **fields = form_fields(**form);

  int ch;
  while (1) {
    if (current_field(**form) == fields[field_count(**form) - 1]) {
      set_field_back(fields[field_count(**form) - 1], A_REVERSE);
    } else {
      set_field_back(fields[field_count(**form) - 1], A_BOLD);
    }

    ch = getch();

    switch (ch) {
      case KEY_LEFT:form_driver(**form, REQ_PREV_CHAR);
        break;

      case KEY_RIGHT:form_driver(**form, REQ_NEXT_CHAR);
        break;

      case KEY_DOWN:form_driver(**form, REQ_NEXT_FIELD);
        form_driver(**form, REQ_END_LINE);
        break;

      case KEY_UP:form_driver(**form, REQ_PREV_FIELD);
        form_driver(**form, REQ_END_LINE);
        break;

      case KEY_BACKSPACE:form_driver(**form, REQ_DEL_PREV);
        break;

      case 10:
        if (current_field(**form) == fields[field_count(**form) - 1]) {
          form_driver(**form, REQ_VALIDATION);
          return;
        } else {
          form_driver(**form, REQ_NEXT_FIELD);
        }
        break;

      default:form_driver(**form, ch);
        break;
    }

    wrefresh(*fieldsWin);
  }
}

void renderForm(FORM **form, FIELD ***fields) {
  *form = new_form(*fields);
  assert(form != NULL);

  WINDOW *fieldsWin = (WINDOW *) malloc(sizeof(WINDOW *));
  assert(fieldsWin != NULL);

  WINDOW *formWin = derwin(stdscr, getmaxy(stdscr) - 4, getmaxx(stdscr) - 4, getmaxy(stdscr) - 2, getmaxx(stdscr) - 2);
  fieldsWin = derwin(formWin, getmaxy(formWin) - 2, getmaxx(formWin) - 2, 1, 1);
  box(formWin, 0, 0);

  set_form_win(*form, formWin);
  set_form_sub(*form, derwin(fieldsWin, getmaxy(fieldsWin), getmaxx(fieldsWin), 0, 0));
  post_form(*form);
  pos_form_cursor(*form);

  refresh();
  wrefresh(formWin);
  wrefresh(fieldsWin);

  driver(&form, &fieldsWin);

  free(fieldsWin);
  free(formWin);
}