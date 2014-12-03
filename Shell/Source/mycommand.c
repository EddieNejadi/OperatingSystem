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
		fprintf(stderr,"bgp: %d and pgid: %d and pid: %d\n",bgpg,getpgid(getpid()),getpid());
		_exit(-1);
	}
}

void runCommand(Command *cmd)
{
	pid_t res,resBg;
/*	res = NULL; */
/*	bgp = NULL; */
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
	else if (strcmp(ps[0], "echobgpg") == 0)
	{
		fprintf(stderr, "bgpg:%d and getpgid(getpid()): %d\n",bgpg,getpgid(getpid()));
	}
	else
	{
		if (!bgpg)
		{
			resBg = fork();
			if (resBg == 0)
			{
				bgpg = setsid();
				fprintf(stderr, "resBg: %d and bgpg: %d\n", resBg,bgpg);
				if (bgpg < 0)
				{
					fprintf(stderr, "bgpg = setsid(): %d\n", bgpg);
				}
				if (setpgid(bgpg, 0) < 0)
				{
					fprintf(stderr, "Error occurred setpgid(bgpg, 0): %d\n", setpgid(bgpg, 0));
				}
				waitpid(parent_pid,&bgStatus,0);
				_exit(0);
			}
			// else
			// {
			// 	bgpg = resBg;
			// }
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
				/* if (bgp > 0 && bgp != NULL) */
				if (bgpg)
				{
					setpgid(getpid(), bgpg);
				}
				// else
				// {	
				// 	bgpg = setsid();
				// 	setpgid(bgpg, 0);
				// }

			}
		}
		else /* Child process */
		{
			FILE *output, *input;
	/*		input = NULL; */
	/*		output = NULL; */
				
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
				/* if (bgp != NULL && bgp == getpgid(getpid()))  */
				if (bg != 0)
				{
					/* if (bgp == NULL) bgp = getpid();  */
					if (!bgpg) bgpg = getpid();
					setpgid(getpid(), bgpg);
					fprintf(stderr, "Added to bgpg and pid is %d\n", getpid());
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
			/* if (bgp == NULL) bgp = getpid();  */
			if (!bgpg) bgpg = getpid();
			setpgid(getpid(), bgpg);
			fprintf(stderr, "Added to bgpg and pid is %d\n", getpid());
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
				/* fprintf(stderr, "IN child execvp\n"); */
				_exit(0);
			}
		}
		return 0;
	}
}


