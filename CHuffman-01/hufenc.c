#include <string.h>
#include <limits.h>
#include "pqueue.h"

#define DEBUG 0
#define TABLE_MAX 257
#define BITS_MAX 256
#define FREQ_MAX 256



void clearTable(char*** table) {
	for(int i=0;i<TABLE_MAX;i++) {
		(*table)[i]=NULL;
	}
}



void BT_Walk(PQ_Node** head, char ***table,int space,char bit) {

	static char bits[BITS_MAX]={0};

    if((*head)==NULL) {
        return;
	}

if(DEBUG)	for(int i=0;i<space+1;i++) printf("\t");

	if(bit!='X') {

	    bits[space]=bit;
		bits[space+1]='\0';

		if((*head)->data==-1) {
if(DEBUG)			printf("OO->%d->%c\n",(*head)->priority,bit);
		} else if((*head)->data==256) {
if(DEBUG)			printf("XX->%d->%s\n",(*head)->priority,bits);
			(*table)[(*head)->data]=strdup(bits);
		} else {
if(DEBUG)			printf("%02X->%d->%s\n",(*head)->data,(*head)->priority,bits);
			(*table)[(*head)->data]=strdup(bits);
		}
	} else {
		bits[0]='\0';
if(DEBUG)		printf("OO->%d->%c\n",(*head)->priority,bit);
	}

	BT_Walk(&((*head)->left),table,space+1,'0');
	BT_Walk(&((*head)->right),table,space+1,'1');

}


void BT_WalkX(PQ_Node** head, char ***table) {
if(DEBUG)	printf("--- BT_WalkX ---\n");
	clearTable(table);
	BT_Walk(head,table,-1,'X');
}



void BT_PrintTable(char*** table) {
if(DEBUG)	printf("--- BT_PrintTable ---\n");
	for(int i=0;i<TABLE_MAX;i++) {
		if((*table)[i]) {
	 		if(i==256) {
if(DEBUG) 		printf("XX->%s\n",(*table)[i]);
			} else {
if(DEBUG)		printf("%02X->%s\n",i,(*table)[i]);
			}
		}
	}
}



void encode(char ***table,char *filename) {

if(DEBUG)	printf("--- encode ---\n");

	FILE *fin=NULL;
	FILE *fout=NULL;

	fin=fopen(filename,"rb");

	if(!fin) {
		printf("cannot open file %s\n",filename);
		return;
	}

	char outfile[PATH_MAX]={0};
	strcpy(outfile,filename);
	strcat(outfile,".enc");

	fout=fopen(outfile,"wb");

	int ch='\0';
	int b=0;
	int j=0;
	while((ch=fgetc(fin))!=EOF) {
		for(int i=0;i<strlen((*table)[ch]);i++) {
			b|=((*table)[ch][i]=='0'?0:1)<<(7-j);
			j++;
			if(j==8) {
				fputc(b,fout);
if(DEBUG)		printf("%02X ",b);
				b=0;
				j=0;
			}
		}
	}
	ch=256;
	for(int i=0;i<strlen((*table)[ch]);i++) {
		b|=((*table)[ch][i]=='0'?0:1)<<(7-j);
		j++;
		if(j==8) {
if(DEBUG)	printf("%02X ",b);
			fputc(b,fout);
			b=0;
			j=0;
		}
	}
	if(b!=0) {
		fputc(b,fout);
if(DEBUG)	printf("%02X ",b);
	}

if(DEBUG)	printf("\n");

	fclose(fin);
	fclose(fout);

}

void saveTable(PQ_Node** head,FILE *fout) {
	if((*head)==NULL) return;

	if(((*head)->left==NULL) && ((*head)->right==NULL)) {
		fprintf(fout,"00\n");
		if((*head)->data==FREQ_MAX && (*head)->priority==0) {
			fprintf(fout,"XX\n");
		} else {
			fprintf(fout,"%02X\n",(*head)->data);
		}
	} else {
		fprintf(fout,"01\n");
	}
	saveTable(&((*head)->left),fout);
	saveTable(&((*head)->right),fout);
}



void saveTableX(PQ_Node** head,char *filename) {
if(DEBUG)	printf("--- saveTableX ---\n");
	char outfile[PATH_MAX];
	strcpy(outfile,filename);
	strcat(outfile,".tab");
	FILE *fout=fopen(outfile,"wb");
	saveTable(head,fout);
	fclose(fout);
}


PQ_Node* createHuffmanTree(char *filename) {

if(DEBUG)	printf("--- createHuffmanTree ---\n");


    PQ_Node *pq=NULL;
	FILE *fin=NULL;
	int ch='\0';
	int freq[FREQ_MAX]={0};

	fin=fopen(filename,"rb");

	if(!fin) {
		printf("cannot open file %s.\n",filename);
		return NULL;
	}

	while((ch=fgetc(fin))!=EOF) {
		freq[ch]++;
	}

	fclose(fin);

	for(int i=0;i<FREQ_MAX;i++) {
		if(freq[i]) PQ_Push(&pq,i,freq[i],NULL,NULL);
	}
	PQ_Push(&pq,FREQ_MAX,0,NULL,NULL);

	while(PQ_Length(&pq)>=2) {
		PQ_Node *l=PQ_Pop(&pq);
		PQ_Node *r=PQ_Pop(&pq);
		PQ_Push(&pq,-1,l->priority+r->priority,l,r);
	}

	return pq;
}

int main(int argc,char **argv) {

	char **table=malloc(sizeof(*table)*TABLE_MAX);
	PQ_Node *pq=NULL;

	pq=createHuffmanTree(argv[1]);

	BT_WalkX(&pq,&table);

	saveTableX(&pq,argv[1]);

	BT_PrintTable(&table);

	encode(&table,argv[1]);

    return EXIT_SUCCESS;
}
