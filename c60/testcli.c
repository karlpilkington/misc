#include <stdio.h>
#include "c60.h"
int main() {
  UT_string *err;
  utstring_new(err);
  void *c60;

  c60 = c60_client_init_fromfile("config.txt",err);
  if (!c60) fprintf(stderr,"init failed: %s",utstring_body(err));
  return 0;

}
