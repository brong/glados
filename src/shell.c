/*
 ============================================================================
 Name        : shell.c
 Author      : Samuel Wejeus
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "parser.h"

#define TYPE_FG 0
#define TYPE_BG 1
#define TYPE_INTERNAL 2
#define TRUE 1
#define FALSE 0

void handle_cd_command(int argc, char **argv);
int command_type(int argc, char **argv);

/* A loop reads in a string of length infinity and parse it into tokens.
 * Some testing is then made on the input to determine if the requested
 * command should be performed in FG or BG or if it is a built-in command.
 * Actions is then taken to carry out the command accordingly.
 */
int main (int argc, char **argv) {

	printf("\nThis is GLaDOS (Genetic Lifeform and Disk Operating System) v0.1-Alpha\n\n");

	/* Flag to indicate if the program loop should continue or not */
	int QUIT = FALSE;

	while(!QUIT) {
		printf("PuniHuman@GLaDOS -> ");

		char *content = get_line();
		if (content == NULL) {
			printf("Read (null) quitting.\n");
			exit(1);
		}

		char **args;
		pid_t pid_bg;
		pid_t pid_fg;

		/* Allocate some initial memory to hold the first set of tokens (2 for pointer to pointer) */
		printf("(shell) &args: %p\n", &args);
		args = (char **)malloc(3*sizeof(char*)); // 6 for num of valid parameters
		printf("(shell) &args: %p, args: %p\n", &args, args);

		int ret = tokenize(content, &args);
		if(ret == 0) continue; /* If no command was entered, restart the loop */

		int i;
		printf("Got %d tokens: ", ret);
		for(i=0;i<ret;i++) {
			printf("[%s] ", args[i]);
		} printf("\n");

		/* make sure to allways null terminate arg vector. */
		args[ret] = NULL;

		switch (command_type(ret, args)) {

		/* handle case of internal commands */
		case TYPE_INTERNAL:
			if(strcmp(args[0], "exit") == 0) {
				printf("Got exit signal.\n");
				QUIT = TRUE;
				continue;
			}
			if(strcmp(args[0], "cd") == 0) {
				handle_cd_command(ret, args);
			} else {
				printf("Internal command not recognized.\n");
			}
			break;
		/* handle case of background processes */
		case TYPE_BG:
			pid_bg = fork();
			if (0 < pid_bg) {
				/* print reduced statistics */
				printf("Spawned BG_PID: %d\n", pid_bg);
			} else if(0 == pid_bg) {
				(void) execvp(args[0], args);
			} else {
				printf("Could not fork!\n");
			}
			break;

		/* handle case of foreground processes */
		case TYPE_FG:
			pid_fg = fork();
			struct timeval starttime;
			gettimeofday(&starttime, NULL);
			if (0 < pid_fg) {
				printf("Spawned FG_PID: %d\n", pid_fg);
			} else if(0 == pid_fg) {
				printf("Command output ----------------------------------------\n\n");
				(void) execvp(args[0], args);
			} else {
				printf("Could not fork!\n");
			}

			/* waits for child to terminate */
			int childExitStatus;
			while(1) {
				waitpid(pid_fg, &childExitStatus, 0);
				if(WIFEXITED(childExitStatus)) break;
			}

			/* calculate and print statistics */
			struct timeval endtime;
			gettimeofday(&endtime, NULL);
			time_t runtime_sec = (endtime.tv_sec - starttime.tv_sec);
			time_t runtime_usec = (endtime.tv_usec - starttime.tv_usec);
			float runtime = (1000000.0*runtime_sec) + runtime_usec;
			printf("\n-------------------------------------------------------\n");
			printf("FG command ran for: %g usec.\n", runtime);
			break;
		}

	}

//	ii) kontrollera om några bakgrundsprocesser avslutast och skriv ut information om dessa.
	exit(EXIT_SUCCESS);
	return 0;
}

void handle_cd_command(int argc, char **argv) {
	if(-1 == chdir(argv[1])) printf("Directory \"%s\" does not exists!\n", argv[1]);
}

/* check values in argument vector and determines type of command (internal, fg, bg) */
int command_type(int argc, char **argv) {
	if( (strcmp(argv[0], "exit") == 0) || (strcmp(argv[0], "cd") == 0)) {
		return TYPE_INTERNAL;
	}

	if(strcmp(argv[(argc-1)], "&") == 0) {
		argv[(argc-1)] = NULL;
		return TYPE_BG;
	}

	/* if not internal or bg then it must be fourground */
	return TYPE_FG;
}
