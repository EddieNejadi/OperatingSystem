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
static pid_t ch;



void exitHandler(int sig) 
{
	printf("\nin exitHandler signal is: %d and pid is: %d\n", sig, getpid());
    if (ch != NULL)
    {
    	kill(ch,9);
    }
    // return 0;
}

void runCommand(Command *cmd)
{
	ch = NULL;
	char **pl = cmd->pgm->pgmlist;
	int bg = (int) cmd->bakground;
	// printf ("%i\n", bg);
	// printf("in runCommand\n");
	// printf(" %s this the runCommand function\n", *pl);
	Pgm *p = cmd->pgm;
	char **ps = p->pgmlist;

	if (strcmp(ps[0], "exit") == 0)
	{
		exit(0);
	}
	pid_t res;
	res = NULL;
	// int pfd[2];	
	// pipe(pfd);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGINT, exitHandler);
	if (bg == 0)
	{
		ch = fork();
	}else
		res = fork();
	{
	}
	if ((ch < 0 && res == NULL) || (res < 0 && ch == NULL))
	{
		printf("fork get error\n");
	}
	if ((ch > 0 && res == NULL) || (res > 0 && ch == NULL)) // Parent process
	{
		if (bg == 0) // Check if the ch is not background process
		{
			printf("Parent pid is: %d and ch is %d\n", getpid(), ch);
			if (ch > 0) waitpid(ch,NULL,NULL);
			else waitpid(res,NULL,NULL);
			// while(c)
			// {

			// }	
			printf("DONE\n");
		} 
	}
	else // ch process
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
	else // ch process
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


