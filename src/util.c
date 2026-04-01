#include <stdio.h>
#include <string.h>
#include "util.h"

// Helper function that print the commands that can be ran
void print_usage() {
    printf("MyLXC - Simplified LXC Interface\n\n");
    printf("Usage:\n");
    printf("  mylxc run <image>           Create and start container\n");
    printf("  mylxc ps                    List containers\n");
    printf("  mylxc stop <n>              Stop container\n");
    printf("  mylxc rm <n>                Remove container\n");
    printf("\n");
    printf("Examples:\n");
    printf("  mylxc run ubuntu:22.04\n");
    printf("  mylxc ps\n");
    printf("  mylxc stop mycontainer\n");
}

/* 
 * Helper function that parses strings that have a ':' seperator for left/right
 *  Returns:
 *  0  = Success
 *  -1 = Couldn't find colon
 *  -2 = Size of string is bigger than left's buffer
 *  -3 = Size of string is bigger then right's buffer
 */ 
int parse_col_seperator(const char *input, char *left, size_t left_size,
		        char *right, size_t right_size){
        // Get first occurence of colon
	const char *colon = strchr(input, ':');

	// If colon wasn't found ret -1
	if(colon == NULL){return -1;}

	// Get length of left part
	size_t left_len = colon - input;
	// If length of left is greater than or equal to size of buffer
	if(left_len >= left_size){return -2;}
	// Copy left part
	strncpy(left, input, left_len);
	left[left_len] = '\0';

	// Get right chars
	const char *right_front = colon + 1;
	// If length of right part is greater than or equal to size of buffer
	if(strlen(right_front) >= right_size){return -3;}
	// Copy right part
	strcpy(right, right_front);

	return 0;
}
