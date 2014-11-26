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

extern char **environ;




void runCommand(Command *cmd){
	char **pl = cmd->pgm->pgmlist;
	int bg = (int) cmd->bakground;
	// printf ("%i\n", bg);
	// printf("in runCommand\n");
	// printf(" %s this the runCommand function\n", *pl);
	Pgm *p = cmd->pgm;
	char **ps = p->pgmlist;

	int res;
	int pfd[2];	
	pipe(pfd);
	signal(SIGCHLD, SIG_IGN);
	res = fork();
	if (res > 0) // Parent process
	{
		if (bg == 0) wait(NULL);
	}
	else // Child process
	{
		FILE *output, *input;
		input = NULL;
		output = NULL;

		if (cmd->rstdin != NULL)
		{
			input = freopen(cmd->rstdin, "r", stdin);
		}
		if (cmd->rstdout != NULL)
		{
			output = freopen(cmd->rstdout, "w", stdout);
		}

		runCmds(cmd, cmd->pgm);
		
		if (input != NULL) fclose(input);
		if (output != NULL) fclose(output);
	}
}


int runCmds(Command *cmd,Pgm *p)
{
	char **ps = p->pgmlist;
	int pfds[2];
	pipe(pfds);
	char b;
	
	if (p == NULL) // Base case
	{
		return 0;
	}
	pid_t pid;
	pid = fork();
	if (pid < 0)
	{
		return -1;
	}
	else if (pid > 0) // Parent process 
	{
		close(pfds[1]);
		// printf("STDIN %s %s\n", cmd->rstdin, ps[0]);
		dup2(pfds[0], STDIN_FILENO);
		close(pfds[0]);
		waitpid(pid,NULL,NULL);
		if ((execvp(ps[0],ps)) < 0)
		{
			return -1;
		}
	}
	else // Child process
	{
		close(pfds[0]);
		// printf("STDOUT %s %s\n", cmd->rstdout, ps[0]);
		dup2(pfds[1], STDOUT_FILENO);
		close(pfds[1]);
		if (p->next != NULL)
		{
			p = p->next;
			runCmds(cmd, p);
		}
		else
		{
			if ((execvp(ps[0],ps)) < 0)
			{
			return -1;
			}
		}
	}


}
