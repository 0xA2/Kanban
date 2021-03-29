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

FORM *form;
FIELD *fields[10];

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

void nuke() {
  delwin(fieldsWin);
  delwin(formWin);
  delwin(todoBoard);
  delwin(todoBoardIn);
  delwin(doingBoard);
  delwin(doingBoardIn);
  delwin(doneBoard);
  delwin(doneBoardIn);
}

//////////////////////////////////////////////////////

/* PRINT STUFF */

void printList(WINDOW *win, tasklist *list, int option) {
  wmove(win, 0, 0);

  whline(win, ACS_HLINE, getmaxx(win));
  for (int i = 0; i < listSize(list); ++i) {
    char *string = listPrint(list, i, option);
    wmove(win, getcury(win) + 1, 0);
    waddstr(win, string);
    wmove(win, getcury(win) + 1, 0);
    whline(win, ACS_HLINE, getmaxx(win));
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

void initBoard(int full) {
  nuke();

  int startx = 0;
  int height = 0;
  int width = 0;

  width = getmaxx(stdscr) / 3;
  height = getmaxy(stdscr) - 3;

  // If we are prompting a form
  if (full == 1) height = getmaxy(stdscr);

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

void renderBoard(int full) {
  initBoard(full);

  // Print todo
  title(todoBoard, "| TODO |");
  printList(todoBoardIn, boardList->todo, 1);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(doingBoardIn, boardList->doing, 2);

  // Print done
  title(doneBoard, "| DONE |");
  printList(doneBoardIn, boardList->done, 3);

  wrefresh(todoBoard);
  wrefresh(todoBoardIn);
  wrefresh(doingBoard);
  wrefresh(doingBoardIn);
  wrefresh(doneBoard);
  wrefresh(doneBoardIn);
}

void driver() {
  pos_form_cursor(form);
  form_driver(form, REQ_PREV_FIELD);
  form_driver(form, REQ_NEXT_FIELD);
  wrefresh(fieldsWin);

  int ch = 0;

  while (1) {
    ch = getch();

    // KEY_ENTER doesn't work, this works
    if (ch == 10) {
      break;
    }

    switch (ch) {
      case KEY_LEFT:form_driver(form, REQ_PREV_CHAR);
        break;

      case KEY_RIGHT:form_driver(form, REQ_NEXT_CHAR);
        break;

      case KEY_DOWN:form_driver(form, REQ_NEXT_FIELD);
        form_driver(form, REQ_END_LINE);
        break;

      case KEY_UP:form_driver(form, REQ_PREV_FIELD);
        form_driver(form, REQ_END_LINE);
        break;

      case KEY_BACKSPACE:form_driver(form, REQ_DEL_PREV);
        break;

      default:form_driver(form, ch);
        break;
    }

    wrefresh(fieldsWin);
  }

  form_driver(form, REQ_VALIDATION);
}

void renderForm() {
  nuke();

  formWin = newwin(24, 80, 0, 0);
  assert(formWin != NULL);
  box(formWin, 0, 0);

  fieldsWin = derwin(formWin, 20, 78, 3, 1);
  assert(fieldsWin != NULL);
  box(fieldsWin, 0, 0);
  mvwprintw(formWin, 1, 2, "Arrows to select fields, ENTER to confirm all.");

  form = new_form(fields);
  assert(form != NULL);

  set_form_win(form, fieldsWin);
  set_form_sub(form, derwin(fieldsWin, 18, 76, 1, 1));
  post_form(form);

  refresh();
  wrefresh(formWin);
  wrefresh(fieldsWin);

  driver();

  // All good to continue
  form_driver(form, REQ_LAST_FIELD);
  form_driver(form, REQ_VALIDATION);
}

void newFieldPrompt(int field, int pos, char *prompt) {
  fields[field] = new_field(1, 25, pos * 2, 0, 0, 0);
  set_field_buffer(fields[field], 0, prompt);
  set_field_opts(fields[field], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
}

void newFieldInputInt(int field, int pos, int min, int max) {
  fields[field] = new_field(1, 40, pos * 2, 25, 0, 0);
  set_field_type(fields[field], TYPE_NUMERIC, min, max);
  set_field_opts(fields[field], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(fields[field], A_UNDERLINE);
}

void newFieldInputStr(int field, int pos) {
  fields[field] = new_field(1, 40, pos * 2, 25, 0, 0);
  set_field_type(fields[field], TYPE_ALPHA, 1);
  set_field_opts(fields[field], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(fields[field], A_UNDERLINE);
}

void newFieldInputDate(int field, int pos) {
  fields[field] = new_field(1, 40, pos * 2, 25, 0, 0);
  set_field_type(fields[field], TYPE_REGEXP, "^\\d{1,2}\\/\\d{1,2}\\/\\d{4}$");
  set_field_opts(fields[field], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(fields[field], A_UNDERLINE);
}

//////////////////////////////////////////////////////

/* MAIN UI LOOP */

void addChoice() {
  newFieldPrompt(0, 0, "Priority [1-10]:");
  newFieldInputInt(1, 0, 0, 10);

  newFieldPrompt(2, 1, "Description:");
  newFieldInputStr(3, 1);

  assert(fields[0] != NULL && fields[1] != NULL && fields[2] != NULL && fields[3] != NULL);
  renderForm();

  // Reading priority
  char *priorityBuffer = strtok(field_buffer(fields[1], 0), " ");
  int priorityInt;
  readInt(&priorityInt, priorityBuffer);

  // Reading description
  char *descriptionBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(descriptionBuffer, field_buffer(fields[3], 0));
  trimWhitespaces(descriptionBuffer);

  // Adding task to list
  addTask(priorityInt, descriptionBuffer);

  choiceLoop();
}

void startChoice() {
  newFieldPrompt(0, 0, "Task ID:");
  newFieldInputInt(1, 0, 0, 10);

  newFieldPrompt(2, 1, "Assign to:");
  newFieldInputStr(3, 1);

  newFieldPrompt(4, 2, "Deadline (dd/mm/year):");
  newFieldInputDate(5, 2);

  renderForm();

  // Reading ID
  char *idBuffer = strtok(field_buffer(fields[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char *personBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(personBuffer, field_buffer(fields[3], 0));
  trimWhitespaces(personBuffer);

  // Reading deadline
  char *deadlineBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(deadlineBuffer, field_buffer(fields[5], 0));
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

  choiceLoop();
}

void getNextMenu(int choice) {
  switch (choice) {
    case 1:addChoice();
      break;

    case 2:startChoice();
      break;

    default: saveTasks(boardList->todo, boardList->doing, boardList->done);
  }
}

void choiceLoop() {
  int highlight = 1;
  int choice = 0;
  int c;

  // Press right highlight and select next option, left is the opposite
  while (1) {
    clear();
    renderBoard(0);
    renderMenu(highlight);

    c = wgetch(menuWin);

    switch (c) {
      case KEY_LEFT:
        if (highlight == 1) {
          highlight = n_choices;
        } else {
          --highlight;
        }
        break;

      case KEY_RIGHT:
        if (highlight == n_choices) {
          highlight = 1;
        } else {
          ++highlight;
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

  unpost_form(form);
  free_form(form);
  for (int i = 0; i < 10; ++i) {
    free_field(fields[i]);
  }

  clrtoeol();
  refresh();
  endwin();
}