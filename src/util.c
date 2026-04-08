#include <stdio.h>
#include <string.h>
#include "util.h"

// Helper function that print the commands that can be ran
void print_usage() {
    printf("MyLXC - Simplified LXC Interface\n\n");
    printf("Usage:\n");
    printf("  mylxc run <image> [options]  Create and start container\n");
    printf("  mylxc ps                     List containers\n");
    printf("  mylxc start <name> [...]     Start one or more stopped containers\n");
    printf("  mylxc stop <name> [...]      Stop one or more running containers\n");
    printf("  mylxc rm <name> [...]        Remove one or more containers\n");
    printf("  mylxc limits <name> [...]    Apply resource limits to containers\n");
    printf("\n");
    printf("Run Options:\n");
    printf("  --name <name>                Assign a name to the container\n");
    printf("  -p <host>:<container>        Map a host port to a container port\n");
    printf("  --memory <limit>              Set memory limit for the container\n");
    printf("  --cpu <limit>                 Set CPU limit for the container\n");
    printf("\n");
    printf("Examples:\n");
    printf("  mylxc run ubuntu:22.04\n");
    printf("  mylxc run ubuntu:22.04 --name mycontainer\n");
    printf("  mylxc run ubuntu:22.04 --name webserver -p 8080:80\n");
    printf("  mylxc ps\n");
    printf("  mylxc start container1 container2 container3\n");
    printf("  mylxc stop container1 container2 container3\n");
    printf("  mylxc rm container1 container2 container3\n");
	printf("  mylxc limits container1 --memory 512M --cpu 2\n");
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
