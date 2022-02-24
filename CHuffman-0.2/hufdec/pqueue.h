#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct PQ_Node PQ_Node;

struct PQ_Node {
	int data;
	long priority;
	PQ_Node *next;
	PQ_Node *left,*right;
};

PQ_Node *PQ_NewNode(int d,long p,PQ_Node* left,PQ_Node* right);
PQ_Node* PQ_Peek(PQ_Node** head);
PQ_Node* PQ_Pop(PQ_Node** head);
void PQ_Push(PQ_Node** head,int d,long p,PQ_Node* left,PQ_Node* right);
bool PQ_IsEmpty(PQ_Node** head);
int PQ_Length(PQ_Node** head);
void PQ_Print(PQ_Node** head);

#endif
