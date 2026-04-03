/*
 * container.c - Container Management Operations
 * 
 * This file implements the core container lifecycle operations:
 * - Creating and starting containers
 * - Listing running containers
 * - Stopping containers
 * - Removing/destroying containers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "container.h"
#include "image.h"
#include "network.h"

/**
 * cmd_run - Create and start an LXC container
 * @img: Image name (e.g., "ubuntu:22.04") - currently unused, hardcoded to Ubuntu
 * @container_name: Name for the new container
 * 
 * Creates a new LXC container using the download template and automatically
 * starts it. Currently only supports Ubuntu Jammy (22.04).
 * 
 * Return: 0 on success, non-zero on failure
 */
int cmd_run(const char *img, const char *container_name, port_mapping *ports){
	// Init image info
	image_info info;

	// Parse image
	parse_image(img, &info);

	char cmd[512];
	// Build LXC create command
	snprintf(cmd, sizeof(cmd), "lxc-create -n %s -t download -- -d %s -r %s -a amd64", 
		 container_name, info.distro, info.release);
	
	// Create container
	int ret = system(cmd);

	// If container was made successfully, start container
	if(ret == 0){
		// Start container -- also new
		ret = cmd_start(container_name);

		// Delay for a second
		sleep(5);

		// Set up port mapping
		if(ports->has_port){
			setup_port_mapping(container_name, ports->host_port, ports->container_port);
		}
	}
	// If container wasn't created, print error 
	else{
		fprintf(stderr, "Error: Failed to create container\n");
	}

	return ret;
}

/**
 * cmd_ps - List all LXC containers
 * 
 * Displays a formatted table of all containers with their state, autostart
 * settings, and other information.
 * 
 * Return: 0 on success, non-zero on failure
 */
int cmd_ps(){
	return system("lxc-ls --fancy");
}

/**
 * cmd_stop - Stop a running container
 * @container_name: Name of the container to stop
 * 
 * Gracefully stops a running LXC container. The container is not destroyed
 * and can be restarted later.
 * 
 * Return: 0 on success, non-zero on failure
 */
int cmd_stop(const char *container_name){
	char cmd[512];
	
	printf("Stopping container %s...\n", container_name);

	// Build LXC stop command
	snprintf(cmd, sizeof(cmd), "lxc-stop -n %s", container_name);
	
	// Stop container
	int ret = system(cmd);
	
	// If container was stopped
	if(ret == 0){
		printf("Container stopped\n");
	}
	// If container wasn't stopped
	else{
		fprintf(stderr, "Error: Failed to stop container\n");
	}

	return ret;
}

/**
 * cmd_stop - Stop a running container
 * @container_name: Name of the container to stop
 * 
 * Gracefully stops a running LXC container. The container is not destroyed
 * and can be restarted later.
 * 
 * Return: 0 on success, non-zero on failure
 */
int cmd_rm(const char *container_name){
    char cmd[512];

	printf("Removing container %s...\n", container_name);

	// Build LXC command destroy
    snprintf(cmd, sizeof(cmd), "lxc-destroy -n %s", container_name);
	
	// Remove container
	int ret = system(cmd);

	// If container was removed
	if(ret == 0){
		printf("Container was removed\n");
	}
	// If container wasn't removed
	else{
		fprintf(stderr, "Error: Failed to remove container\n");
	}

    return ret;
}

// New addition
int cmd_start(const char *container_name){
	char cmd[512];

	printf("Starting conatiner %s...\n", container_name);
	
	// Build LXC start command
	snprintf(cmd, sizeof(cmd), "lxc-start -n %s", container_name);
	
	// Start container
	int ret = system(cmd);

	// If container started
	if(ret == 0){
		printf("Container %s is running\n", container_name);
	}
	// If container wasn't started
	else{
		fprintf(stderr, "Error: Failed to start container\n");
	}

    return ret;

}