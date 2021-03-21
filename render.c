#include <ncurses.h>
#include <stdlib.h>

#include "render.h"
#include "core.h"

/*
 * Utils
 */
tasklist *todo;
tasklist *doing;
tasklist *done;

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

void print_menu(WINDOW *menu_win, int highlight) {
  int x, y, i, pad;

  // Initial values, initial "padding"
  x = 2;
  y = 1;

  box(menu_win, 0, 0);

  // Get padding
  int effectiveSize = 0;

  for (int j = 0; j < n_choices; ++j) {
    effectiveSize += strSize(choices[j]);
  }
  pad = (getmaxx(menu_win) - effectiveSize) / (n_choices - 1);

  // Print stuff
  for (i = 0; i < n_choices; ++i) {
    // Highlight the current choice
    if (highlight == i + 1) {
      wattron(menu_win, A_REVERSE);
      mvwprintw(menu_win, y, x, "%s", choices[i]);
      wattroff(menu_win, A_REVERSE);
    } else
      mvwprintw(menu_win, y, x, "%s", choices[i]);
    x += pad + strSize(choices[i]);
  }

  wrefresh(menu_win);
}

void getNextMenu(int choice) {
  switch (choice) {
  case 1:addTask(todo, doing, done);
    break;

  case 2:workOnTask(todo, doing);
    break;

  case 3:closeTask(doing, done);
    break;

  case 4:reAssignTask(doing);
    break;

  case 5:reopenTask(todo, done);
    break;

  default:return;
  }
}

void renderMenu() {
  WINDOW *menu_win;
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
  menu_win = newwin(height, width, starty, startx);
  keypad(menu_win, TRUE);
  mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
  refresh();

  // Print the thing
  print_menu(menu_win, highlight);

  // Press right highlight and select next option, left is the opposite
  while (1) {
    c = wgetch(menu_win);

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

    print_menu(menu_win, highlight);

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
  int x, y, i;

  // Padding
  x = 2;
  y = 1;

  box(boardWin, 0, 0);

  // Printing the TODO
  for (i = 0; i < todo->size; ++i) {
    mvwprintw(boardWin, y, x, "%s", listPrintToDo(todo, i + 1));
    y += 1;
  }

  wrefresh(boardWin);
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
void render(tasklist *todoR, tasklist *doingR, tasklist *doneR) {
  todo = todoR;
  doing = doingR;
  done = doneR;

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

