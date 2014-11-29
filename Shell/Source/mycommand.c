#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include "parse.h"
#include "mycommand.h"


extern char **environ;
static pid_t parent_pid;
static pid_t child_pid;
static pid_t bgp;



void exitHandler(int sig) 
{

	if (parent_pid != getpid() && bgp != getpgid(getpid()) )
	{
		_exit(-1);
	}
}

void runCommand(Command *cmd)
{
	parent_pid = getpid();
	int bg = (int) cmd->bakground;
	Pgm *p = cmd->pgm;
	char **ps = p->pgmlist;

	if (strcmp(ps[0], "exit") == 0)
	{
		exit(0);
	}
	else if (strcmp(ps[0], "cd") == 0)
		{
		chdir(ps[1]);
		}
	pid_t res;
	res = NULL;
	bgp = NULL;

	signal(SIGINT, exitHandler);
		signal(SIGCHLD, SIG_IGN);
	res = fork();

	if (res < 0)
	{
		fprintf(stderr,"fork() was unsuccessful\n");
	}
	if (res > 0) // Parent process
	{
		if (bg == 0) // Check if the child is not background process
		{
			child_pid = res;
			waitpid(res,NULL,NULL);
		}
		else // Background process
		{
			if (bgp > 0 && bgp != NULL)
			{
				setpgid(getpid(), bgp);
		}
		else
		{	
				bgp = setsid();
				setpgid(bgp, 0);
		}

	}
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
		if (p->next != NULL)
		{
			runCmds(cmd, cmd->pgm);
		}
		else
		{
			// if (bgp != NULL && bgp == getpgid(getpid())) 
			if (bg != 0)
			{
				if (bgp == NULL) bgp = getpid();  
				setpgid(getpid(), bgp);
				fprintf(stderr, "Added to bgpg\n");
			}
			if ((execvp(ps[0],ps)) < 0)
			{
				_exit(-1);
			}
			else
			{
				_exit(0);
			}
		}
		
		if (input != NULL) fclose(input);
		if (output != NULL) fclose(output);
	}
}


int runCmds(Command *cmd,Pgm *p)
{
	char **ps = p->pgmlist;
	int pfds[2];
	pipe(pfds);
	
	if (p == NULL) // Base case
	{
		return 0;
	}
	pid_t pid;
	signal(SIGINT, exitHandler);
	pid = fork();
	if (pid < 0)
	{
		return -1;
	}
	else if (pid > 0) // Parent process 
	{
		close(pfds[1]);
		dup2(pfds[0], STDIN_FILENO);
		close(pfds[0]);
		if(cmd->bakground)
		{
			if (bgp == NULL) bgp = getpid();  
			setpgid(getpid(), bgp);
			fprintf(stderr, "Added to bgpg\n");
		}
		waitpid(pid,NULL,NULL);
		if ((execvp(ps[0],ps)) < 0)
		{
			return -1;
		}
		_exit(0);
	}
	else // Child process
	{
		close(pfds[0]);
		dup2(pfds[1], STDOUT_FILENO);
		close(pfds[1]);
		if (p->next != NULL)
		{
			runCmds(cmd, p->next);
		}
		else
		{
			if ((execvp(ps[0],ps)) < 0)
			{
				fprintf(stderr, "IN error of child execvp\n");
				_exit(-1);
			}
			else
			{
				fprintf(stderr, "IN child execvp\n");
				_exit(0);
			}
		}
	}
}


