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

int main(int argc, char **argv){
    
    // Check if any command was provided
    if(argc < 2){
        print_usage();
        return 1;
    }
    
    const char *command = argv[1];  // Get command for easier comparison
    int cmd_ret = 0;                // Holds return values from each cmd
    
    // If command is run
    if(strcmp(command, "run") == 0){
        // Validate image argument
        if(argc < 3){
            fprintf(stderr, "Error: 'run' requires more info\n");
            return 1;
        }
        
        // Variables for optional flags
            char container_name[128] = "";
        port_mapping ports = {0,0,0};
        
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
            // Check for --p flag
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
        cmd_ret = cmd_run(argv[2], container_name, &ports);

        return cmd_ret;
    }
    
    // If command is ps
    else if(strcmp(command, "ps") == 0){
        return cmd_ps();
    }
    
    // If command is stop
    else if(strcmp(command, "stop") == 0){
        // Check if a container name is given after "stop"
        if(argc < 3){
            fprintf(stderr, "Error: 'stop' requires container name\n");
            return 1;
        }

        // For each arg after "stop"
        for(int i = 2; i < argc; i++){
            // Stop container
            cmd_ret = cmd_stop(argv[i]);

            // Check if error occur break out of loop
            if(cmd_ret != 0){
                break;
            }
        }

        return cmd_ret;
    }
    
    // If command is rm
    else if(strcmp(command, "rm") == 0){
        // Check if a container name is given after "rm"
        if(argc < 3){
            fprintf(stderr, "Error: 'rm' requires container name\n");
            return 1;
        }

        // For each arg after "rm"
        for(int i = 2; i < argc; i++){
            // Remove container
            cmd_ret = cmd_rm(argv[i]);

            // Check if error occured, if so break out of loop
            if(cmd_ret != 0){
                break;
            }
        }
        

        return cmd_ret;
    }
    
    // If cmd is start
    else if(strcmp(command, "start") == 0){
        //Check if container name is given after "start"
        if(argc < 3){
            fprintf(stderr, "Error: 'start' requires container name\n");
            return 1;
        }

        // For each arg after "start"
        for(int i = 2; i < argc; i++){
            // Start container
            cmd_ret = cmd_start(argv[i]);

            // Check if error has occured, if so break out of
            if(cmd_ret != 0){
                break;
            } 
        }

        return cmd_ret;
    }

    // If command pass is not known
    else{
        fprintf(stderr, "Error: Unknown command '%s'\n", command);
        print_usage();
        return 1;
    }
}
