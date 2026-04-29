#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "json.h"

char* get_ip(const char *container_name){
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
		return NULL;
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
        	return NULL;
	}

	// Remove newline
	container_ip[strcspn(container_ip, "\n")] = '\0';
	printf("Container IP: %s\n", container_ip);	

	return strdup(container_ip);
}

/** 
 * setup_port_mapping - Set up port mapping for a container
 * @container_name: Name of the container
 * @host_port: Host port number
 * @container_port: Container port number
 *
 * This function retrieves the IP address of the specified container and sets up iptables rules for port forwarding.
 *
 * Return: 0 on success, non-zero on failure
 */
int setup_port_mapping(const char *container_name, int host_port, int container_port){
	// Get ip addr of container
	char *container_ip= get_ip(container_name);
	if(container_ip == NULL){
		return 1;
	}

	char cmd[512];
 	
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
		store_container_info(container_name, container_ip, host_port, container_port);
	} 
	else{
		fprintf(stderr, "Failed to configure port mapping\n");
	}

	free(container_ip);

	return ret;
}

/** 
 * iptables_cleanup - Clean up iptables rules for a container
 * @container_name: Name of the container
 *
 * This function removes the iptables rules associated with the specified container.
 *
 * Return: 0 on success, non-zero on failure
 */
int iptable_cleanup(const char *container_name){
    char ip[64];
    int host_port;
    int container_port;

    // Get container info from JSON
    if(get_container_info(container_name, ip, &host_port, &container_port) != 0){
        fprintf(stderr, "Error: couldn't get container info for cleanup\n");
        return 1;
    }

    char cmd[512];

    // Remove DNAT rule
    snprintf(cmd, sizeof(cmd),
        "iptables -t nat -D PREROUTING -p tcp --dport %d "
        "-j DNAT --to-destination %s:%d",
        host_port, ip, container_port);
    system(cmd);

    // Remove FORWARD rule
    snprintf(cmd, sizeof(cmd),
        "iptables -D FORWARD -p tcp -d %s --dport %d -j ACCEPT",
        ip, container_port);
    system(cmd);

    // Remove MASQUERADE rule
    snprintf(cmd, sizeof(cmd),
        "iptables -t nat -D POSTROUTING -s %s -j MASQUERADE", ip);
    int ret = system(cmd);

    return ret;
}
