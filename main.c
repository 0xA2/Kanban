#include <locale.h>
#include "./utils/render.h"

int main() {
  // to use wide chars in ncurses
  setlocale(LC_ALL, "");

  tasklist *all = listNew();
  tasklist *todo = loadToDo(all);
  tasklist *doing = loadDoing(all);
  tasklist *done = loadDone(all);
  board_t board = {
      all,
      todo,
      doing,
      done
  };

  render(&board);

  return EXIT_SUCCESS;
}

