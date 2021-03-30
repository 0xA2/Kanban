#include "render.h"

//////////////////////////////////////////////////////
board_t *boardList;

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

//////////////////////////////////////////////////////

/* MAIN UI LOOP */

void addChoice() {
  FIELD **fields = (FIELD **) malloc(5 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(10 * sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Priority [1-10]:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Description:");
  fields[3] = newFieldInputStr(1);
  fields[4] = newFieldButton(2, "[ OK ]");

  for (int i = 0; i < 5; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm(&form, &fields);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    addChoice();
  }

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

  choiceLoop();
}

void startChoice() {
  FIELD **fields = (FIELD **) malloc(7 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));
  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Assign to:");
  fields[3] = newFieldInputStr(1);
  fields[4] = newFieldPrompt(2, "Deadline (dd/mm/year):");
  fields[5] = newFieldInputDate(2);
  fields[6] = newFieldButton(3, "[ OK ]");

  for (int i = 0; i < 7; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm(&form, &fields);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    startChoice();
  }

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

  int day, e1, month, e2, year, e3;
  char *dayBuffer = strtok(deadlineBuffer, "/");
  char *monthBuffer = strtok(NULL, "/");
  char *yearBuffer = strtok(NULL, "/");

  // Checking errors
  e1 = readInt(&day, dayBuffer);
  e2 = readInt(&month, monthBuffer);
  e3 = readInt(&year, yearBuffer);

  if (!(e1 || e2 || e3) || isValidDate(day, month, year)) {
    startChoice();
  } else {
    // Adding task to list
    workOnTask(idInt, day, month - 1, year, personBuffer);
    choiceLoop();
  }
}

void closeChoice() {
  FIELD **fields = (FIELD **) malloc(3 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldButton(1, "[ OK ]");

  for (int i = 0; i < 3; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm(&form, &fields);

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Adding task to done
  closeTask(idInt);

  choiceLoop();
}

void reAssign() {
  FIELD **fields = (FIELD **) malloc(5 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Assign to:");
  fields[3] = newFieldInputStr(1);
  fields[4] = newFieldButton(2, "[ OK ]");

  for (int i = 0; i < 5; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm(&form, &fields);

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

  choiceLoop();
}

void reOpen() {
  FIELD **fields = (FIELD **) malloc(3 * sizeof(FIELD));
  FORM *form = (FORM *) malloc(sizeof(FORM));

  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldButton(1, "[ OK ]");

  for (int i = 0; i < 3; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm(&form, &fields);

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Adding task to done
  reopenTask(idInt);

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

  renderBoard();
  renderMenu(highlight);

  // Press right highlight and select next option, left is the opposite
  while (1) {
    if (is_term_resized(starty, startx)) {
      startx = getmaxx(stdscr);
      starty = getmaxy(stdscr);

      renderBoard();
      renderMenu(highlight);
    }

    c = getch();
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

//////////////////////////////////////////////////////

/* RENDER */

void renderMenu(int highlight) {
  refresh();
  WINDOW **menuWin = (WINDOW **) malloc(sizeof(WINDOW *));
  int x, y, i, pad;

  // Upper left corner of menu
  int startx = 0;
  int starty = getmaxy(stdscr) - 3;

  // Menu dimensions
  int width = getmaxx(stdscr);
  int height = 3;

  // Selection Menu Dimensions
  *menuWin = newwin(height, width, starty, startx);
  keypad(*menuWin, TRUE);

  // Initial values, initial "padding"
  x = 2;
  y = 1;

  box(*menuWin, 0, 0);

  // Get padding
  int effectiveSize = 0;

  for (int j = 0; j < n_choices; ++j) {
    effectiveSize += (int) strlen(choices[j]);
  }
  pad = (getmaxx(*menuWin) - effectiveSize) / n_choices;

  // Print stuff
  for (i = 0; i < n_choices; ++i) {
    if (highlight == i + 1) {
      wattron(*menuWin, A_REVERSE);
      mvwaddstr(*menuWin, y, x, choices[i]);
      wattroff(*menuWin, A_REVERSE);
    } else
      mvwaddstr(*menuWin, y, x, choices[i]);
    x += pad + (int) strlen(choices[i]);
  }
  assert(menuWin != NULL);

  wrefresh(*menuWin);
}

void renderBoard() {
  refresh();
  int h, w, bh, bw;
  h = getmaxy(stdscr) - 3;
  w = getmaxx(stdscr) / 3;

  WINDOW **board = (WINDOW **) malloc(sizeof(WINDOW *));
  WINDOW **todoBoard = (WINDOW **) malloc(sizeof(WINDOW *));
  WINDOW **todo = (WINDOW **) malloc(sizeof(WINDOW *));
  WINDOW **doingBoard = (WINDOW **) malloc(sizeof(WINDOW *));
  WINDOW **doing = (WINDOW **) malloc(sizeof(WINDOW *));
  WINDOW **doneBoard = (WINDOW **) malloc(sizeof(WINDOW *));
  WINDOW **done = (WINDOW **) malloc(sizeof(WINDOW *));

  // Portion of screen occupied by the board
  *board = newwin(getmaxy(stdscr) - 3, getmaxx(stdscr), 0, 0);

  // Each list box
  *todoBoard = derwin(*board, h, w, 0, 0);
  *doingBoard = derwin(*board, h, w, 0, w);
  *doneBoard = derwin(*board, h, w, 0, 2 * w);

  box(*todoBoard, 0, 0);
  box(*doingBoard, 0, 0);
  box(*doneBoard, 0, 0);

  // Boxes inside the previous ones to prevent text from overflowing
  // - Getting the height and width
  // - Since there are 3 windows, one of them might be smaller
  // - We have to make this check for every. single. one.
  bh = (getmaxy(*todoBoard)) - 4;
  bw = (getmaxx(*todoBoard)) - 4;
  *todo = derwin(*todoBoard, bh, bw, 2, 2);

  bh = (getmaxy(*doingBoard)) - 4;
  bw = (getmaxx(*doingBoard)) - 4;
  *doing = derwin(*doingBoard, bh, bw, 2, 2);

  bh = (getmaxy(*doneBoard)) - 4;
  bw = (getmaxx(*doneBoard)) - 4;
  *done = derwin(*doneBoard, bh, bw, 2, 2);

  // Print todo
  title(todoBoard, "| TODO |");
  printList(*todo, boardList->todo, 1);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(*doing, boardList->doing, 2);

  // Print done
  title(doneBoard, "| DONE |");
  printList(*done, boardList->done, 3);

  wrefresh(*board);
  wrefresh(*todoBoard);
  wrefresh(*doingBoard);
  wrefresh(*doneBoard);
  wrefresh(*todo);
  wrefresh(*doing);
  wrefresh(*done);
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

//////////////////////////////////////////////////////