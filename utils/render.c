#include "render.h"

//////////////////////////////////////////////////////
board_t *boardList;
WINDOW *menuWin;
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

char *trimwhitespace(char *str) {
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

void driver(FORM *form) {
  pos_form_cursor(form);
  refresh();

  int ch = 0;

  while (1) {
    ch = wgetch(menuWin);

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

    wrefresh(stdscr);
  }

  // All good to continue
  form_driver(form, REQ_LAST_FIELD);
  form_driver(form, REQ_VALIDATION);
}

void title(WINDOW *win, char *title) {
  int startx = (getmaxx(win) - (int) strlen(title)) / 2;
  wmove(win, 0, startx);
  wattron(win, A_REVERSE);
  waddstr(win, title);
  wattroff(win, A_REVERSE);
  wrefresh(win);
}

void initBoard() {
  int startx = 0;
  int width = getmaxx(stdscr) / 3;
  int height = getmaxy(stdscr) - 3;

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

  wrefresh(todoBoard);
  wrefresh(todoBoardIn);
  wrefresh(doingBoard);
  wrefresh(doingBoardIn);
  wrefresh(doneBoard);
  wrefresh(doneBoardIn);
}

//////////////////////////////////////////////////////

/* MAIN UI LOOP */

void addChoice() {
  FORM *form;
  FIELD *fields[4];

  fields[0] = new_field(1, 20, 0, 0, 0, 0);
  fields[1] = new_field(1, 40, 0, 20, 0, 0);
  fields[2] = new_field(1, 20, 2, 0, 0, 0);
  fields[3] = new_field(1, 40, 2, 20, 0, 0);

  set_field_type(fields[1], TYPE_NUMERIC, 0, 10);
  set_field_type(fields[3], TYPE_ALPHA, 40);

  set_field_buffer(fields[0], 0, "Priority [1-10]:");
  set_field_buffer(fields[2], 0, "Description:");

  // Skip non input fields
  set_field_opts(fields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
  set_field_opts(fields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_opts(fields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
  set_field_opts(fields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

  // Input fields look
  set_field_back(fields[1], A_UNDERLINE);
  set_field_back(fields[3], A_UNDERLINE);

  form = new_form(fields);
  set_form_win(form, stdscr);
  set_form_sub(form, derwin(stdscr, 18, 76, 1, 1));
  post_form(form);

  wrefresh(stdscr);

  driver(form);

  // Reading priority
  char *priorityBuffer = strtok(field_buffer(fields[1], 0), " ");
  int priorityInt;
  readInt(&priorityInt, priorityBuffer);

  // Reading description
  char *descriptionBuffer = (char *) malloc(40 * sizeof(char));
  strcpy(descriptionBuffer, field_buffer(fields[3], 0));
  trimwhitespace(descriptionBuffer);

  addTask(priorityInt, descriptionBuffer);

  unpost_form(form);

  free_form(form);
  free_field(fields[0]);
  free_field(fields[1]);
  free_field(fields[2]);
  free_field(fields[3]);

  choiceLoop();
}

void getNextMenu(int choice) {
  switch (choice) {
    case 1:addChoice();
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
    renderBoard();
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

  choiceLoop();

  clrtoeol();
  refresh();
  endwin();
}