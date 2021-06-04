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



void BT_PrintTable(char*** table) {
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



void clearTable(char*** table) {
	for(int i=0;i<TABLE_MAX;i++) {
		(*table)[i]=NULL;
	}
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
	char outfile[PATH_MAX];
	strcpy(outfile,filename);
	strcat(outfile,".tab");
	FILE *fout=fopen(outfile,"wb");
	saveTable(head,fout);
	fclose(fout);
}



void loadTable(char ***table,FILE *fin,int depth,char bit) {
	static char bits[BITS_MAX]={0};
	char b1[3]={0};
	char b2[3]={0};
	unsigned int v=0;

	if(fscanf(fin,"%2s",b1)==1) {

		if(bit!='X') {
			bits[depth]=bit;
			bits[depth+1]='\0';
		}

		if(!strcmp(b1,"00")) {
			if(fscanf(fin,"%2s",b2)==1) {
				if(!strcmp(b2,"XX")) {
					(*table)[256]=strdup(bits);
				} else {
					sscanf(b2,"%x",&v);
					(*table)[v]=strdup(bits);
				}
			} else {
				return;
			}
		} else if(!strcmp(b1,"01")) {
			loadTable(table,fin,depth+1,'0');
			loadTable(table,fin,depth+1,'1');
		}
	}

}



char** loadTableX(char *filename) {
	char **table=malloc(sizeof(*table)*TABLE_MAX);
	FILE *fin=NULL;

	char infile[PATH_MAX];
	strcpy(infile,filename);
	strcat(infile,".tab");

	fin=fopen(infile,"rt");

	if(!fin) {
		printf("cannot open file %s.\n",infile);
		return NULL;
	}

	clearTable(&table);

	loadTable(&table,fin,-1,'X');

	fclose(fin);

	return table;
}



int main(int argc,char **argv) {

	FILE *fin=NULL;

	int ch='\0';
	int freq[FREQ_MAX]={0};
    PQ_Node *pq=NULL;

	char **table=malloc(sizeof(*table)*TABLE_MAX);

	clearTable(&table);

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

	BT_PrintTable(&table);

	saveTableX(&pq,argv[1]);

	encode(&table,argv[1]);

	char **table2=loadTableX(argv[1]);

	BT_PrintTable(&table2);

	decode(&table2,argv[1]);

    return EXIT_SUCCESS;
}
