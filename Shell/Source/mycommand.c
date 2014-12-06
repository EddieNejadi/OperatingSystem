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


static pid_t parent_pid;
static pid_t child_pid;
static pid_t bgpg;


int runCmds(Command *cmd,Pgm *p);

void exitHandler(int sig) 
{

	if (parent_pid != getpid() && bgpg != getpgid(getpid()) )
	{
		_exit(-1);
	}
}

void runCommand(Command *cmd)
{
	pid_t res,resBg;
	int bg, status, bgStatus;
	Pgm *p;
	char **ps;
	parent_pid = getpid();
	bg = cmd->bakground;
	p = cmd->pgm;
	ps = p->pgmlist;

	if (strcmp(ps[0], "exit") == 0)
	{
		exit(0);
	}
	else if (strcmp(ps[0], "cd") == 0)
	{
		chdir(ps[1]);
	}
	else
	{
		if (!bgpg)
		{
			resBg = fork();
			if (resBg == 0)
			{
				bgpg = setsid();
				waitpid(parent_pid,&bgStatus,0);
				_exit(0);
			}
		}
		signal(SIGINT, exitHandler);
		signal(SIGCHLD, SIG_IGN);
		res = fork();
		if (res < 0)
		{
			fprintf(stderr,"fork() was unsuccessful\n");
		}
		if (res > 0) /* Parent process */
		{
			if (bg == 0) /* Check if the child is not background process */
			{
				child_pid = res;
				waitpid(res,&status,0);
			}
			else /* Background process */
			{
				if (bgpg)
				{
					setpgid(getpid(), bgpg);
				}
			}
		}
		else /* Child process */
		{
			FILE *output, *input;

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
				if (bg != 0)
				{
					if (!bgpg) bgpg = getpid();
					setpgid(getpid(), bgpg);
				}
				if ((execvp(ps[0],ps)) < 0)
				{
					fprintf(stderr, "Bad command\n");
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
}


int runCmds(Command *cmd,Pgm *p)
{
	int status;
	char **ps;
	pid_t pid;
	int pfds[2];
	ps = p->pgmlist;
	pipe(pfds);
	
	if (p == NULL) /* Base case */
	{
		return 0;
	}
	signal(SIGINT, exitHandler);
	pid = fork();
	if (pid < 0)
	{
		return -1;
	}
	else if (pid > 0) /* Parent process */
	{
		close(pfds[1]);
		dup2(pfds[0], STDIN_FILENO);
		close(pfds[0]);
		if(cmd->bakground)
		{
			if (!bgpg) bgpg = getpid();
			setpgid(getpid(), bgpg);
		}
		waitpid(pid,&status,0);
		if ((execvp(ps[0],ps)) < 0)
		{
			return -1;
		}
		_exit(0);
		return 0;
	}
	else /* Child process */
	{
		close(pfds[0]);
		dup2(pfds[1], STDOUT_FILENO);
		close(pfds[1]);
		if (p->next != NULL)
		{
			runCmds(cmd, p->next);
			return 0;
		}
		else
		{
			if ((execvp(ps[0],ps)) < 0)
			{
				fprintf(stderr, "Error occurred in child execvp function\n");
				_exit(-1);
			}
			else
			{
				_exit(0);
			}
		}
		return 0;
	}
}


