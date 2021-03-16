// New type for list of integers

typedef struct lnode {
	int val;
	struct lnode* next;
} node;

typedef struct {
	int size;
	node* first;
} list;

// List API

node* nodeNew(int, node*);
list* listNew();
int listIsEmpty(list*);
void listDestroy(list*);
void listAddFirst(int, list*);
void listAddLast(int, list*);
int listGetFirst(list*);
int listGetLast(list*);
void listRemoveFirst(list*);
void listRemoveLast(list*);
int listValueAtIndex(int, list*);
void listRemoveAtIndex(int, list*);
void listAddAtIndex(int,int,list*);
void listChangeAtIndex(int,int,list*);
int listSize(list*);
int listContains(int,list*);
void listPrint(list*);
