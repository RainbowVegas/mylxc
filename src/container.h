#ifndef CONTAINER_H
#define CONTAINER_H

// Structs 
typedef struct{
        int host_port;
        int container_port;
        int has_port;
} port_mapping;

// Container Functions
int cmd_run(const char *img, const char *container_name, port_mapping *ports);
int cmd_ps();
int cmd_stop(const char *container_name);
int cmd_rm(const char *container_name);

// Constants

#endif // CONTAINER_H
