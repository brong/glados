/*
 * Parser.h
 *
 *  Created on: Jun 28, 2011
 *  Author: Samuel Wejeus (wejeus@kth.se)
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_PARSER 0
#define DEBUG_TOKEN 1

#define BUF_SIZE 8
#define TRUE 1
#define FALSE 0

char *get_line();
int tokenize(const char *string, char **argv[]);

#endif /* PARSER_H_ */
