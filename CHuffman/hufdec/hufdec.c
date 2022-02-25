#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

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



void clearTable(char*** table) {
	int i;
	for(i=0;i<TABLE_MAX;i++) {
		(*table)[i]=NULL;
	}
}



void decode(char ***table,char *filename) {

	FILE *fin=NULL;
	FILE *fout=NULL;


	char infile[PATH_MAX]={0};
	char outfile[PATH_MAX]={0};
	unsigned int ch='\0';
	int i=0,j=0,k=0,l=0;
	char b[256]={0};
	long sz=0;
	unsigned long gt=0,m=0,max1=0,max2=0;


	printf("--- decode ---\n");


	strcpy(infile,filename);
	strcat(infile,".enc");


	printf("decoding file %s.\n",infile);

	if((sz=getFileSize(infile))==-1) {
		printf("error getting size of file %s.\n",infile);
		return;
	}

	fin=fopen(infile,"rb");

	if(!fin) {
		printf("cannot open file %s\n",infile);
		return;
	}


	setvbuf(fin,NULL,_IOFBF,BUFFER_MAX);

	strcpy(outfile,filename);
	strcat(outfile,".dec");

	fout=fopen(outfile,"wb");

	i=0;
	max2=0;
	while(gt<sz) {

		max1=sz-gt>=BUFFER_MAX?BUFFER_MAX:sz-gt;
		memset(buf1,0,BUFFER_MAX);
		if(fread(buf1,max1,1,fin)!=1) {
			printf("error reading file %s.\n",infile);
		}

		gt+=max1;

		printf("%10lu %10lu / %10lu == %6.2lf%%\n",max1,gt,sz,(double)gt/sz*100.0);

		for(m=0;m<max1;m++) {
			ch=buf1[m];
			for(j=0;j<8;j++) {
				b[i]=(((ch&(1<<(7-j)))==0)?'0':'1');
				b[i+1]='\0';
				i++;
				k=-1;
				for(l=0;l<257;l++) {
					if(((*table)[l]!=NULL) && (!strcmp(b,(*table)[l]))) {
						k=l;
  					break;
  				}
  			}
  			if(k==256) {
					if(max2>0) {
						if(fwrite(buf2,max2,1,fout)!=1) {
							printf("error writing file %s\n.",outfile);
							return;
						}
						memset(buf2,0,BUFFER_MAX);
						max2=0;
					}
  				goto terminate;
  			} else if(k!=-1) {
					buf2[max2++]=k;
					if(max2>=BUFFER_MAX) {
						if(fwrite(buf2,max2,1,fout)!=1) {
							printf("error writing file %s\n.",outfile);
							return;
						}
						memset(buf2,0,BUFFER_MAX);
						max2=0;
					}
  				i=0;
  				b[0]='\0';
				}
  		}
		}

		if(max2>0) {
			if(fwrite(buf2,max2,1,fout)!=1) {
				printf("error writing file %s\n.",outfile);
				return;
			}
			memset(buf2,0,BUFFER_MAX);
			max2=0;
		}
	}
terminate:

	fclose(fin);
	fclose(fout);
}



void loadTableX(char ***table,FILE *fin,int depth,char bit) {
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
			loadTableX(table,fin,depth+1,'0');
			loadTableX(table,fin,depth+1,'1');
		}
	}
}



char** loadTable(char *filename) {

	char **table=(char**)malloc(sizeof(*table)*TABLE_MAX);
	FILE *fin=NULL;
	char infile[PATH_MAX];

	printf("--- loadTable ---\n");

	strcpy(infile,filename);
	strcat(infile,".tab");

	printf("loading table file %s.\n",infile);

	fin=fopen(infile,"rt");

	if(!fin) {
		printf("cannot open file %s.\n",infile);
		return NULL;
	}

	clearTable(&table);

	loadTableX(&table,fin,-1,'X');

	fclose(fin);

	return table;
}



int main(int argc,char **argv) {

	char **table=NULL;

	if(argc!=2) {
        	printf("Syntax: %s filepath\n",argv[0]);
                return 1;
	}

	table=loadTable(argv[1]);

	decode(&table,argv[1]);

  return 0;
}

