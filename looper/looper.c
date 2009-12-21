#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
 
int n=16;
int verbose=0;
char *ofile=NULL;
char *ifile=NULL;
 
void usage(char *prog) {
  fprintf(stderr, "usage: %s [-n <num>] [-o <outfile>] [-v]\n", prog);
  exit(-1);
}
 
int main(int argc, char * argv[]) {
  int opt;
  FILE *ifilef=stdin;
  char line[100];
 
  while ( (opt = getopt(argc, argv, "n:o:v+")) != -1) {
    switch (opt) {
      case 'n':
        n = atoi(optarg);
        break;
      case 'o':
        ofile = strdup(optarg);
        break;
      case 'v':
        verbose++;
        break;
      default:
        usage(argv[0]);
        break;
    }
  }
 
  if (optind < argc) ifile=argv[optind++];
  fprintf(stderr,"n %u, ofile %s, verbose %u\n", n, ofile?ofile:"none",verbose);
  fprintf(stderr,"ifile %s\n", ifile?ifile:"stdin");
 
  /* loop over the input */
  if (ifile) {
    if ( (ifilef = fopen(ifile,"r")) == NULL) {
      fprintf(stderr,"can't open %s: %s\n", ifile, strerror(errno));
      exit(-1);
    }
  }
 
  while (fgets(line,sizeof(line),ifilef) != NULL) {
    fprintf(stderr, "read line: %s", line);
  }
}
