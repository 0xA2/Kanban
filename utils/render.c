#include "render.h"

//////////////////////////////////////////////////////
board_t *boardList;

int maxx;
int maxy;
int currentBoard;

WINDOW *menuWin;
WINDOW *boardWin;
WINDOW *allBoard;
WINDOW *all;
WINDOW *todoBoard;
WINDOW *todo;
WINDOW *doingBoard;
WINDOW *doing;
WINDOW *doneBoard;
WINDOW *done;
WINDOW *fieldsWin;
WINDOW *formWin;

static const char *choices[] = {
    "Add task",
    "Start task",
    "Close task",
    "Reassign task",
    "Reopen a task",
    "Tasks by person",
};

static int n_choices = sizeof(choices) / sizeof(char *);

//////////////////////////////////////////////////////

// clears windows, refreshes screen
// any window that was on screen needs to be refreshed afterwards
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

void driver(FORM *form, FIELD **fields) {
  // allow arrow keys
  keypad(fieldsWin, TRUE);
  // position cursor on form
  pos_form_cursor(form);

  int ch;
  while (1) {

    // highlight the button field
    if (current_field(form) == fields[field_count(form) - 1]) {
      set_field_back(fields[field_count(form) - 1], A_STANDOUT);
      form_driver(form, REQ_END_LINE);
    } else {
      set_field_back(fields[field_count(form) - 1], A_BOLD);
    }

    ch = wgetch(fieldsWin);

    switch (ch) {
    case KEY_LEFT:form_driver(form, REQ_PREV_CHAR);
      break;

    case KEY_RIGHT:form_driver(form, REQ_NEXT_CHAR);
      break;

    case KEY_DOWN:
      form_driver(form, REQ_NEXT_FIELD);
      form_driver(form, REQ_END_LINE);
      break;

    case KEY_UP:
      form_driver(form, REQ_PREV_FIELD);
      form_driver(form, REQ_END_LINE);
      break;

    // depending on the terminal emulator, the key code provided might be different
    case KEY_BACKSPACE:form_driver(form, REQ_DEL_PREV);
      break;

    case KEY_DC:form_driver(form, REQ_DEL_PREV);
      break;

    case 127:form_driver(form, REQ_DEL_PREV);
      break;

    case KEY_ENTER:
      if (current_field(form) == fields[field_count(form) - 1]) {
        form_driver(form, REQ_VALIDATION);
        return;
      } else {
        form_driver(form, REQ_NEXT_FIELD);
      }
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

    wrefresh(formWin);
    wrefresh(fieldsWin);
  }
}

FORM *renderForm(struct field_info *options, int s) {
  mvprintw(0, 1, "Navigate with the arrow keys, you need to complete each field correctly before continuing.");

  FIELD **fields = (FIELD **) malloc((s + 1) * sizeof(FIELD));
  formWin = derwin(boardWin, getmaxy(boardWin), getmaxx(boardWin), 0, 0);
  fieldsWin = derwin(formWin, getmaxy(boardWin) - 2, getmaxx(boardWin) - 2, 1, 1);
  box(formWin, 0, 0);

  for (int i = 0; i < s; ++i) {
    if (strcmp(options[i].type, "prompt") == 0) {
      fields[i] = new_field(1, 25, options[i].number * 2, 0, 0, 0);
      set_field_buffer(fields[options[i].number], 0, options[i].label);
      set_field_opts(fields[options[i].number], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);

    } else if (strcmp(options[i].type, "button") == 0) {
      fields[i] = new_field(1, 6, options[i].number * 2, 0, 0, 0);
      set_field_buffer(fields[options[i].number], 0, options[i].label);
      set_field_opts(fields[options[i].number], O_VISIBLE | O_PUBLIC | O_ACTIVE);

    } else if (strcmp(options[i].type, "input_int") == 0) {
      fields[i] = new_field(1, 40, options[i].number * 2, 0, 0, 0);
      set_field_opts(fields[options[i].number], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
      set_field_back(fields[options[i].number], A_UNDERLINE);
      set_field_type(fields[options[i].number], TYPE_INTEGER, 1, 1, 1000);

    } else if (strcmp(options[i].type, "input_str") == 0) {
      fields[i] = new_field(1, 40, options[i].number * 2, 0, 0, 0);
      set_field_opts(fields[options[i].number], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
      set_field_back(fields[options[i].number], A_UNDERLINE);
      set_field_type(fields[options[i].number], TYPE_REGEXP, "^ *[a-zA-Z0-9 !?]{0,30} *$");

    } else if (strcmp(options[i].type, "input_date") == 0) {
      fields[i] = new_field(1, 40, options[i].number * 2, 0, 0, 0);
      set_field_opts(fields[options[i].number], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
      set_field_back(fields[options[i].number], A_UNDERLINE);
      set_field_type(fields[options[i].number],
                     TYPE_REGEXP,
                     "([0-2][0-9]|3[01])\\/(0[0-9]|1[0-2])\\/([01][0-9][0-9]|20[0-9][0-9])");
    }
  }

  assert(fields != NULL);
  fields[s] = NULL;

  FORM *form = new_form(fields);
  assert(form != NULL);

  set_form_win(form, formWin);
  set_form_sub(form, fieldsWin);
  post_form(form);

  refresh();
  wrefresh(formWin);
  wrefresh(fieldsWin);

  driver(form, fields);

  return form;
}

void refreshBoard(){
  refresh();
  wrefresh(boardWin);
  wrefresh(todoBoard);
  wrefresh(doingBoard);
  wrefresh(doneBoard);
  wrefresh(todo);
  wrefresh(doing);
  wrefresh(done);
}

void renderPerson(char* name) {
  mvprintw(0, 1, "Please press \"R\" to return.");

  int h, w, bh, bw;
  h = getmaxy(stdscr) - 3;
  w = getmaxx(stdscr) / 3;

  // Portion of screen occupied by the board
  boardWin = newwin(getmaxy(stdscr) - 3, getmaxx(stdscr), 2, 0);

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
  title(todoBoard, "| TODO (no one assigned) |");
  printList(todo, boardList->todo, 1);

  // Print doing
  title(doingBoard, "| DOING |");
  printListByPerson(doing, boardList->doing, name);

  // Print done
  title(doneBoard, "| DONE |");
  printListByPerson(done, boardList->done, name);

  refreshBoard();
  wrefresh(stdscr);
}

void renderAll() {

  int h, w, bh, bw;
  h = getmaxy(stdscr) - 5;
  w = getmaxx(stdscr);

  // Portion of screen occupied by the board
  boardWin = newwin(getmaxy(stdscr) - 2, getmaxx(stdscr), 2, 0);

  allBoard = derwin(boardWin, h, w, 0, 0);
  box(allBoard, 0, 0);


  bh = (getmaxy(allBoard)) - 4;
  bw = (getmaxx(allBoard)) - 4;
  all = derwin(boardWin, bh, bw, 2, 2);

  // Print todo
  title(allBoard, "| ALL |");
  printList(all, boardList->all, 4);

  wrefresh(boardWin);
  wrefresh(allBoard);
  wrefresh(all);
}

void initBoard(int option) {
  if (option == 1) {
    renderBoard();
  } else if (option == -1) {
    renderAll();
  }
}

//////////////////////////////////////////////////////

/* MAIN UI LOOP */

void unpost(FORM *form, FIELD **fields) {
  unpost_form(form);
  free_form(form);
  for (int i = 0; fields[i] == NULL; ++i) {
    free_field(fields[i]);
  }
}

void addChoice() {
  FIELD_INFO options[5] = {
      {"prompt", "Priority [1-10]:", 0},
      {"input_int", NULL, 1},
      {"prompt", "Description:", 2},
      {"input_str", NULL, 3},
      {"button", "[ OK ]", 4}
  };

  FORM *form = renderForm(options, 5);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    addChoice();
  }

  // Reading priority
  char *priorityBuffer = field_buffer(form->field[1], 0);
  trimWhitespaces(priorityBuffer);
  int priorityInt;
  readInt(&priorityInt, priorityBuffer);

  // Reading description
  char *descriptionBuffer = field_buffer(form->field[3], 0);
  trimWhitespaces(descriptionBuffer);

  // Adding task to list
  addTask(priorityInt, descriptionBuffer);

  unpost(form, form->field);
  choiceLoop();
}

void startChoice() {
  struct field_info options[7] = {
      {"prompt", "Task ID:", 0},
      {"input_int", NULL, 1},
      {"prompt", "Assign to:", 2},
      {"input_str", NULL, 3},
      {"prompt", "Deadline (dd/mm/year):", 4},
      {"input_date", NULL, 5},
      {"button", "[ OK ]", 6}
  };

  FORM *form = renderForm(options, 7);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    startChoice();
  }

  // Reading ID
  char *idBuffer = field_buffer(form->field[1], 0);
  trimWhitespaces(idBuffer);
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char *personBuffer = field_buffer(form->field[3], 0);
  trimWhitespaces(personBuffer);

  // Reading deadline
  char *deadlineBuffer = field_buffer(form->field[5], 0);
  trimWhitespaces(deadlineBuffer);

  int day, e1, month, e2, year, e3;
  char *dayBuffer = strtok(deadlineBuffer, "/");
  char *monthBuffer = strtok(NULL, "/");
  char *yearBuffer = strtok(NULL, "/");

  // Checking errors
  e1 = readInt(&day, dayBuffer);
  e2 = readInt(&month, monthBuffer);
  e3 = readInt(&year, yearBuffer);

  if (!(e1 || e2 || e3) || isValidDate(year, month, day) != TRUE || !listTaskExists(idInt, boardList->todo)) {
    unpost(form, form->field);
    startChoice();
  } else {
    // Adding task to list
    workOnTask(idInt, day, month - 1, year, personBuffer);
    unpost(form, form->field);
    choiceLoop();
  }
}

void closeChoice() {
  struct field_info options[3] = {
      {"prompt", "Task ID:", 0},
      {"input_int", NULL, 1},
      {"button", "[ OK ]", 2}
  };

  FORM *form = renderForm(options, 3);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    closeChoice();
  }

  // Reading ID
  char *idBuffer = field_buffer(form->field[1], 0);
  idBuffer = strtok(idBuffer, " ");
  int idInt;
  readInt(&idInt, idBuffer);

  if (!listTaskExists(idInt, boardList->doing)) {
    unpost(form, form->field);
    startChoice();
  } else {
    // Adding task to done
    closeTask(idInt);
    unpost(form, form->field);
    choiceLoop();
  }
}

void reAssign() {
  struct field_info options[5] = {
      {"prompt", "Task ID:", 0},
      {"input_int", NULL, 1},
      {"prompt", "Assign to:", 2},
      {"input_str", NULL, 3},
      {"button", "[ OK ]", 4}
  };

  FORM *form = renderForm(options, 5);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    reAssign();
  }

  // Reading ID
  char *idBuffer = field_buffer(form->field[1], 0);
  trimWhitespaces(idBuffer);
  int idInt;
  readInt(&idInt, idBuffer);

  // Reading assigned worker
  char *personBuffer = field_buffer(form->field[3], 0);
  trimWhitespaces(personBuffer);

  if (!listTaskExists(idInt, boardList->doing)) {
    unpost(form, form->field);
    reAssign();
  } else {
    // Reassign task
    reassignTask(idInt, personBuffer);
    unpost(form, form->field);
    choiceLoop();
  }
}

void reOpen() {
  struct field_info options[3] = {
      {"prompt", "Task ID:", 0},
      {"input_int", NULL, 1},
      {"button", "[ OK ]", 2}
  };

  FORM *form = renderForm(options, 3);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    reAssign();
  }

  // Reading ID
  char *idBuffer = field_buffer(form->field[1], 0);
  trimWhitespaces(idBuffer);
  int idInt;
  readInt(&idInt, idBuffer);

  if (!listTaskExists(idInt, boardList->done)) {
    unpost(form, form->field);
    reOpen();
  } else {
    // Reopen task
    reopenTask(idInt);
    unpost(form, form->field);
    choiceLoop();
  }
}

void personTasksChoice() {
  FIELD_INFO options[3] = {
      {"prompt", "Person to query:", 0},
      {"input_str", NULL, 1},
      {"button", "[ OK ]", 2}
  };

  FORM *form = renderForm(options, 3);

  if (form_driver(form, REQ_VALIDATION) != 0) {
    personTasksChoice();
  }

  // Reading description
  char *personBuffer = field_buffer(form->field[1], 0);
  trimWhitespaces(personBuffer);
  unpost(form, form->field);

  int c;
  renderPerson(personBuffer);
  while (1) {
    c = wgetch(menuWin);
    if (c == KEY_LEFT || c == KEY_RIGHT || c == 'r')
      break;
  }

  nuke();
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

  case 6:personTasksChoice();
    break;

  default: saveTasks(boardList->todo, boardList->doing, boardList->done);
    break;
  }
}

void choiceLoop() {
  int highlight = 1;
  int choice = 0;
  int c;

  maxx = getmaxx(stdscr);
  maxy = getmaxy(stdscr);

  nuke();

  initBoard(currentBoard);
  renderMenu(highlight);
  mvprintw(0, 1, "[F1: Save] [F2: Save/Quit] Use arrows to select option, \"s\" to switch board view.");

  // Press right highlight and select next option, left is the opposite
  while (1) {
    if (is_term_resized(maxy, maxx)) {
      nuke();

      maxx = getmaxx(stdscr);
      maxy = getmaxy(stdscr);

      initBoard(currentBoard);
      renderMenu(highlight);
      mvprintw(0, 1, "[F1: Save] [F2: Save/Quit] Use arrows to select option, \"s\" to switch board view.");
    }

    c = wgetch(menuWin);

    switch (c) {

    case 'r':
      nuke();
      initBoard(currentBoard);
      mvprintw(0, 1, "[F1: Save] [F2: Save/Quit] Use arrows to select option, \"s\" to switch board view.");
      wrefresh(stdscr);
      break;

    case 's':
      currentBoard *= -1;
      initBoard(currentBoard);
      break;

    case KEY_F(1):
      saveTasks(boardList->todo, boardList->doing, boardList->done);
      break;

    case KEY_F(2):
      saveTasks(boardList->todo, boardList->doing, boardList->done);
      return;

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
  wclear(menuWin);

  int x, y, i, pad;

  // Upper left corner of menu
  int startx = 0;
  int starty = maxy - 3;

  // Menu dimensions
  int width = maxx;
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
  h = getmaxy(stdscr) - 5;
  w = getmaxx(stdscr) / 3;

  // Portion of screen occupied by the board
  boardWin = newwin(getmaxy(stdscr) - 3, getmaxx(stdscr), 2, 0);

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
  scrollok(todo, TRUE);

  bh = (getmaxy(doingBoard)) - 4;
  bw = (getmaxx(doingBoard)) - 4;
  doing = derwin(doingBoard, bh, bw, 2, 2);
  scrollok(doing, TRUE);

  bh = (getmaxy(doneBoard)) - 4;
  bw = (getmaxx(doneBoard)) - 4;
  done = derwin(doneBoard, bh, bw, 2, 2);
  scrollok(done, TRUE);

  // Print todo
  title(todoBoard, "| TODO |");
  printList(todo, boardList->todo, 1);

  // Print doing
  title(doingBoard, "| DOING |");
  printList(doing, boardList->doing, 2);

  // Print done
  title(doneBoard, "| DONE |");
  printList(done, boardList->done, 3);

  refreshBoard();
}

void render(board_t *board_init) {
  currentBoard = 1;
  initCore(board_init);
  boardList = board_init;

  initscr();
  clear();
  noecho();
  keypad(stdscr, TRUE);

  choiceLoop();
  nuke();

  clrtoeol();
  refresh();
  endwin();
}

//////////////////////////////////////////////////////
