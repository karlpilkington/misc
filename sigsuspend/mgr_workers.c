
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <sys/types.h>

/*
 *
 */

int wr;    /* workers running */
int wn=2;  /* workers needed */

sigjmp_buf jmpbuf;

typedef struct {
  pid_t pid;
  time_t start;
} worker_t;

worker_t *workers;

int sighandler(int signo) {
  siglongjmp(jmpbuf,signo);
}
 
void worker(int n) {
  pid_t pid;

  if ( (pid = fork()) == -1) {
    printf("fork error\n"); 
    exit(-1); 
  }
  if (pid > 0) { /* parent. */
    /* record worker */
    workers[n].pid = pid;
    workers[n].start = time();
    return;
  } 
  /* child here */
  /* setproctitle() or prctl(PR_SET_NAME) */
  int maxsleep = 30;
  int awhile = 1.0*maxsleep*rand()/RAND_MAX;
  sleep(awhile);
  exit(0);
}

int main(int argc, char *argv[]) {
    pid_t pid;
    int n,es;

    if ( (workers = malloc(sizeof(worker_t)*wn)) == NULL) 
        exit(-1);

    /* block all signals */
    sigprocmask(SIG_SETMASK,blocked,NULL);

    /* come back here every time a signal happens */
    rc = sigsetjmp(jmpbuf,1);
    switch(rc) {
        case 0: /* initial */
          /* bring up wn workers */
          while(wr < wn) worker(wr++);
          break;
        case SIGCHLD:
          /* loop over children that have exited */
          while( (pid = waitpid(-1,&es,WNOHANG)) > 0) {
              for(n=0; n < wr; n++) if (workers[n].pid==pid) break;
              assert(n != wr);
              int elapsed = time() - workers[n].start;
              printf("pid %d exited after %d seconds: ", (int)pid, elapsed);
              if (WIFEXITED(es)) printf("exit status %d\n", (int)WEXITSTATUS(es));
              else if (WIFSIGNALED(es)) printf("signal %d\n", (int)WTERMSIG(es));
              if (n < wr-1) memmove(&workers[n],&workers[n+1],sizeof(worker_t)*(wr-1-n));
              wr--;
          }
          /* bring up wn workers */
          while(wr < wn) worker(wr++);
          break;
        case SIGALRM:
          /* bring up wn workers */
          while(wr < wn) worker(wr++);
          break;
        default:
          printf("got signal %d\n", rc);
          goto done;
          break;
    }
    /* wait for signals */
    sigsuspend(unblock);

    /* the only way we get past this point
     * is from the "goto done" above, because
     * sigsuspend waits for signals, and when
     * one arrives we longjmp back to sigsetjmp! */

done:
  /* reap any running workers */
  while (nr) {
    n = --nr;
    printf("terminating pid %d\n", workers[n]);
    kill(workers[n].pid, SIGTERM);
    waitpid(workers[n].pid, NULL, 0);
  }
}
