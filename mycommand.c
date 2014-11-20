#include <stdio.h>
#include <unistd.h>
#include "parse.h"
#include "mycommand.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

// chaguos14
extern char **environ;

typedef struct b {
  pid_t pid_n;
  struct b *next;
  struct b *back;
} Pros;



void runCommand(Command *cmd){
	char **pl = cmd->pgm->pgmlist;
	int bg = (int) cmd->bakground;
	printf ("%i\n", bg);
	// printf("in runCommand\n");
	// printf(" %s this the runCommand function\n", *pl);
	
	Pros *root;
	// TODO: Handle malloc error numbers 
	root = (Pros *) malloc( sizeof(Pros) ); 
	root -> pid_n  = (pid_t) 0; // This is the first process which the Root process with pid 0 as our definition
	root -> back = NULL;
	root -> next = NULL;


	pid_t pid;
	signal(SIGCHLD, SIG_IGN);
	pid = fork();
	if (pid < 0){
		printf(stderr, "Fork failed");
		return 1;
	}
	else if (pid == 0 ) { // Child process
		int result_t;
		/*
		char my_env[100] = "/usr/bin/";
		strcat(my_env, *pl); 
		*/

		// printf("testing print function\n");
		// printf("**************this is my env: %s, %s, %s\n", my_env, pl[0], pl[1]);
		// result_t = execle(my_env, pl, NULL);
		// result_t = exevp(*pl, *(++pl));
		result_t = execvp(pl[0],pl);
		// char * s = malloc(_scprintf("%s %s", my_env, pl) + 1);
		// result_t = execlp(s, pl, NULL);
		// sprintf(s,"%s %s", my_env, pl);

	}
	else { // Parent process
		if (!bg){
			wait(NULL);
			printf("Child is done\n");
		}
		else
		{
			Pros *cp;
			cp -> pid_n  = pid;
			cp -> back = root;
			cp -> next = NULL;
			printf("Child is on the background and running\n");
				
		}

	}

}