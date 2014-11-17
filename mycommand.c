#include <stdio.h>
#include <unistd.h>
#include "parse.h"
#include "mycommand.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;



void runCommand(Command *cmd){
	char **pl = cmd->pgm->pgmlist;
	// printf("in runCommand\n");
	printf(" %s this the runCommand function\n", *pl);
	pid_t pid;
	pid = fork();
	if (pid < 0){
		printf(stderr, "Fork failed");
		return 1;
	}
	else if (pid == 0 ) { // Child process
		int result_t;
		char my_env[100] = "/usr/bin/";
		// strcpy ( my_env, "/usr/bin/");
		// printf("testing print function\n");
		strcat(my_env, *pl); 
		printf("**************this is my env: %s, %s, %s\n", my_env, pl[0], pl[1]);
		// result_t = execle(my_env, pl, NULL);
		// result_t = exevp(*pl, *(++pl));
		result_t = execvp(pl[0],pl);
		// char * s = malloc(_scprintf("%s %s", my_env, pl) + 1);
		// result_t = execlp(s, pl, NULL);
		// sprintf(s,"%s %s", my_env, pl);

	}
	else { // Parent process
		wait(NULL);
		printf("Child is done\n");

	}

}