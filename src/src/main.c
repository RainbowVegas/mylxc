/*
 * main.c - MyLXC Entry Point
 * 
 * Handles command-line argument parsing and routes commands to
 * appropriate handler functions.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "container.h"
#include "util.h"

/**
 * run_handler - Handle 'run' command
 * @argc: Argument count
 * @argv: Argument vector
 * 
 * Parses arguments for the 'run' command, validates them, and calls cmd_run to 
 * create and start a container. Supports optional flags for container name, and port mapping.
 * 
 * Return: 0 on success, non-zero on failure
 */
int run_handler(int argc, char **argv){
    // Validate image argument
    if(argc < 3){
        fprintf(stderr, "Error: 'run' requires more info\n");
        return 1;
    }
    
    // Variables for optional flags
    char container_name[128] = "";
    port_mapping ports = {0,0,0};
    char memory_limit[64] = "";
    int cpu_limit = 0;
    
    // For every arg after image
    for(int i = 3; i < argc; i++){
        // Check for --name flag
        if(strcmp(argv[i], "--name") == 0){
                // If name was provided
                if(i + 1 < argc){
                    // Copy name given
                    strcpy(container_name, argv[i + 1]);
                    i++;
                }
                // If name wasn't provided
                else{
                    fprintf(stderr, "Error: --name requires a value\n");
                    return 1;
                }	
        }
        // Check for -p flag
        else if(strcmp(argv[i], "-p") == 0){
            // If port mapping was provided
            if(i + 1 < argc){
                // If parse was successful
                if(sscanf(argv[i + 1], "%d:%d", &ports.host_port, &ports.container_port) == 2){
                    ports.has_port = 1;
                    i++;
                }
                // If parse failed
                else{
                    fprintf(stderr, "Error: bad port mapping format\n");
                    return 1;
                }
            }
            // If port mapping wasn't provided
            else{
                fprintf(stderr, "Error: port mapping wasn't provided\n");
                return 1;
            }
        }
        // Check for --cpu flag
        else if(strcmp(argv[i], "--cpu") == 0){
            // If CPU limit was provided
            if(i + 1 < argc){
                // Copy CPU limit given
                cpu_limit = atoi(argv[i + 1]);
                i++;
            }
            // If CPU limit wasn't provided
            else{
                fprintf(stderr, "Error: CPU limit wasn't provided\n");
                return 1;
            }
        }
        // Check for --memory flag
        else if(strcmp(argv[i], "--memory") == 0){
            // If memory limit was provided
            if(i + 1 < argc){
                // Copy memory limit given
                strcpy(memory_limit, argv[i + 1]);
                i++;
            }
            // If memory limit wasn't provided
            else{
                fprintf(stderr, "Error: memory limit wasn't provided\n");
                return 1;
            }
        }
        // If unknown flag
        else{
            fprintf(stderr, "Error: unknown flags given %s\n", argv[i]);
            return 1;
        }
    }

    // No --name flag, generate random name
    if(strcmp(container_name, "") == 0){    	
        srand(time(NULL));
        snprintf(container_name, sizeof(container_name),
                "mylxc-%d", rand() % 10000);
    }
    
    // Create and start container
    return cmd_run(argv[2], container_name, &ports, memory_limit, cpu_limit);
}

/**
 * stop_handler - Handle 'stop' command
 * @argc: Argument count
 * @argv: Argument vector
 * 
 * Parses arguments for the 'stop' command, validates them, and calls cmd_stop to 
 * stop the specified container(s).
 * 
 * Return: 0 on success, non-zero on failure
 */
int stop_handler(int argc, char **argv){
    // Check if a container name is given after "stop"
    if(argc < 3){
        fprintf(stderr, "Error: 'stop' requires container name\n");
        return 1;
    }

    // For each arg after "stop"
    int ret = 0;
    for(int i = 2; i < argc; i++){
        // Stop container
        ret = cmd_stop(argv[i]);

        // Check if error occur break out of loop
        if(ret != 0){
            break;
        }
    }

    return ret;
}

/**
 * rm_handler - Handle 'rm' command
 * @argc: Argument count
 * @argv: Argument vector
 * 
 * Parses arguments for the 'rm' command, validates them, and calls cmd_rm to 
 * remove the specified container(s).
 * 
 * Return: 0 on success, non-zero on failure
 */
int rm_handler(int argc, char **argv){
    // Check if a container name is given after "rm"
    if(argc < 3){
        fprintf(stderr, "Error: 'rm' requires container name\n");
        return 1;
    }

    // For each arg after "rm"
    int ret = 0;
    for(int i = 2; i < argc; i++){
        // Remove container
        ret = cmd_rm(argv[i]);

        // Check if error occured, if so break out of loop
        if(ret != 0){
            break;
        }
    }
    

    return ret;
}

/**
 * start_handler - Handle 'start' command
 * @argc: Argument count
 * @argv: Argument vector
 * 
 * Parses arguments for the 'start' command, validates them, and calls cmd_start to 
 * start the specified container(s).
 * 
 * Return: 0 on success, non-zero on failure
 */
int start_handler(int argc, char **argv){
    //Check if container name is given after "start"
    if(argc < 3){
        fprintf(stderr, "Error: 'start' requires container name\n");
        return 1;
    }

    // For each arg after "start"
    int ret = 0;
    for(int i = 2; i < argc; i++){
        // Start container
        ret = cmd_start(argv[i]);

        // Check if error has occured, if so break out of
        if(ret != 0){
            break;
        } 
    }

    return ret;
}

int main(int argc, char **argv){
    // Check if any command was provided
    if(argc < 2){
        print_usage();
        return 1;
    }
    
    // Get command for easier comparison
    const char *command = argv[1];  
    
    // If command is run
    if(strcmp(command, "run") == 0) return run_handler(argc, argv);
    
    // If command is ps
    if(strcmp(command, "ps") == 0) return cmd_ps();
    
    // If command is stop
    if(strcmp(command, "stop") == 0) return stop_handler(argc, argv);
    
    // If command is rm
    if(strcmp(command, "rm") == 0) return rm_handler(argc, argv);
    
    // If cmd is start
    if(strcmp(command, "start") == 0) return start_handler(argc, argv);
    
    // If command pass is not known
    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    print_usage();
    return 1;
}
