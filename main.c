#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "card.h"

void printMenu (char *title, char *options[], int count, int ident)
{
  for (int i = 0; i < strlen (title) + ident; i++)
    {
      printf ("-");
    }
  printf ("\n");
  for (int i = 0; i < count; i++)
    {
      printf ("%d) ", i + 1);
      puts (options[i]);
    }
}

int main ()
{
  // Manter o quadro sempre no do ecra com uma função tipo printBoard();
  char title[] = "KanBan";
  char *options[] = {"Add Task", "Move to 'Doing'", "Change person", "Close task", "Reopen task",
                     "View tasks by person", "View tasks by date", "Quit"};
  int op = 0;

  printMenu (title, options, 8, 8);
  printf ("> ");

  Card *c = malloc (sizeof (Card));

  // Implementar safe read (sugestao: fgets + atoi)
  scanf ("%d\n", &op);

  switch (op)
    {
      case 1:
        // add tasks
        fillCard (c, "antonio costa", "obliterar covid");
      printCard (c);
      break;
      default:
        puts ("Invalid Options");

    }
  return 0;
}
