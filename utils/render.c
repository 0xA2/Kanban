#include "render.h"

board_t *boardList;
WINDOW *menuWin;
WINDOW *boardWin;

char *choices[] = {
    "Add task",
    "Start task",
    "Close task",
    "Reassign task",
    "Reopen a task",
    "Exit",
};

int n_choices = sizeof(choices) / sizeof(char *);

// [ Here are the available features implemented in the UI ]
// btw this is needed lol, if you call a function it needs
// to be before it in the code, kinda meme way to do it
void addChoice();
////////////////////////////////////////////////////////////

void print_menu(int highlight) {
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

void clearWindow(WINDOW *win) {
  werase(win);
  wmove(win, 1, 2);
  wrefresh(win);
  box(win, 0, 0);
}

void printBoard() {
  clearWindow(boardWin);

  // Print stuff
  for (int i = 0; i < listSize(boardList->todo); ++i) {
    char *string = listPrintToDo(boardList->todo, i);
    waddstr(boardWin, string);
    wmove(boardWin, getcury(boardWin) + 1, 2);
  }

  wrefresh(boardWin);
}

void promptUser(char *string) {
  clearWindow(menuWin);
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

void getNextMenu(int choice) {
  switch (choice) {
    case 1:addChoice();
      break;

    default: break;
  }
  refresh();
}

void renderBoard() {
  // Upper left corner of menu
  int startx = 0;
  int starty = 0;

  // Menu dimensions
  int width = COLS;
  int height = LINES - 3;

  // Selection Menu Dimensions
  boardWin = newwin(height, width, starty, startx);

  // Print the thing
  wrefresh(boardWin);

  // Print board :)
  printBoard();
}

void renderMenu() {
  int highlight = 1;
  int choice = 0;
  int c;

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
  print_menu(highlight);

  // Start the board
  renderBoard();

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

    print_menu(highlight);

    /* User did a choice come out of the infinite loop */
    if (choice != 0) {
      break;
    }
  }

  wmove(menuWin, 1, 2);

  wrefresh(menuWin);
  getNextMenu(choice);
}

void addChoice() {
  char *priorityPrompt = "Task Priority [1-10] >";
  char *descriptionPrompt = "Task Description >";
  char *description = (char *) malloc(1024);
  int priority;

  readInputInt(&priority, priorityPrompt);

  readInputString(&description, descriptionPrompt);

  addTask(priority, description);
  printBoard();
  renderMenu();
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


