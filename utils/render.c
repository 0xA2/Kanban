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
//////////////////////////////////////////////////////
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
void choiceLoop();
//////////////////////////////////////////////////////

void clearWindow(WINDOW *win) {
  werase(win);
  wmove(win, 1, 2);
  wrefresh(win);
}

void initBoard() {
  int startx = 0;
  int width = (COLS - 2) / 3;
  int height = LINES - 3;

  todoBoard = newwin(height, width, 0, startx);
  todoBoardIn = newwin(height - 2, width - 2, 1, startx + 1);
  box(todoBoard, 0, 0);

  startx += width + 1;
  doingBoard = newwin(height, width, 0, startx);
  doingBoardIn = newwin(height - 2, width - 2, 1, startx + 1);
  box(doingBoard, 0, 0);

  startx += width + 1;
  doneBoard = newwin(height, width, 0, startx);
  doneBoardIn = newwin(height - 2, width - 2, 1, startx + 1);
  box(doneBoard, 0, 0);
}

void windowsRefresh() {
  wrefresh(todoBoard);
  wrefresh(doingBoard);
  wrefresh(doneBoard);
  wrefresh(todoBoardIn);
  wrefresh(doingBoardIn);
  wrefresh(doneBoardIn);
  wrefresh(menuWin);
}

void printList(WINDOW *win, tasklist *list) {
  for (int i = 0; i < listSize(list); ++i) {
    char *string = listPrintToDo(list, i);
    waddstr(win, string);
    wmove(win, getcury(win) + 1, 0);
  }
}

void title(WINDOW *win, char *title) {
  int startx = (getmaxx(win) - (int) strlen(title)) / 2;
  wmove(win, 0, startx);
  wattron(win, A_REVERSE);
  waddstr(win, title);
  wattroff(win, A_REVERSE);
}

void refreshBoard() {
  refresh();
  clearok(todoBoardIn, true);
  clearok(doingBoardIn, true);
  clearok(doneBoardIn, true);

  // Print todo
  title(todoBoard, "| TODO |");
  printList(todoBoardIn, boardList->todo);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(todoBoardIn, boardList->todo);

  // Print done
  title(doneBoard, "| DONE |");
  printList(todoBoardIn, boardList->todo);

  windowsRefresh();
}

void promptUser(char *string) {
  clearWindow(menuWin);
  box(menuWin, 0, 0);
  wattron(menuWin, A_REVERSE);
  mvwprintw(menuWin, getcury(menuWin), getcurx(menuWin), string);
  wattroff(menuWin, A_REVERSE);
  wrefresh(menuWin);
  wmove(menuWin, getcury(menuWin), getcurx(menuWin) + 1);
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
  wrefresh(menuWin);
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
  wrefresh(menuWin);
}

void printMenu(int highlight) {
  int x, y, i, pad;

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

  wmove(menuWin, 1, 2);
  wrefresh(menuWin);
}

void addChoice() {
  char *priorityPrompt = "Task Priority [1-10] >";
  char *descriptionPrompt = "Task Description >";
  char *description = (char *) (malloc(80 * sizeof(char)));
  int priority;

  readInputInt(&priority, priorityPrompt);

  readInputString(&description, descriptionPrompt);

  addTask(priority, description);
  refreshBoard();
  windowsRefresh();
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

    printMenu(highlight);

    /* User did a choice come out of the infinite loop */
    if (choice != 0) {
      break;
    }
  }

  wrefresh(menuWin);
  getNextMenu(choice);
}

void renderMenu() {
  // Upper left corner of menu
  int startx = 0;
  int starty = LINES - 3;

  // Menu dimensions
  int width = COLS;
  int height = 3;

  // Selection Menu Dimensions
  menuWin = newwin(height, width, starty, startx);
  keypad(menuWin, TRUE);
  refresh();

  // Print the thing
  printMenu(1);

  // Start the board
  initBoard();
  refreshBoard();

  choiceLoop();
}

void render(board_t *board_init) {
  initCore(board_init);
  boardList = board_init;

  initscr();
  clear();
  noecho();
  cbreak();    /* Line buffering disabled. pass on everything */

  renderMenu();

  clrtoeol();
  refresh();
  endwin();
}


