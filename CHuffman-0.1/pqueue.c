#include "pqueue.h"

PQ_Node *PQ_NewNode(int d,int p,PQ_Node* left,PQ_Node* right) {
	PQ_Node* node=malloc(sizeof(*node));
	node->data=d;
	node->priority=p;
	node->left=left;
	node->right=right;
	node->next=NULL;
	return node;
}

PQ_Node* PQ_Peek(PQ_Node** head) {
	return (*head);
}

PQ_Node* PQ_Pop(PQ_Node** head) {
	PQ_Node* node=(*head);
	(*head)=(*head)->next;
	return node;
}

void PQ_Push(PQ_Node** head,int d,int p,PQ_Node* left,PQ_Node* right) {
	PQ_Node* curr=(*head);
	PQ_Node* node=PQ_NewNode(d,p,left,right);

	if((*head)==NULL) {
		(*head)=node;
	} else if((*head)->priority>p) {
		node->next=(*head);
		(*head)=node;
	} else {
		while(curr->next!=NULL && curr->next->priority<p) {
			curr=curr->next;
		}

		node->next=curr->next;
		curr->next=node;
	}
}

bool PQ_IsEmpty(PQ_Node** head) {
	return (*head)==NULL;
}

int PQ_Length(PQ_Node** head) {
	PQ_Node *curr=(*head);
	int n=0;
	while(curr!=NULL) {
		curr=curr->next;
		n++;
	}
	return n;
}

void PQ_Print(PQ_Node** head) {
	PQ_Node *curr=(*head);
	bool first=true;
	while(curr) {
		if(first) first=false; else printf(", ");
		printf("%d->%d",curr->data,curr->priority);
		curr=curr->next;
	}
	printf("\n");
}
