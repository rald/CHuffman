#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>

#include "pqueue.h"

#define TABLE_MAX 257
#define BITS_MAX 256
#define FREQ_MAX 256
#define BUFFER_MAX 16384


unsigned char buf1[BUFFER_MAX];
unsigned char buf2[BUFFER_MAX];



long getFileSize(char* filename) {
	struct stat statbuf;

	if(stat(filename,&statbuf)==-1) {
        	printf("failed to stat %s.\n",filename);
					return -1;
	}

	return statbuf.st_size;
}


unsigned long* countCharFreq(char *filename) {

	unsigned long* freq=(unsigned long*)calloc(FREQ_MAX,sizeof(*freq));
	FILE *fin;
	unsigned char buf[BUFFER_MAX];
	long gt=0;
	long sz=0;
	int max=0,i=0;
	struct stat statbuf;

	printf("--- countCharFreq ---\n");


	sz=getFileSize(filename);
	if(sz==-1) {
		printf("cannot get size of file: %s\n",filename);
		return NULL;
	}


	fin=fopen(filename,"rb");

	if(!fin) {
		printf("cannot open file %s.\n",filename);
		return NULL;
	}

	setvbuf(fin,NULL,_IOFBF,BUFFER_MAX);

	for(i=0;i<256;i++) freq[i]=0;

	gt=0;
	while(gt<sz) {

		max=(int)(sz-gt>=BUFFER_MAX?BUFFER_MAX:(unsigned long)sz-gt);

		memset(buf,0,BUFFER_MAX);

		if(fread(buf,max,1,fin)!=1) {
			printf("error reading file %s.\n",filename);
			return NULL;
		}

		gt+=max;

		printf("%10d %10lu / %10ld == %6.2f%%\n",max,gt,sz,(float)gt/sz*100.0);

		for(i=0;i<max;i++) {
			freq[buf[i]]++;
		}

	}

	fclose(fin);

	return freq;
}


void printCharFreq(unsigned long *freq) {
	unsigned long	tf=0;
	int i;
	printf("--- printCharFreq --\n");
	for(i=0;i<256;i++) {
		printf("%02X %c %10lu\n",i,isalnum((unsigned char)i)||ispunct((unsigned char)i)?i:' ',freq[i]);
		tf+=freq[i];
	}
	printf("total: %lu\n",tf);

}



void clearTable(char*** table) {
	int i;
	for(i=0;i<TABLE_MAX;i++) {
		(*table)[i]=NULL;
	}
}


void createTableX(PQ_Node** head, char ***table,int space,char bit) {

	static char bits[BITS_MAX]={0};
	int i;

  if((*head)==NULL) {
    return;
	}

	for(i=0;i<space+1;i++) printf("\t");

	if(bit!='X') {

	  bits[space]=bit;
		bits[space+1]='\0';

		if((*head)->data==-1) {
			printf("OO->%ld->%c\n",(*head)->priority,bit);
		} else if((*head)->data==256) {
      printf("XX->%ld->%s\n",(*head)->priority,bits);
			(*table)[(*head)->data]=strdup(bits);
		} else {
			printf("%02X->%ld->%s\n",(*head)->data,(*head)->priority,bits);
			(*table)[(*head)->data]=strdup(bits);
		}
	} else {
		bits[0]='\0';
		printf("OO->%ld->%c\n",(*head)->priority,bit);
	}

	createTableX(&((*head)->left),table,space+1,'0');
	createTableX(&((*head)->right),table,space+1,'1');

}


void createTable(PQ_Node** head, char ***table) {
  printf("--- createTable ---\n");
	clearTable(table);
	createTableX(head,table,-1,'X');
}



void printTable(char*** table) {
	int i;
  printf("--- printTable ---\n");
	for(i=0;i<TABLE_MAX;i++) {
		if((*table)[i]) {
	 		if(i==256) {
        printf("XX->%s\n",(*table)[i]);
			} else {
    		printf("%02X->%s\n",i,(*table)[i]);
			}
		}
	}
}


void encode(char*** table,char* filename) {

	FILE *fin=NULL;
	FILE *fout=NULL;


	int ch='\0';
	unsigned char b=0;
	long i=0,j=0;
	unsigned long k=0,gt=0,max1=0,max2=0;

	char outfile[PATH_MAX]={0};

	long sz=getFileSize(filename);

	char *p=NULL;

	printf("--- encode ---\n");


	fin=fopen(filename,"rb");

	if(!fin) {
		printf("cannot open file %s\n",filename);
		return;
	}

	setvbuf(fin,NULL,_IOFBF,BUFFER_MAX);

	strcpy(outfile,filename);
	p=strrchr(outfile,'.');
	if(p) *p='\0';
	strcat(outfile,".enc");

	fout=fopen(outfile,"wb");

	max1=0;
	gt=0;
	j=0;
	b=0;
	memset(buf1,0,BUFFER_MAX);
	while(gt<sz) {
		max2=sz-gt>=BUFFER_MAX?BUFFER_MAX:sz-gt;
		memset(buf2,0,BUFFER_MAX);
		if(fread(buf2,max2,1,fin)!=1) {
      printf("error reading file %s.\n",filename);
      return;
		}
		gt+=max2;
		printf("%10lu %10lu / %10lu %6.2f%%\n",max2,gt,sz,(float)gt/sz*100.0f);
		for(k=0;k<max2;k++) {
			ch=buf2[k];
			for(i=0;i<strlen((*table)[ch]);i++) {
				b|=((*table)[ch][i]=='0'?0:1)<<(7-j);
				j++;
				if(j==8) {
					buf1[max1++]=b;
					if(max1==BUFFER_MAX) {
						if(fwrite(buf1,max1,1,fout)!=1) {
							printf("error writing file %s.\n",filename);
              return;
						}
          	memset(buf1,0,BUFFER_MAX);
						max1=0;
					}
					b=0;
					j=0;
				}
			}
		}
	}

	if(fwrite(buf1,max1,1,fout)!=1) {
    printf("error writing file %s.\n",filename);
    return;
	}
	memset(buf1,0,BUFFER_MAX);
	max1=0;

	ch=256;
	for(i=0;i<strlen((*table)[ch]);i++) {
		b|=((*table)[ch][i]=='0'?0:1)<<(7-j);
		j++;
		if(j==8) {
			buf1[max1++]=b;
			if(max1==BUFFER_MAX) {
				if(fwrite(buf1,max1,1,fout)!=1) {
          printf("error writing file %s.\n",filename);
          return;
				}
	      memset(buf1,0,BUFFER_MAX);
				max1=0;
			}
			b=0;
			j=0;
		}
	}
	if(max1!=0) {
		if(fwrite(buf1,max1,1,fout)!=1) {
      printf("error writing file %s.\n",filename);
      return;
		}
  	memset(buf1,0,BUFFER_MAX);
		max1=0;
	}
	if(b!=0) {
		fputc(b,fout);
	}


	fclose(fout);
	fclose(fin);

}



void saveTableX(PQ_Node** head,FILE* fout) {
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
	saveTableX(&((*head)->left),fout);
	saveTableX(&((*head)->right),fout);
}



void saveTable(PQ_Node** head,char* filename) {
	char outfile[PATH_MAX];
	FILE *fout=NULL;
	char *p=NULL;

  printf("--- saveTable ---\n");

	strcpy(outfile,filename);
	p=strrchr(outfile,'.');
	if(p) *p='\0';
	strcat(outfile,".tab");
	fout=fopen(outfile,"wb");
	saveTableX(head,fout);
	fclose(fout);
}



PQ_Node* createHuffmanTree(char* filename) {
	PQ_Node *pq=NULL;

	unsigned long *freq=NULL;
	FILE *fin;
	int i;
	printf("--- createHuffmanTree ---\n");

	freq=countCharFreq(filename);

	printCharFreq(freq);

	for(i=0;i<FREQ_MAX;i++) {
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

	char** table=(char**)malloc(sizeof(*table)*TABLE_MAX);
	PQ_Node* pq=NULL;

	if(argc!=2) {
        	printf("Syntax: %s filepath\n",argv[0]);
        	return 1;
	}

	pq=createHuffmanTree(argv[1]);

 	createTable(&pq,&table);

	saveTable(&pq,argv[1]);

	printTable(&table);

	encode(&table,argv[1]);

	return 0;
}

