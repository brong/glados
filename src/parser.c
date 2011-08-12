/*
 *	Parser.c
 *
 *  Created on: Jun 28, 2011
 *  Author: Samuel Wejeus (wejeus@kth.se)
 */

#include "Parser.h"

/*
 * For info on fgets(..) see https://buildsecurityin.us-cert.gov/bsi/articles/knowledge/coding/300-BSI.html
 */
char *get_line() {

	char buffer[BUF_SIZE];
	size_t buffers_used = 1;
	int strict_line = FALSE;

	/* Preallocate space.  We could just allocate one char here, but that wouldn't be efficient. */
	char *content = malloc(sizeof(char) * BUF_SIZE);
	if(content == NULL) {
		perror("Failed to allocate content");
		exit(1);
	}

	content[0] = '\0'; /* make sure content is null-terminated */

	/* fgets stops reading on: newline, EOF or error.
	 * If fgets return NULL it means that no string is available. */
	while(!strict_line && fgets(buffer, BUF_SIZE, stdin))
	{
		char *old = content;
		buffers_used += strlen(buffer);
		content = realloc(content, buffers_used);

		/* For debugging purpose, checks how many buffers we fill */
		if (DEBUG_PARSER) printf("Allocated new buffer.\n");

		if(content == NULL) {
			perror("Failed to reallocate content");
			free(old);
			exit(2);
		}

		/* Test if last read contains newline, if so we are done gathering
		 * input and set flag that we have successfully read a command line */
		size_t buf_len = strlen(buffer);
		if (buffer[buf_len - 1] == '\n') {
			buffer[buf_len - 1] = '\0';
			fflush(stdin); /* ignore rest of input after newline */
			strict_line = TRUE;

			/* For debugging purpose.. */
			if (DEBUG_PARSER) printf("Newline found!\n");
		}

		strcat(content, buffer);
	}

	if(ferror(stdin)) {
		free(content);
		perror("Error reading from stdin.");
		exit(3);
	}

	if (strict_line) {
		/* For debugging purpose.. */
		if (DEBUG_PARSER) printf("Read: %s\n", content);

		return content;
	} else {
		return NULL;
	}
}


/* takes a string in const char* and tokenize it into a
 * vector of chars pointed to by **tokens. If tokenization
 * was successful the number of tokens is returned otherwise
 * -1 is returned. */
/* BROKEN CANT NOT REALLOC */
int tokenize(const char *string, char **argv[]) {

	/* Preliminary test if we actually received a string that contains
	 * *something* (maybe \0) to work with */
	if(string == NULL) {
		return -1;
	}

	/* work on a local copy, dont mess with the original string */
	size_t len_string = strlen(string);
	char *input = malloc(len_string*sizeof(char));
	strcpy(input, string);

	/* The string we are going to parse must consist of at least 1 token */
	int argc = 1; //CHANGEME

	if(DEBUG_TOKEN) printf("I will now tokenize the following string: \"%s\"\n\n", input);

	/* Defines on what type of character we want to make a split */
	char *DELIMITER = " ";

	char *cur_token;
	/* also splits, and returns, even if next char is DELIMITER. Loop to get next */
	while(cur_token = strsep(&input,DELIMITER)) {
		if(DEBUG_TOKEN) printf("CUR: %s\n", cur_token);
		if(*cur_token != NULL) break;
	}


	while (cur_token != NULL) {
		if(DEBUG_TOKEN) printf("(start) cur_token: %s and argc is: %d \n",cur_token, argc);

		if(DEBUG_TOKEN) printf("(tokenizer-while, before REalloc) &p: %p, p: %p, *p: %p\n", &argv, argv, *argv);
		/* Reallocates the size of the array to make it equal to the number of tokens found so far */
		*argv = realloc(argv, (argc)*sizeof(char**)); //on first run this will allocate the same amount as first malloc()
		if(argv == NULL){
			fprintf(stderr, "out of memory with %d elements\n", argc);
			exit(1);
		}
		if(DEBUG_TOKEN) printf("(tokenizer-while, after REalloc) &p: %p, p: %p, *p: %p\n", &argv, argv, *argv);

		if(argv == NULL) {
			perror("Failed to reallocate argv");
			exit(2);
		}

		/* this SHOULD make argv[argc-1] point to the area on the heap that stores the value strcpy:ied from cur_token pointed to by tmp_array */
		size_t len = strlen(cur_token);
		if(DEBUG_TOKEN) printf("strlen(cur_token): %d\n", (int) len);

		/* Allocate some space on the heap for our new token */
		argv[(argc-1)] = (char *) malloc((int)(len)*sizeof(char));
		if(DEBUG_TOKEN) printf("malloc for new token. Length: %d with adress: %p\n", (int)(len), argv[(argc-1)]);
		strcpy(argv[(argc-1)], cur_token);

		/* also splits, and returns, even if next char is DELIMITER. Loop to get next */
		while(cur_token = strsep(&input, DELIMITER)) {
			if(DEBUG_TOKEN) printf("CUR: %s\n", cur_token);
			if(*cur_token != NULL) break;
		}
		/* increase number of tokens found */
		argc++;
	}

	/* decrement number of tokens since while loop runs for tokens+1 times */
	--argc;

	if(DEBUG_TOKEN) {
		printf("Returning: %d tokens.\n", argc);
		printf("(in tokenizer, after crunching) &p: %p, p: %p, *p: %p\n", &argv, argv, *argv);
		int i;
		for(i = 0; i < argc; i++) {
			printf("(before return) argv[%d] %s with adress: %p\n", i, argv[i], &argv[i]);
		}
	}

	return argc;
}


