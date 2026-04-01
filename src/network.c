#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"

int setup_port_mapping(const char *container_name, int host_port, int container_port){
	char container_ip[64];
	char cmd[512];
	
	// Build lxc command lxc-info
	printf("Retrieving container %s's IP address...\n", container_name);
        snprintf(cmd, sizeof(cmd), "lxc-info -n %s -iH 2>&1", container_name);

	// DEBUG: print built command
	//printf("Running command: %s\n", cmd);

	// Run command and store IP
	FILE *fp = popen(cmd, "r");
	if(fp == NULL){
		fprintf(stderr, "Error: couldn't get container info\n");
		return -1;
	}

	// Debug: check if there's any data being outputted from the command
	//int c; // Temporary variable to hold characters
	//while ((c = fgetc(fp)) != EOF) {
    	//	putchar(c); // Print character by character to the console
	//}
	
	// Read the IP addr from command output
	if(fgets(container_ip, sizeof(container_ip), fp) == NULL){
        	fprintf(stderr, "Error: couldn't read container IP\n");
        	pclose(fp);
        	return -1;
	}

	// Remove newline
	container_ip[strcspn(container_ip, "\n")] = '\0';
    	printf("Container IP: %s\n", container_ip);
 	
    	// Setup iptables rule
    	printf("Setting up port forwarding: %d -> %s:%d\n",
           	host_port, container_ip, container_port);
    
    	// DNAT rule
	snprintf(cmd, sizeof(cmd),
        	"iptables -t nat -A PREROUTING -p tcp --dport %d "
        	"-j DNAT --to-destination %s:%d",
        	host_port, container_ip, container_port);
	system(cmd);

	// FORWARD rule
	snprintf(cmd, sizeof(cmd),
        	"iptables -A FORWARD -p tcp -d %s --dport %d -j ACCEPT",
        	container_ip, container_port);
	system(cmd);

	// MASQUERADE rule
	snprintf(cmd, sizeof(cmd),
        	"iptables -t nat -A POSTROUTING -s %s -j MASQUERADE",
        	container_ip);

	// Run iptable rules
    	int ret = system(cmd);

    	if(ret == 0){
        	printf("Port mapping configured\n");
    	} 
	else{
        	fprintf(stderr, "Failed to configure port mapping\n");
    	}
	return 0;
}
