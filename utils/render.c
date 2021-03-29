#include "render.h"

//////////////////////////////////////////////////////
board_t *boardList;

WINDOW *menuWin;
WINDOW *formWin;
WINDOW *fieldsWin;
WINDOW *todoBoard;
WINDOW *todoBoardIn;
WINDOW *doingBoard;
WINDOW *doingBoardIn;
WINDOW *doneBoard;
WINDOW *doneBoardIn;

char *choices[] = {
    "Add task",
    "Start task",
    "Close task",
    "Reassign task",
    "Reopen a task",
    "Exit",
};

int n_choices = sizeof(choices) / sizeof(char *);
//////////////////////////////////////////////////////

/* UTILS */

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

void title(WINDOW *win, char *title) {
  int startx = (getmaxx(win) - (int) strlen(title)) / 2;
  wmove(win, 0, startx);
  wattron(win, A_REVERSE);
  waddstr(win, title);
  wattroff(win, A_REVERSE);
  wrefresh(win);
}

FIELD *newFieldPrompt(int pos, char *prompt) {
  FIELD *newField;
  newField = new_field(1, 25, pos * 2, 0, 0, 0);
  set_field_buffer(newField, 0, prompt);
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

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
  set_field_type(newField, TYPE_REGEXP, "^\\d{1,2}\\/\\d{1,2}\\/\\d{4}$");
  set_field_opts(newField, O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(newField, A_UNDERLINE);

  return newField;
}

//////////////////////////////////////////////////////

/* PRINT STUFF */

void nuke() {
  // Free menuwin
  delwin(menuWin);
  // Free board
  delwin(todoBoard);
  delwin(todoBoardIn);
  delwin(doingBoard);
  delwin(doingBoardIn);
  delwin(doneBoard);
  delwin(doneBoardIn);
  // Free form
  delwin(formWin);
  delwin(fieldsWin);

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
    free(string);
  }
}

void renderMenu(int highlight) {
  int x, y, i, pad;

  // Upper left corner of menu
  int startx = 0;
  int starty = getmaxy(stdscr) - 3;

  // Menu dimensions
  int width = getmaxx(stdscr);
  int height = 3;

  // Selection Menu Dimensions
  menuWin = newwin(height, width, starty, startx);
  keypad(menuWin, TRUE);

  // Initial values, initial "padding"
  x = 2;
  y = 1;

  box(menuWin, 0, 0);

  // Get padding
  int effectiveSize = 0;

  for (int j = 0; j < n_choices; ++j) {
    effectiveSize += (int) strlen(choices[j]);
  }
  pad = (getmaxx(menuWin) - effectiveSize) / n_choices;

  // Print stuff
  for (i = 0; i < n_choices; ++i) {
    if (highlight == i + 1) {
      wattron(menuWin, A_REVERSE);
      mvwprintw(menuWin, y, x, "%s", choices[i]);
      wattroff(menuWin, A_REVERSE);
    } else
      mvwprintw(menuWin, y, x, "%s", choices[i]);
    x += pad + (int) strlen(choices[i]);
  }

  wrefresh(menuWin);
}

void initBoard() {
  int startx = 0;
  int height = 0;
  int width = 0;

  width = getmaxx(stdscr) / 3;
  height = getmaxy(stdscr) - 3;

  // Boxes inside to prevent text from overflowing
  int inStartx = 2;
  int inWidth = (getmaxx(stdscr) / 3) - 4;
  int inHeight = (getmaxy(stdscr) - 3) - 4;

  todoBoard = newwin(height, width, 0, startx);
  todoBoardIn = newwin(inHeight, inWidth, 2, inStartx);
  box(todoBoard, 0, 0);

  inStartx += width;
  startx += width;
  doingBoard = newwin(height, width, 0, startx);
  doingBoardIn = newwin(inHeight, inWidth, 2, inStartx);
  box(doingBoard, 0, 0);

  startx += width;
  inStartx += width;
  doneBoard = newwin(height, width, 0, startx);
  doneBoardIn = newwin(inHeight, inWidth, 2, inStartx);
  box(doneBoard, 0, 0);
}

void refreshBoard() {
  wrefresh(todoBoard);
  wrefresh(todoBoardIn);
  wrefresh(doingBoard);
  wrefresh(doingBoardIn);
  wrefresh(doneBoard);
  wrefresh(doneBoardIn);
}

void renderBoard() {
  initBoard();

  // Print todo
  title(todoBoard, "| TODO |");
  printList(todoBoardIn, boardList->todo, 1);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(doingBoardIn, boardList->doing, 2);

  // Print done
  title(doneBoard, "| DONE |");
  printList(doneBoardIn, boardList->done, 3);

  refreshBoard();
}

void driver(FORM **form) {
  pos_form_cursor(*form);
  form_driver(*form, REQ_PREV_FIELD);
  form_driver(*form, REQ_NEXT_FIELD);

  int ch;
  while (1) {
    ch = getch();

    // KEY_ENTER doesn't work, this works
    if (ch == 10) {
      break;
    }

    switch (ch) {
      case KEY_LEFT:form_driver(*form, REQ_PREV_CHAR);
        break;

      case KEY_RIGHT:form_driver(*form, REQ_NEXT_CHAR);
        break;

      case KEY_DOWN:form_driver(*form, REQ_NEXT_FIELD);
        form_driver(*form, REQ_END_LINE);
        break;

      case KEY_UP:form_driver(*form, REQ_PREV_FIELD);
        form_driver(*form, REQ_END_LINE);
        break;

      case KEY_BACKSPACE:form_driver(*form, REQ_DEL_PREV);
        break;

      default:form_driver(*form, ch);
        break;
    }

    wrefresh(fieldsWin);
  }

  // All good to continue
  form_driver(*form, REQ_LAST_FIELD);
  form_driver(*form, REQ_VALIDATION);
}

void initForm(FORM **form, FIELD **fields) {
  *form = new_form(fields);
  assert(form != NULL);
}

void renderForm(FORM **form) {
  formWin = newwin(24, 80, 0, 0);
  assert(formWin != NULL);
  fieldsWin = derwin(formWin, 20, 78, 3, 1);
  assert(fieldsWin != NULL);

  mvwprintw(formWin,
            1,
            2,
            "Arrows to select fields, ENTER to confirm all. You may only proceed to next field if the current is correct.");
  set_form_win(*form, fieldsWin);
  box(formWin, 0, 0);
  set_form_sub(*form, derwin(fieldsWin, 18, 76, 1, 1));
  box(fieldsWin, 0, 0);

  post_form(*form);
  refresh();
  wrefresh(formWin);
  wrefresh(fieldsWin);

  driver(form);
}

//////////////////////////////////////////////////////

/* MAIN UI LOOP */

void addChoice() {
  FIELD **fields = (FIELD **) malloc(4 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Priority [1-10]:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Description:");
  fields[3] = newFieldInputStr(1);
  assert(fields != NULL);

  initForm(&form, fields);

  // Render it
  renderForm(&form);

  // Reading priority
  char *priorityBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int priorityInt;
  readInt(&priorityInt, priorityBuffer);

  // Reading description
  char *descriptionBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(descriptionBuffer, field_buffer(form->field[3], 0));
  trimWhitespaces(descriptionBuffer);

  // Adding task to list
  addTask(priorityInt, descriptionBuffer);

  free(fields);
  free(form);

  choiceLoop();
}

void startChoice() {
  FIELD **fields = (FIELD **) malloc(6 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));
  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Assign to:");
  fields[3] = newFieldInputStr(1);
  fields[4] = newFieldPrompt(2, "Deadline (dd/mm/year):");
  fields[5] = newFieldInputDate(2);
  assert(fields != NULL);

  initForm(&form, fields);

  // Render it
  renderForm(&form);

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char *personBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(personBuffer, field_buffer(form->field[3], 0));
  trimWhitespaces(personBuffer);

  // Reading deadline
  char *deadlineBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(deadlineBuffer, field_buffer(form->field[5], 0));
  trimWhitespaces(deadlineBuffer);

  int day, month, year;
  char *dayBuffer = strtok(deadlineBuffer, "/");
  readInt(&day, dayBuffer);
  char *monthBuffer = strtok(NULL, "/");
  readInt(&month, monthBuffer);
  char *yearBuffer = strtok(NULL, "/");
  readInt(&year, yearBuffer);

  // Adding task to list
  workOnTask(idInt, day, month, year, personBuffer);

  free(fields);
  free(form);

  choiceLoop();
}

void closeChoice() {
  FIELD **fields = (FIELD **) malloc(2 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  assert(fields != NULL);

  initForm(&form, fields);

  // Render it
  renderForm(&form);

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Adding task to done
  closeTask(idInt);

  free(fields);
  free(form);

  choiceLoop();
}

void reAssign() {
  FIELD **fields = (FIELD **) malloc(4 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Assign to:");
  fields[3] = newFieldInputStr(1);
  assert(fields != NULL);

  initForm(&form, fields);

  // Render it
  renderForm(&form);

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char *personBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(personBuffer, field_buffer(form->field[3], 0));
  trimWhitespaces(personBuffer);

  // Adding task to done
  reassignTask(idInt, personBuffer);

  free(fields);
  free(form);

  choiceLoop();
}

void reOpen() {
  FIELD **fields = (FIELD **) malloc(2 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  assert(fields != NULL);

  initForm(&form, fields);

  // Render it
  renderForm(&form);

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Adding task to done
  reopenTask(idInt);

  free(fields);
  free(form);

  choiceLoop();
}

void getNextMenu(int choice) {
  switch (choice) {
    case 1:addChoice();
      break;

    case 2:startChoice();
      break;

    case 3:closeChoice();
      break;

    case 4:reAssign();
      break;

    case 5:reOpen();
      break;

    default: saveTasks(boardList->todo, boardList->doing, boardList->done);
  }
}

void choiceLoop() {
  int highlight = 1;
  int choice = 0;
  int c;

  int startx = getmaxx(stdscr);
  int starty = getmaxy(stdscr);

  nuke();
  renderBoard();
  renderMenu(highlight);

  // Press right highlight and select next option, left is the opposite
  while (1) {
    if (is_term_resized(starty, startx)) {
      nuke();
      renderBoard();
      renderMenu(highlight);
    }

    c = wgetch(menuWin);
    switch (c) {
      case KEY_LEFT:
        if (highlight == 1) {
          highlight = n_choices;
          renderMenu(highlight);
        } else {
          --highlight;
          renderMenu(highlight);
        }
        break;

      case KEY_RIGHT:
        if (highlight == n_choices) {
          highlight = 1;
          renderMenu(highlight);
        } else {
          ++highlight;
          renderMenu(highlight);
        }
        break;

      case 10:choice = highlight;
        break;

      default: break;
    }

    /* User did a choice come out of the infinite loop */
    if (choice != 0) {
      break;
    }
  }

  getNextMenu(choice);
}

void render(board_t *board_init) {
  initCore(board_init);
  boardList = board_init;

  initscr();
  clear();
  noecho();
  cbreak();    /* Line buffering disabled. pass on everything */
  keypad(stdscr, TRUE);

  choiceLoop();

  clrtoeol();
  refresh();
  endwin();
}