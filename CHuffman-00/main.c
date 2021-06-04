#include <string.h>
#include <limits.h>
#include "pqueue.h"

#define DEBUG 0
#define TABLE_MAX 257
#define BITS_MAX 256
#define FREQ_MAX 256



void BT_Walk(PQ_Node** head, char ***table,int space,char bit) {

	static char bits[BITS_MAX]={0};

    if((*head)==NULL) {
        return;
	}

	for(int i=0;i<space+1;i++) printf("\t");

	if(bit!='X') {

	    bits[space]=bit;
		bits[space+1]='\0';

		if((*head)->data!=-1) {
			printf("%d->%d->%s\n",(*head)->data,(*head)->priority,bits);
			(*table)[(*head)->data]=strdup(bits);
		} else {
			printf("%d->%d->%c\n",(*head)->data,(*head)->priority,bit);
		}

	} else {
		bits[0]='\0';
		printf("%d->%d->%c\n",(*head)->data,(*head)->priority,bit);
	}

	BT_Walk(&((*head)->left),table,space+1,'0');
	BT_Walk(&((*head)->right),table,space+1,'1');

}



void encode(char ***table,char *filename) {

if(DEBUG)	printf("--- ENCODE ---\n");

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

	printf("\n");

	fclose(fin);
	fclose(fout);

}



void decode(char ***table,char *filename) {

if(DEBUG)	printf("--- DECODE ---\n");

	FILE *fin=NULL;
	FILE *fout=NULL;

	char infile[PATH_MAX]={0};
	strcpy(infile,filename);
	strcat(infile,".enc");

	fin=fopen(infile,"rb");

	if(!fin) {
		printf("cannot open file %s\n",infile);
		return;
	}

	char outfile[PATH_MAX]={0};
	strcpy(outfile,filename);
	strcat(outfile,".dec");

	fout=fopen(outfile,"wb");

	int ch='\0';
	int i=0;
	char b[256]={0};
	while((ch=fgetc(fin))!=EOF) {
		for(int j=0;j<8;j++) {
			b[i]=(((ch&(1<<(7-j)))==0)?'0':'1');
			b[i+1]='\0';
			i++;
			int k=-1;
			for(int l=0;l<257;l++) {
				if(((*table)[l]!=NULL) && (!strcmp(b,(*table)[l]))) {
					k=l;
					break;
				}
			}
			if(k==256) {
if(DEBUG)		printf("%d->%s\n",k,b);
				goto terminate;
			} else if(k!=-1) {
				fputc(k,fout);
if(DEBUG)		printf("%d->%s\n",k,b);
				i=0;
				b[0]='\0';
			}
		}
	}
terminate:

	fclose(fin);
	fclose(fout);
}



void saveTable(PQ_Node** head,FILE *fout) {
	if((*head)==NULL) return;

	if(((*head)->left==NULL) && ((*head)->right==NULL)) {
		fprintf(fout,"0 ");
		if((*head)->data==FREQ_MAX && (*head)->priority==0) {
			fprintf(fout,"XX ");
		} else {
			fprintf(fout,"%02X ",(*head)->data);
		}
	} else {
		fprintf(fout,"1 ");
	}
	saveTable(&((*head)->left),fout);
	saveTable(&((*head)->right),fout);
}

void saveTableX(PQ_Node** head,char *filename) {
	char outfile[PATH_MAX];
	strcpy(outfile,filename);
	strcat(outfile,".tab");
	FILE *fout=fopen(outfile,"wb");
	saveTable(head,fout);
	fclose(fout);
}

PQ_Node* loadTable(char *filename) {

}

int main(int argc,char **argv) {

	FILE *fin=NULL;

	int ch='\0';
	int freq[FREQ_MAX]={0};
    PQ_Node *pq=NULL;

	char **table=malloc(sizeof(*table)*TABLE_MAX);

	for(int i=0;i<TABLE_MAX;i++) {
		table[i]=NULL;
	}

	fin=fopen(argv[1],"rb");

	if(!fin) {
		printf("cannot open file %s.\n",argv[1]);
		return EXIT_FAILURE;
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

	BT_Walk(&pq,&table,-1,'X');

	for(int i=0;i<257;i++) {
		if(table[i]) printf("%d->%s\n",i,table[i]);
	}

	saveTableX(&pq,argv[1]);

	encode(&table,argv[1]);

	decode(&table,argv[1]);

    return EXIT_SUCCESS;
}

