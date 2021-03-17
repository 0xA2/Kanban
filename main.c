#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "card.h"

int readOption(int* op){
	long toInt;
	char buffer[1024];

	// Exit if something goes wrong reading input
	if(fgets(buffer, sizeof(buffer), stdin) == NULL){ printf("\n"); exit(1);}

	// Convert string input into long
	char* ptr; errno = 0;
	toInt = strtol(buffer, &ptr, 10);


	// Check errors

	if(errno == ERANGE){ return 0; }

	if(ptr == buffer){ return 0; }

	if(*ptr && *ptr != '\n'){ return 0; }

	if(toInt > INT_MAX || toInt < INT_MIN){ return 0; }

	*op = (int)toInt;
	return 1;
}


void printMenu(char* title, char* options[], int count, int ident){
	//system("clear");
	for(int i = 0; i<strlen(title)+ident; i++){ printf("-");}printf("\n");
	for(int i = 0; i<count; i++){
		printf("%d) ", i+1);
		puts(options[i]);
	}
}


int main(){
	char title[] = "KanBan";
	char* options[] = {"Add Task", "Move to 'Doing'", "Change person", "Close task", "Reopen task", "View tasks by person", "View tasks by date", "Exit"};
	while(1){

		// Manter o quadro sempre no do ecra
		//printBoard();
		printMenu(title, options, 8, 8);
		printf("> ");
		int op = 0; readOption(&op);
		switch(op){
			case 1:
				// add task
				break;

			case 2:
				// move to doing
				break;

			case 3:
				// change person
				break;

			case 4:
				// close task
				break;

			case 5:
				// reopen
				break;

			case 6:
				// view tasks by person
				break;

			case 7:
				// View tasks by date
				break;

			case 8:
				exit(0);
				break;

			default:
				puts("Invalid Option");
		}

	}
	return 0;
}

