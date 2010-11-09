#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "tconf.h"

static const unsigned char ws[256] = {[' ']=1,['\t']=1};
static const unsigned char nl[256] = {['\r']=1,['\n']=1};

int tconf(char *file, tconf_t *tconf, int tclen, int opt) {
  
  int rc = -1,klen,vlen;
  char line[200],*k,*v;
  FILE *f=NULL;
  tconf_t *t;

  if ( (f = fopen(file,"r")) == NULL) {
    fprintf(stderr,"can't open %s: %s\n", file, strerror(errno));
    rc = -1;
    goto done;
  }

  while (fgets(line,sizeof(line),f) != NULL) {
    k=line; while(ws[*k]) k++;                           /* trim space */
    if ((*k == '#') || nl[*k] || *k == '\0') continue;   /* skip comments */
    v=k; while(!(ws[*v] || nl[*v] || *v=='\0')) v++;     /* find k/v delim */
    klen = v-k;
    if (*v != '\0') v++;
    vlen = 0; while (!(nl[*v] || (*v=='\0'))) vlen++;

    for(t = tconf; t < tconf+tclen; t++) {
      //if (t->name
    }
  }


  rc = 0; /* success */

 done:
  if (f) fclose(f);
  return rc;
}
