#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "tpl.h"

char *path = "socket"; 
char prompt[100];

void usage(char *prog) {
  fprintf(stderr, "usage: %s [-v] [-s|-S socket] [-f file]\n", prog);
  fprintf(stderr, "          -v verbose\n");  
  fprintf(stderr, "          -s path to UNIX domain socket\n");  
  fprintf(stderr, "          -S path to socket (abstract namespace)\n");
  fprintf(stderr, "          -f file to read commands from\n");
  exit(-1);
}

int verbose;
char *file;
FILE *filef;
char buf[256]; // max line length

char *next_line() {
  size_t len;
  char *line,*tmp;

  if (file) line=fgets(buf,sizeof(buf), filef);
  else line=readline(prompt);  // must free it
  if (!line) goto done;

  len = strlen(line);
  
  if (file) { 
    /* fgets keeps trailing newline. null it out. if absent, line got truncated*/
    if (buf[len-1] == '\n') buf[len-1]='\0'; 
    else { fprintf(stderr, "line too long\n"); line=NULL; }
  } else {  
    /* copy the mallocd readline buffer and free it */
    tmp = line;
    if (len+1 < sizeof(buf)) {memcpy(buf, line, len+1); line=buf; }
    else { fprintf(stderr, "line too long\n"); line=NULL; }
    free(tmp); 
  }

 done:
  if (file && !line) fclose(filef);
  return line;
}

/* This little parsing function finds one word at a time from the
 * input line. It supports double quotes to group words together. */
const int ws[256] = {[' ']=1, ['\t']=1};
char *find_word(char *c, char **start, char **end) {
  int in_qot=0;
  while ((*c != '\0') && ws[*c]) c++; // skip leading whitespace
  if (*c == '"') { in_qot=1; c++; }
  *start=c;
  if (in_qot) {
    while ((*c != '\0') && (*c != '"')) c++;
    *end = c;
    if (*c == '"') { 
      in_qot=0; c++; 
      if ((*c != '\0') && !ws[*c]) {
        fprintf(stderr,"text follows quoted text without space\n"); return NULL;
      }
    }
    else {fprintf(stderr,"quote mismatch\n"); return NULL;}
  }
  else {
    while ((*c != '\0') && (*c != ' ')) {
      if (*c == '"') {fprintf(stderr,"start-quote within word\n"); return NULL; }
      c++;
    }
    *end = c;
  }
  return c;
}

int do_rqst(char *line, int fd, int *cr) {
  char *c=line, *start=NULL, *end=NULL;
  tpl_node *tn=NULL,*tr=NULL;
  tpl_bin bbuf;
  int rc = -1;

  tn = tpl_map("A(B)", &bbuf);

  /* parse the line into argv style words, pack and transmit the request */
  while(*c != '\0') {
    if ( (c = find_word(c,&start,&end)) == NULL) goto done;
    //fprintf(stderr,"[%.*s]\n", (int)(end-start), start);
    assert(start && end);
    bbuf.addr =   start;
    bbuf.sz = end-start;
    tpl_pack(tn,1);
    start = end = NULL;
  }
  if (tpl_dump(tn, TPL_FD, fd) == -1) goto done;

  /* get the reply */
  tr = tpl_map("iA(B)", cr, &bbuf);
  if (tpl_load(tr, TPL_FD, fd) == -1) goto done;
  tpl_unpack(tr,0);
  /* do something with the A(B) now. either put it in
   * a cp_cmd_t to return to caller. 
   * or deal with it according to <FILE and >FILE idea
   */

  rc = 0;

 done:
  if (tn) tpl_free(tn);
  if (tr) tpl_free(tr);
  return rc;
}
 
int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  int opt,fd,rc,cr;
  tpl_bin bbuf;
  char *line;

  while ( (opt = getopt(argc, argv, "v+f:s:S:")) != -1) {
    switch (opt) {
      case 'v': verbose++; break;
      case 'f': file = strdup(optarg); break;
      case 's': path = strdup(optarg); break;
      case 'S': path = calloc(strlen(optarg)+2,1); strcpy(path+1,optarg); break;
      default: usage(argv[0]); break;
    }
  }
  if (optind < argc) usage(argv[0]);
  snprintf(prompt,sizeof(prompt),"%s> ", path);
  using_history();

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }

  if (file && !(filef = fopen(file,"r"))) {
    perror("fopen error"); 
    exit(-1);
  }

  while ( (line=next_line()) != NULL) {
    add_history(line);
    if (do_rqst(line,fd,&cr) == -1) break;
    if (cr) printf("non-zero exit status: %d\n",cr);
  }

  clear_history();
  return 0;
}

