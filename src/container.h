#ifndef CONTAINER_H
#define CONTAINER_H

// Structs 
// Port mapping struct
typedef struct{
        int host_port;          // Port on the host machine
        int container_port;     // Port inside the container
        int has_port;           // Flag to indicate if port mapping is set
} port_mapping;

// Container Functions
int cmd_run(const char *img, const char *container_name, port_mapping *ports,
            const char *memory_limit, int cpu_limit);
int cmd_ps();
int cmd_stop(const char *container_name);
int cmd_rm(const char *container_name);
int cmd_start(const char *container_name); //New

// Constants

#endif // CONTAINER_H
