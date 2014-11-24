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
	int res;
	// bool int_out = true;
	// bool buf_empty = true;
	// char buf [10000];
	// bool p_next = (p->next != NULL);
	// int pfds[10][2];
	int pfd[2];	
	pipe(pfd);
	// if (fork() > 0)
	// {
	// 	wait(NULL);
	// }
	// else
	// {
	// 	runCmds((cmd->pgm), pfd, 0);
	// }
		runCmds((cmd->pgm), pfd, 0);
	// res = fork();
	// if ( res < 0)
	// 	{
	// 		printf("Some kind of error occurred\n");;
	// 	}
	// else if (res == 0)
	// {
	// 	printf("in the child\n");
	// }
	// else{
		// wait(NULL);
	// }
	// for (i = 0;; i++)
	// {
	// 	// if (i == 0)
	// 	// {
	// 	// 	pipe(pfds[i]);
	// 	// }
	// 	if (buf_empty) // Try to empty the buffer if it should be empty
	// 	{
	// 		memset(buf, 0, sizeof(buf));
	// 	}
	// 	pid_t pid;
	// 	signal(SIGCHLD, SIG_IGN);
	// 	pid = fork();
	// 	if (pid < 0){
	// 		printf(stderr, "Fork failed");
	// 		return 1;
	// 	}
	// 	else if (pid == 0 ) 
	// 	{ // Child process
	// 		printf("%s %i***\n", ps[0],i);
	// 		dup2(pfd[1], STDOUT_FILENO);
	// 		close(pfd[1]);
			
	// 		// if (!buf_empty)
	// 		// {
				
	// 		// 	dup2(pfd[0], STDIN_FILENO);
	// 		// 	write(STDIN_FILENO, buf, sizeof(buf));
	// 		// 	buf_empty = true;
	// 		// }
	// 		close(pfd[0]);
	// 		// else if (p_next)
	// 		// {
	// 		// 	close(STDOUT_FILENO);
	// 		// 	dup(pfds[i][STDOUT_FILENO]);
	// 		// 	close(STDIN_FILENO);
	// 		// 	dup(pfds[i-1][STDIN_FILENO]);
	// 		// }
	// 		// else
	// 		// {
	// 			// close(0);
	// 			// dup(pfd[0]);
	// 			// close();
	// 			// close(STDOUT_FILENO);
	// 		// }
	// 		int result_t;
	// 		result_t = execvp(ps[0],ps);

	// 	}
	// 	else
	// 	{
	// 		 // Parent process
	// 		if (!bg){
	// 			wait(NULL);
	// 			printf("Child is done\n");
	// 			close(pfd[1]);
	// 			dup2(pfd[0], STDIN_FILENO);
	// 			close(pfd[0]);
	// 			memset(buf, 0, sizeof(buf));
	// 			if ((read(STDIN_FILENO, buf, sizeof(buf))) > 0)
 //    			{
 //    				buf_empty = false;
 //        			printf("Buffer is filled\n");
 //    			}
 //    			else
 //    			{
 //    				printf("reading STDIN encounter an error\n");
 //    			}      
				
	// 		}
	// 		else
	// 		{
	// 			printf("Child is on the background and running\n");
					
	// 		}
	// 		if (p->next == NULL)
	// 		{
	// 			if (!buf_empty)
	// 			{
	// 				printf("%s", buf);
	// 			}
	// 			printf("in the break\n");
	// 			// break;
	// 		}
	// 		else
	// 		{	
	// 			// wait(NULL);
	// 			p = p ->next;
	// 			ps = p ->pgmlist;
	// 		}
	// 	}
	// }

}

	// int pi[2];
	// pipe(pi);
int runCmds(Pgm *p)
{
	char **ps = p->pgmlist;
	int pfds[2];
	pipe(pfds);
	
	if (p == NULL) // Base case
	{
		return 0;
	}
	int res;
	res = fork();
	if (res < 0)
	{
		return -1;
	}
	else if (res > 0) // Parent process 
	{
		close(pfds[1]);
		dup2(pfds[0], STDIN_FILENO);
		close(pfds[0]);
		wait(NULL);
		if ((execvp(ps[0],ps)) < 0)
		{
			return -1;
		}
	}
	else // Child process
	{
		close(pfds[0]);
		dup2(pfds[1], STDOUT_FILENO);
		close(pfds[1]);
		if (p->next != NULL)
		{
			p = p->next;
			runCmds(p);
		}
		else
		{
			if ((execvp(ps[0],ps)) < 0)
			{
			return -1;
			}
			// free(pfd);
		}
	}


}

// void runCommand(Command *cmd)
// {
// 	char **pl = cmd->pgm->pgmlist;
// 	int bg = (int) cmd->bakground;
// 	// printf ("%i\n", bg);
// 	// printf("in runCommand\n");

// 	int i;
// 	Pgm *p = cmd->pgm;

// 	char **s = p->pgmlist;
// 	printf(" %s this the runCommand function\n", *s);
// 	int pipefd[2];
// 	pipe(pipefd);
// 	pid_t pid;
// 	signal(SIGCHLD, SIG_IGN);
// 	pid = fork();
// 	if (pid < 0)
// 	{
// 		printf(stderr, "Fork failed");
// 		return EXIT_FAILURE;
// 	}
// 	else if (pid == 0 ) 
// 	{ // Child process
// 		close(1);
// 		dup(pipefd[1]);
// 		close(pipefd[0]);
// 		int result_t;
// 		result_t = execvp(pl[0],pl);
// 		if (result_t < 0) return EXIT_FAILURE;
// 	}
// 	else 
// 	{ // Parent process
// 		if (!bg){
// 			close(0);
// 			dup(pipefd[0]);
// 			close(pipefd[1]);
// 			char b[10] ={0};
// 			int c=0;
// 			wait(NULL);
// 			execvp("wc", pl);
// 			printf("Child is done\n");
// 			// memset(pip_buf, 0 , sizeof(pip_buf));
// 			int r = 0;
// 			while (r = read(pipefd[0], b, 10) > 1)
// 			{
// 				printf("in the while\n");
// 			}
// 				printf ("%s", b);
// 			printf ("pip is red and write to pip_buf and result is %i\n", r);

// 		}
// 		else
// 		{
// 			printf("Child is on the background and running\n");
				
// 		}
// 	}

	// int pipefd[10][2];
	// char pip_buf[10000] = {0};
	// for (i =0; i < 2; i++)
	// {
	// 	char **s = p->pgmlist;
	// 	printf(" %s this the runCommand function\n", *s);
		
	// 	bool p_bool = ( p->next != NULL);
	// 	if( p_bool) 
	// 	{
	// 		// memset(pip_buf, 0 , sizeof(pip_buf));
	// 		if (pipe(pipefd[i]) == -1) return EXIT_FAILURE;
	// 		printf("pipe is created\n");	
	// 	}


	// 	pid_t pid;
	// 	signal(SIGCHLD, SIG_IGN);
	// 	pid = fork();
	// 	if (pid < 0)
	// 	{
	// 		printf(stderr, "Fork failed");
	// 		return EXIT_FAILURE;
	// 	}
	// 	else if (pid == 0 ) { // Child process
	// 		// if(p_bool) close(pipefd[i][1]);
	// 		int result_t;
	// 		/*
	// 		char my_env[100] = "/usr/bin/";
	// 		strcat(my_env, *pl); 
	// 		*/

	// 		// printf("testing print function\n");
	// 		// printf("**************this is my env: %s, %s, %s\n", my_env, pl[0], pl[1]);
	// 		// result_t = execle(my_env, pl, NULL);
	// 		// result_t = exevp(*pl, *(++pl));
	// 		if(p_bool) 
	// 		{
	// 			printf("in the p_bool\n");
	// 			close(pipefd[i][0]);
	// 			dup2(pipefd[i][1], STDOUT_FILENO);
	// 		}
	// 		// if (i > 0) dup2(pip_buf, STDIN_FILENO);
	// 		result_t = execvp(pl[i],pl);
	// 		if (result_t < 0) return EXIT_FAILURE;
	// 		// printf("Buffer ===================================\n");
	// 		// printf("%s\n", *pip_buf);
	// 		// printf("End of Buffer ============================\n");
	// 		// write(pipefd[i][1]);
	// 		// char * s = malloc(_scprintf("%s %s", my_env, pl) + 1);
	// 		// result_t = execlp(s, pl, NULL);
	// 		// sprintf(s,"%s %s", my_env, pl);

	// 	}
	// 	else 
	// 	{ // Parent process
	// 		if( p_bool) 
	// 		{
	// 			close(pipefd[i][1]);
	// 			char b[10] ={0};
	// 			int c=0;
	// 			wait(NULL);
	// 			memset(pip_buf, 0 , sizeof(pip_buf));
	// 			int r = 0;
	// 			while (r = read(pipefd[i][1], b, 1) > 1)
	// 			{
	// 				pip_buf[c++]= b;
	// 				printf ("%s", *b);
	// 			}
	// 			printf ("pip is red and write to pip_buf and result is %i\n", pipefd[i][0]);
	// 			printf("%s\n", *pip_buf);

	// 		} 
				

	// 		if (!bg){
	// 			// wait(NULL);
	// 			printf("Child is done\n");
	// 		}
	// 		else
	// 		{
	// 			Pros *cp;
	// 			cp -> pid_n  = pid;
	// 			cp -> back = root;
	// 			cp -> next = NULL;
	// 			printf("Child is on the background and running\n");
					
	// 		}

	// 	}




	// 	p = p->next;
	// }
// }