#include <stdio.h>
#include <unistd.h>
#include "parse.h"
#include "mycommand.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#include <sys/wait.h>
// chaguos14
extern char **environ;

// typedef struct b {
//   pid_t pid_n;
//   struct b *next;
//   struct b *back;
// } Pros;



void runCommand(Command *cmd){
	char **pl = cmd->pgm->pgmlist;
	int bg = (int) cmd->bakground;
	// printf ("%i\n", bg);
	// printf("in runCommand\n");
	// printf(" %s this the runCommand function\n", *pl);
	Pgm *p = cmd->pgm;
	char **ps = p->pgmlist;
	int i;
	bool int_out = true;
	bool p_next = (p->next != NULL);
	int pfds[10][2];
	int pfd[2];	
	pipe(pfd);
	for (i = 0;; i++)
	{
		if (i == 0)
		{
			pipe(pfds[i]);
		}
		pid_t pid;
		signal(SIGCHLD, SIG_IGN);
		pid = fork();
		if (pid < 0){
			printf(stderr, "Fork failed");
			return 1;
		}
		else if (pid == 0 ) 
		{ // Child process
			printf("%s %i***\n", ps[0],i);
			int result_t;
			result_t = execvp(ps[0],ps);

		}
		if (p->next == NULL)
		{
			break;
		}
		else
		{	
			wait(NULL);
			p = p ->next;
			ps = p ->pgmlist;
		}
	}
	 // Parent process
	if (!bg){
		wait(NULL);
		printf("Child is done\n");
	}
	else
	{
		printf("Child is on the background and running\n");
			
	}

}