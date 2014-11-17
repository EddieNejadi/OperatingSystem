#include <stdio.h>
#include <signal.h>
#include <unistd.h>
void sigtest(int sig) {
  if(sig == SIGINT)
    printf("in if \n");
    
  /* POINT #1: User pressed a key
   * combination, but which one? */
  else if(sig == SIGTSTP)
  /* POINT #2: And here? */
    printf("in else if \n");
  else
    printf("in else\n");
  /* All other signals go here. */
}

int main(void) {
  signal(SIGINT, sigtest);
  while (1)
    pause();
}