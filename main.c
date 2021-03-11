#include <stdio.h>
#include <string.h>

void printMenu(char* title, char* options[], int count, int ident){
	for(int i = 0; i<strlen(title)+ident; i++){ printf("-");}printf("\n");
	for(int i = 0; i<count; i++){
		printf("%d) ", i+1);
		puts(options[i]);
	}

}

int main(){
	char title[] = "KanBan";
	char* options[] = {"Add Task", "Move to 'Doing'", "Change person", "Close task", "Reopen task", "View tasks by person", "View tasks by date", "Quit"};
	while(1){

		// Manter o quadro sempre no do ecra
		//printBoard();
		printMenu(title, options, 8, 8);
		printf("> ");
		int op = 0;
		// Implementar safe read (sugestao: fgets + atoi)
		scanf("%d\n", &op);

		switch(op){
			case 1:
				// add tasks
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
				return 0;
				break;

			default:
				puts("Invalid Options");

		}

	}

	return 0;
}
