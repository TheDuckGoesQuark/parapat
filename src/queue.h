#define MAX_TASKS 100
#define MAX_THREADS 16

typedef int Value;

typedef struct Node *Node;

struct Node {
  Value value;
  Node next;
};


typedef struct NodePtr *NodePtr;

struct NodePtr {
  int count;
  Node p;
};

typedef struct Queue *Queue;

struct Queue {
  Node first;
};

typedef Queue tq;

tq newtq();
int gettask(tq tq);
void puttask(tq tq,int v);
void createfarm(void (*Worker)(),int n);
void createpipe(void (*Worker1)(),void (*Worker2)());
