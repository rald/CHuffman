#include <string.h>
#include <limits.h>
#include "pqueue.h"

#define BITS_MAX 256

char *code[256];


void BT_Walk(PQ_Node** head, int space,char bit) {

	static char bits[BITS_MAX]={0};

    if((*head)==NULL) {
        return;
	}

	for(int i=0;i<space;i++) printf("\t");

	if(bit!='X') {

	    bits[space++]=bit;
		bits[space]='\0';

		if((*head)->data!=-1) {
			printf("%d->%d->%s\n",(*head)->data,(*head)->priority,bits);
			code[(*head)->data]=strdup(bits);
		} else {
			printf("%d->%d->%c\n",(*head)->data,(*head)->priority,bit);
		}

	} else {
		bits[0]='\0';
		printf("%d->%d->%c\n",(*head)->data,(*head)->priority,bit);
	}

	BT_Walk(&((*head)->left),space,'0');
	BT_Walk(&((*head)->right),space,'1');

}



void encode(char *filename) {

	printf("--- ENCODE ---\n");

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
		for(int i=0;i<strlen(code[ch]);i++) {
			b|=(code[ch][i]=='0'?0:1)<<(7-j);
			j++;
			if(j==8) {
				fputc(b,fout);
				printf("%02X ",b);
				b=0;
				j=0;
			}
		}
	}
	ch=256;
	for(int i=0;i<strlen(code[ch]);i++) {
		b|=(code[ch][i]=='0'?0:1)<<(7-j);
		j++;
		if(j==8) {
			printf("%02X ",b);
			fputc(b,fout);
			b=0;
			j=0;
		}
	}
	if(b!=0) {
		fputc(b,fout);
		printf("%02X ",b);
	}

	printf("\n");

	fclose(fin);
	fclose(fout);

}



void decode(char *filename) {

	printf("--- DECODE ---\n");

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
				if((code[l]!=NULL) && (!strcmp(b,code[l]))) {
					k=l;
					break;
				}
			}
			if(k==256) {
				printf("%d->%s\n",k,b);
				goto terminate;
			} else if(k!=-1) {
				printf("%d->%s\n",k,b);
				fputc(k,fout);
				i=0;
				b[0]='\0';
			}
		}
	}
terminate:

	fclose(fin);
	fclose(fout);
}



int main(int argc,char **argv) {

	FILE *fin=NULL;

	int ch='\0';
	int f[256]={0};
    PQ_Node *pq=NULL;

	fin=fopen(argv[1],"rb");

	if(!fin) {
		printf("cannot open file %s.\n",argv[1]);
		return EXIT_FAILURE;
	}

	while((ch=fgetc(fin))!=EOF) {
		f[ch]++;
	}

	fclose(fin);

	for(int i=0;i<256;i++) {
		if(f[i]) PQ_Push(&pq,i,f[i],NULL,NULL);
	}
	PQ_Push(&pq,256,0,NULL,NULL);

	while(PQ_Length(&pq)>=2) {
		PQ_Node *l=PQ_Pop(&pq);
		PQ_Node *r=PQ_Pop(&pq);
		PQ_Push(&pq,-1,l->priority+r->priority,l,r);
	}

	for(int i=-1;i<257;i++) code[i]=NULL;


	BT_Walk(&pq,0,'X');

	for(int i=0;i<257;i++) if(code[i]) printf("%d->%s\n",i,code[i]);

	encode(argv[1]);

	decode(argv[1]);

    return EXIT_SUCCESS;
}
