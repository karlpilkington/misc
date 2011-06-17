#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
 
/*
 * Generates a stream of bytes having a specified probability distribution.
 * The bytes will be randomly chosen from the alphabet (a,b,c,...).
 */

void usage(char *prog) {
  fprintf(stderr, "usage: %s prob1 [prob2 ...]\n", prog);
  fprintf(stderr, "e.g.,  %s 20 80\n", prog);
  fprintf(stderr, "       %s 10 80 10\n", prog);
  fprintf(stderr, "Probabilities must add to 100\n", prog);
  exit(-1);
}

char out[100];
 
int main(int argc, char * argv[]) {
  unsigned i,w,pos=0,left=100;
  char symbol = 'a';

  if (argc < 2) usage(argv[0]);
  for(i=1; i<argc; i++) {
    if (sscanf(argv[i],"%u",&w) != 1) usage(argv[0]);
    if (w > left) usage(argv[0]);
    memset(&out[pos],symbol,w);
    pos += w; left -= w;
    symbol++;
  }
  if (left) usage(argv[0]);
  printf("%.*s\n",100,out);
}
