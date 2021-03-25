#include "render.h"

/*
 * utils
 */
char *choices[] = {
    "Add task",
    "Start task",
    "Close task",
    "Reassign task",
    "Reopen a task",
    "Exit",
};

int n_choices = sizeof(choices) / sizeof(char *);

int strSize(char *str) {
  return (sizeof(str) / sizeof(char));
}

void updateSize() {
  n_choices = sizeof(choices) / sizeof(char *);
}

/*
 * Menu shenanigans
 */
void restartMenu() {
  choices[0] = "Add task";
  choices[1] = "Start task";
  choices[2] = "Close task";
  choices[3] = "Reassign task";
  choices[4] = "Reopen a task";
  choices[5] = "Exit";
}

void print_menu(WINDOW *menuWin, int highlight) {
  int x, y, i, pad;

  // Initial values, initial "padding"
  x = 2;
  y = 1;

  box(menuWin, 0, 0);

  // Get padding
  int effectiveSize = 0;

  for (int j = 0; j < n_choices; ++j) {
    effectiveSize += strSize(choices[j]);
  }
  pad = (getmaxx(menuWin) - effectiveSize) / (n_choices - 1);

  // Print stuff
  for (i = 0; i < n_choices; ++i) {
    // Highlight the current choice
    if (highlight == i + 1) {
      wattron(menuWin, A_REVERSE);
      mvwprintw(menuWin, y, x, "%s", choices[i]);
      wattroff(menuWin, A_REVERSE);
    } else
      mvwprintw(menuWin, y, x, "%s", choices[i]);
    x += pad + strSize(choices[i]);
  }

  wrefresh(menuWin);
}

void getNextMenu(int choice) {
  switch (choice) {
    case 1:

      addTask();
      break;

    case 2: workOnTask();
      break;

    case 3: closeTask();
      break;

    case 4: reassignTask();
      break;

    case 5: reopenTask();
      break;

    default: refresh();
  }
}

void renderMenu() {
  WINDOW *menuWin;
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
  mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
  refresh();

  // Print the thing
  print_menu(menuWin, highlight);

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

      default:mvprintw(1, 0, "Character pressed is = %3d Hopefully it can be printed as '%c'", c, c);
        refresh();
        break;
    }

    print_menu(menuWin, highlight);

    /* User did a choice come out of the infinite loop */
    if (choice != 0) {
      break;
    }
  }

  mvprintw(1, 0, "You chose (%s)\n", choices[choice - 1]);
  getNextMenu(choice);
}

/*
 * Board shenanigans
 */
void print_board(WINDOW *boardWin) {
}

void renderBoard() {
  WINDOW *boardWin;

  // Upper left corner of menu
  int startx = 4;
  int starty = 2;

  // Menu dimensions
  int width = COLS;
  int height = LINES - 3 - 4;

  // Selection Menu Dimensions
  boardWin = newwin(height, width, starty, startx);
  keypad(boardWin, TRUE);

  // Print the thing
  refresh();

  // Print board :)
  print_board(boardWin);
}

/*
 * Actually do stuff
 */
void render(board_t* board) {
  initscr();
  clear();
  noecho();
  cbreak();    /* Line buffering disabled. pass on everything */

  renderMenu();
  renderBoard();

  clrtoeol();
  refresh();
  endwin();
}

