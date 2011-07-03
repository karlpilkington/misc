#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <tpl.h>

char *socket_path = "./barney";
//char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100];
  int fd,rc,rr,tr;

  if (argc > 1) socket_path=argv[1];

  if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }

  tpl_bin bbuf;

  while( (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
    bbuf.addr = buf; bbuf.sz = rc;
    if (buf[rc-1] == '\n') bbuf.sz--;
    tpl_node *tn = tpl_map("A(B)",&bbuf);
    tpl_pack(tn,1);
    tr = tpl_dump(tn,TPL_FD,fd);
    tpl_free(tn);
    if (tr == -1) { fprintf(stderr,"tpl_dump error"); break; }
    /* get a reply */
    tn = tpl_map("iA(B)",&rr,&bbuf);
    tr = tpl_load(tn,TPL_FD,fd);
    if (tr != -1) {
      tpl_unpack(tn, 0);
      printf("remote response code: %d\n", rr);
      while(tpl_unpack(tn,1) > 0) {
        printf("%u bytes:\n%.*s\n", (int)bbuf.sz, (int)bbuf.sz, (char*)bbuf.addr);
        free(bbuf.addr);
      }
    }
    tpl_free(tn);
    // TODO detect EOF ("quit" sends reply tpl but we haven't read EOF yet)
    if (tr == -1) {
      close(fd);
      break;
    }
  }

  return 0;
}

