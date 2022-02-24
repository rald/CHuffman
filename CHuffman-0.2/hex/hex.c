#include <stdio.h>

int main(int argc,char **argv) {

  int ch;
  unsigned long i=0;
  unsigned j;
  char b[17]={0};
  FILE *fin;

  if(argc!=2) {
    printf("syntax: %s src\n",argv[0]);
    return -1;
  }

  if((fin=fopen(argv[1],"rb"))==NULL) {
    printf("error: opening file: %s\n",argv[1]);
    return -1;
  }

  while((ch=fgetc(fin))!=EOF) {
    if(i%16==0) printf("%08lX: ",i);
    printf("%02X",ch);
    if((i+1)%8==0 && (i+1)%16!=0) printf("-"); else printf(" ");

    if(ch <= 32 || ch > 127) ch='.';

    b[i%16]=ch;
    b[i%16+1]=0;
    i++;
    if(i%16==0) {
      printf(" %s\n",b);
    }
  }
  if(i%16) {
    for(j=0;j<16-i%16;j++) {
      printf("   ");
    }
    printf(" %s\n",b);
  }

  fclose(fin);

  return 0;
}
