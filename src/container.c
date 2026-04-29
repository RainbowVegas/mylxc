/**
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
#include "json.h"

/**
 * apply_resource_limits - Apply resource limits to a container
 * @container_name: Name of the container
 * @memory_limit: Memory limit (e.g. "512M", "1G")
 * @cpu_limit: CPU limit in microseconds (e.g. 50000 for 50ms of CPU time)
 *
 * This function applies memory and CPU limits to the specified container using lxc-cgroup.
 *
 * Return: 0 on success, non-zero on failure
 */
int apply_resource_limits(const char *container_name, const char *memory_limit, int cpu_limit){
	char cmd[512];

	// Apply memory limit if specified
	if(strlen(memory_limit) > 0){
		snprintf(cmd, sizeof(cmd), "lxc-cgroup -n %s memory.max %s", container_name, memory_limit);
        	if(system(cmd) != 0){
            		fprintf(stderr, "Error: Failed to apply memory limit\n");
            		return 1;
        	}
		printf("%s has had its memory limited to %s\n", container_name, memory_limit);
	}

	// Apply CPU limit if specified
	if(cpu_limit > 0){
		// cpu_limit is number of CPUs, convert to quota (period is always 100000)
        	int quota = cpu_limit * 100000;
        	snprintf(cmd, sizeof(cmd), "lxc-cgroup -n %s cpu.max \"%d 100000\"", container_name, quota);
        	if(system(cmd) != 0){
            		fprintf(stderr, "Error: Failed to apply CPU limit\n");
            		return 1;
        	}
		printf("%s has been limited to %d CPUs\n", container_name, cpu_limit);
	}

	return 0; 
}

/**
 * cmd_run - Create and start an LXC container
 * @img: Image name (format: "distro:version", e.g. "ubuntu:20.04")
 * @container_name: Name for the new container
 * @ports: Port mapping information (currently only supports one port mapping)
 * @memory_limit: Memory limit for the container
 * @cpu_limit: CPU limit for the container
 * 
 * Creates a new LXC container based on the specified image, assigns it a name, 
 * and starts it. Optionally sets up port mapping and resource limits.
 * 
 */
int cmd_run(const char *img, const char *container_name, port_mapping *ports,
			const char *memory_limit, int cpu_limit){
	// Init image info
	image_info info;

	// Parse image
	int ret = parse_image(img, &info);
	if(ret != 0) return 1;

	char cmd[512];
	// Build LXC create command
	snprintf(cmd, sizeof(cmd), "lxc-create -n %s -t download -- -d %s -r %s -a amd64", 
		 container_name, info.distro, info.release);
	
	// Create container
	ret = system(cmd);

	// If container was made successfully, start container
	if(ret == 0){
		// Start container -- also new
		ret = cmd_start(container_name);
		if(ret != 0) return ret;

		// Set up port mapping
		if(ports->has_port){
			// Wait 8 secs for IP to get created
			sleep(8);
			setup_port_mapping(container_name, ports->host_port, ports->container_port);
		}

		// Apply resource limits
		ret = apply_resource_limits(container_name, memory_limit, cpu_limit);
		if(ret != 0) return ret;
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
	int ret;	

	printf("Stopping container %s...\n", container_name);
	
	// If contaier exists in JSON file
	if(container_exists(container_name)){
                // Clean up iptable rules
                ret = iptable_cleanup(container_name);
        	if(ret != 0) return ret;
	}

	// Build LXC stop command
	snprintf(cmd, sizeof(cmd), "lxc-stop -n %s", container_name);
	
	// Stop container
	ret = system(cmd);
	
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
 * cmd_rm - Remove a container
 * @container_name: Name of the container to remove
 * 
 * Gracefully removes an LXC container. The container is destroyed and cannot be restarted.
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
		// If container exists in JSON file
		if(container_exists(container_name)){
			// Clean up iptable rules
			//ret = iptable_cleanup(container_name);
			//if(ret != 0) return ret;
			
			// Remove container info from JSON
			ret = remove_container_info(container_name);
			if(ret != 0) return ret;
		}
	}
	// If container wasn't removed
	else{
		fprintf(stderr, "Error: Failed to remove container\n");
	}

    return ret;
}

/**
 * cmd_start - Start a container
 * @container_name: Name of the container to start
 * 
 * Gracefully starts an LXC container. The container must already exist.
 * 
 * Return: 0 on success, non-zero on failure
 */
int cmd_start(const char *container_name){
	char cmd[512];

	printf("Starting container %s...\n", container_name);
	
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

	// If container exists in JSON (for stop and start)
	if(container_exists(container_name)){
		// Wait 8 seconds for container to boot before updating JSON
		sleep(8);
		// Update container info in JSON with new IP address
		update_container_ip(container_name);
		

    		// Re-apply iptables rules with new IP
    		int host_port, container_port;
    		char ip[64];
    		if(get_container_info(container_name, ip, &host_port, &container_port) == 0){
        		setup_port_mapping(container_name, host_port, container_port);
    		}	
	}

    return ret;
}
