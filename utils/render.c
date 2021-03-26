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

void promptUser(char *string) {
  wclear(menuWin);
  box(menuWin, 0, 0);
  wmove(menuWin, 1, 2);

  wattron(menuWin, A_REVERSE);
  mvwprintw(menuWin, getcury(menuWin), getcurx(menuWin), string);
  wattroff(menuWin, A_REVERSE);

  wmove(menuWin, getcury(menuWin), getcurx(menuWin) + 1);
  wrefresh(menuWin);
}

void readInputInt(int *value, char *prompt) {
  promptUser(prompt);

  // Read and output visual feedback
  int n;
  int c = 0;

  while (c != '\n') {
    c = wgetch(menuWin);
    n = c - '0';

    if (n <= 9 && n >= 0) {
      if (*value == 1 && n == 0) {
        *value = 10;
      } else {
        *value = n;
      }
      mvwprintw(menuWin, getcury(menuWin), getcurx(menuWin), "%d", n);
    }
  }

  promptUser("Confirm? (y/n)");
  int confirm = wgetch(menuWin);
  if (confirm != 'y') {
    readInputInt(value, prompt);
  }
}

void readInputString(char **value, char *prompt) {
  promptUser(prompt);

  int initial_x = getcurx(menuWin) + 1;
  int initial_y = getcury(menuWin);
  wmove(menuWin, initial_y, initial_x);

  // Read and output visual feedback
  int c = 0;
  char ch;

  while (c != '\n') {
    c = wgetch(menuWin);
    ch = (char) (c);

    if ((ch <= 'z' && ch >= 'A') || ch == ' ') {
      mvwprintw(menuWin, getcury(menuWin), getcurx(menuWin), "%c", c);
      strncat(*value, &ch, 1);
    }
  }

  promptUser("Confirm? (y/n)");

  int confirm = wgetch(menuWin);
  if (confirm != 'y') {
    readInputString(value, prompt);
  }
}

//////////////////////////////////////////////////////

/* PRINT STUFF */

void printList(WINDOW *win, tasklist *list) {
  werase(win);
  wmove(win, 0, 0);

  whline(win, ACS_HLINE, getmaxx(win));
  for (int i = 0; i < listSize(list); ++i) {
    char *string = listPrintToDo(list, i);
    wmove(win, getcury(win) + 1, 0);
    waddstr(win, string);
    wmove(win, getcury(win) + 1, 0);
    whline(win, ACS_HLINE, getmaxx(win));
  }

  wrefresh(win);
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
  printList(todoBoardIn, boardList->todo);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(doingBoardIn, boardList->todo);

  // Print done
  title(doneBoard, "| DONE |");
  printList(doneBoardIn, boardList->todo);

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
  char *priorityPrompt = "Task Priority [1-10] >";
  char *descriptionPrompt = "Task Description >";
  char *description = (char *) (malloc(80 * sizeof(char)));
  int priority;

  readInputInt(&priority, priorityPrompt);

  readInputString(&description, descriptionPrompt);

  addTask(priority, description);

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




