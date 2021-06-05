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



void decode(char ***table,char *filename) {

if(DEBUG)	printf("--- decode ---\n");

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

if(DEBUG)	printf("--- loadTableX ---\n");

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

	char **table=loadTableX(argv[1]);

	BT_PrintTable(&table);

	decode(&table,argv[1]);

    return EXIT_SUCCESS;
}
