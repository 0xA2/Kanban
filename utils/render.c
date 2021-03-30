#include "render.h"

//////////////////////////////////////////////////////
board_t *boardList;

WINDOW *menuWin;
WINDOW *boardWin;
WINDOW *todoBoard;
WINDOW *todo;
WINDOW *doingBoard;
WINDOW *doing;
WINDOW *doneBoard;
WINDOW *done;

WINDOW *fieldsWin;
WINDOW *formWin;

FIELD *fields[8];
FORM *form;

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

void driver() {
  int ch;
  while (1) {
    if (current_field(form) == fields[field_count(form) - 1]) {
      set_field_back(fields[field_count(form) - 1], A_REVERSE);
    } else {
      set_field_back(fields[field_count(form) - 1], A_BOLD);
    }

    ch = getch();

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

      case 10:
        if (current_field(form) == fields[field_count(form) - 1]) {
          form_driver(form, REQ_VALIDATION);
          return;
        } else {
          form_driver(form, REQ_NEXT_FIELD);
        }
        break;

      default:form_driver(form, ch);
        break;
    }

    wrefresh(fieldsWin);
  }
}

void renderForm() {
  refresh();
  wrefresh(formWin);
  wrefresh(fieldsWin);

  form = new_form(fields);
  assert(form != NULL);

  formWin = derwin(stdscr, getmaxy(stdscr) - 4, getmaxx(stdscr) - 4, getmaxy(stdscr) - 2, getmaxx(stdscr) - 2);
  fieldsWin = derwin(formWin, getmaxy(formWin) - 2, getmaxx(formWin) - 2, 1, 1);
  box(formWin, 0, 0);

  set_form_win(form, formWin);
  set_form_sub(form, derwin(fieldsWin, getmaxy(fieldsWin), getmaxx(fieldsWin), 0, 0));
  post_form(form);
  pos_form_cursor(form);

  refresh();
  wrefresh(formWin);
  wrefresh(fieldsWin);

  driver();
}

//////////////////////////////////////////////////////

/* MAIN UI LOOP */

void nuke() {
  clear();
  clrtoeol();
  refresh();
  endwin();

  initscr();
  clear();
  noecho();
  keypad(stdscr, TRUE);
  refresh();
}

void addChoice() {
  fields[0] = newFieldPrompt(0, "Priority [1-10]:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Description:");
  fields[3] = newFieldInputStr(1);
  fields[4] = newFieldButton(2, "[ OK ]");

  for (int i = 0; i < 5; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm();

  if (form_driver(form, REQ_VALIDATION) != 0) {
    addChoice();
  }

  // Reading priority
  char *priorityBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int priorityInt;
  readInt(&priorityInt, priorityBuffer);

  // Reading description
  char descriptionBuffer[80];
  strcpy(descriptionBuffer, field_buffer(form->field[3], 0));
  trimWhitespaces(descriptionBuffer);

  // Adding task to list
  addTask(priorityInt, descriptionBuffer);

  freeFormFields(form, fields);
  choiceLoop();
}

void startChoice() {
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

  renderForm();

  if (form_driver(form, REQ_VALIDATION) != 0) {
    startChoice();
  }

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char personBuffer[80];
  strcpy(personBuffer, field_buffer(form->field[3], 0));
  trimWhitespaces(personBuffer);

  // Reading deadline
  char deadlineBuffer[80];
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

  if (!(e1 || e2 || e3) || !isValidDate(day, month, year)) {
    freeFormFields(form, fields);
    startChoice();
  } else {
    // Adding task to list
    workOnTask(idInt, day, month - 1, year, personBuffer);
    freeFormFields(form, fields);
    choiceLoop();
  }
}

void closeChoice() {
  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldButton(1, "[ OK ]");

  for (int i = 0; i < 3; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm();

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Adding task to done
  closeTask(idInt);

  freeFormFields(form, fields);
  choiceLoop();
}

void reAssign() {
  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldPrompt(1, "Assign to:");
  fields[3] = newFieldInputStr(1);
  fields[4] = newFieldButton(2, "[ OK ]");

  for (int i = 0; i < 5; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm();

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char personBuffer[80];
  strcpy(personBuffer, field_buffer(form->field[3], 0));
  trimWhitespaces(personBuffer);

  // Adding task to done
  reassignTask(idInt, personBuffer);

  freeFormFields(form, fields);
  choiceLoop();
}

void reOpen() {
  fields[0] = newFieldPrompt(0, "Task ID:");
  fields[1] = newFieldInputInt(0, 0, 10);
  fields[2] = newFieldButton(1, "[ OK ]");

  for (int i = 0; i < 3; ++i) {
    assert(fields[i] != NULL);
  }

  renderForm();

  // Reading ID
  char *idBuffer = strtok(field_buffer(form->field[1], 0), " ");
  int idInt;
  readInt(&idInt, idBuffer);

  // Adding task to done
  reopenTask(idInt);

  freeFormFields(form, fields);
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
  refresh();

  // Press right highlight and select next option, left is the opposite
  while (1) {

    if (is_term_resized(starty, startx)) {
      nuke();

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

    renderMenu(highlight);

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
  if (pad < 0)
    return;

  // Print stuff
  for (i = 0; i < n_choices; ++i) {
    if (highlight == i + 1) {
      wattron(menuWin, A_REVERSE);
      mvwaddstr(menuWin, y, x, choices[i]);
      wattroff(menuWin, A_REVERSE);
    } else
      mvwaddstr(menuWin, y, x, choices[i]);
    x += pad + (int) strlen(choices[i]);
  }
  assert(menuWin != NULL);

  wrefresh(menuWin);
}

void renderBoard() {
  int h, w, bh, bw;
  h = getmaxy(stdscr) - 3;
  w = getmaxx(stdscr) / 3;

  // Portion of screen occupied by the board
  boardWin = newwin(getmaxy(stdscr) - 3, getmaxx(stdscr), 0, 0);

  // Each list box
  todoBoard = derwin(boardWin, h, w, 0, 0);
  doingBoard = derwin(boardWin, h, w, 0, w);
  doneBoard = derwin(boardWin, h, w, 0, 2 * w);

  box(todoBoard, 0, 0);
  box(doingBoard, 0, 0);
  box(doneBoard, 0, 0);

  // Boxes inside the previous ones to prevent text from overflowing
  // - Getting the height and width
  // - Since there are 3 windows, one of them might be smaller
  // - We have to make this check for every. single. one.
  bh = (getmaxy(todoBoard)) - 4;
  bw = (getmaxx(todoBoard)) - 4;
  todo = derwin(todoBoard, bh, bw, 2, 2);

  bh = (getmaxy(doingBoard)) - 4;
  bw = (getmaxx(doingBoard)) - 4;
  doing = derwin(doingBoard, bh, bw, 2, 2);

  bh = (getmaxy(doneBoard)) - 4;
  bw = (getmaxx(doneBoard)) - 4;
  done = derwin(doneBoard, bh, bw, 2, 2);

  // Print todo
  title(todoBoard, "| TODO |");
  printList(todo, boardList->todo, 1);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(doing, boardList->doing, 2);

  // Print done
  title(doneBoard, "| DONE |");
  printList(done, boardList->done, 3);

  wrefresh(boardWin);
  wrefresh(todoBoard);
  wrefresh(doingBoard);
  wrefresh(doneBoard);
  wrefresh(todo);
  wrefresh(doing);
  wrefresh(done);
}

void render(board_t *board_init) {
  initCore(board_init);
  boardList = board_init;

  initscr();
  clear();
  noecho();
  keypad(stdscr, TRUE);

  choiceLoop();

  clrtoeol();
  refresh();
  endwin();
}

//////////////////////////////////////////////////////