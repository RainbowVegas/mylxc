#ifndef JSON_H
#define JSON_H
#include "container.h"

// JSON functions
int store_container_info(const char *container_name, const char *container_ip, 
                         int *host_port, int *container_port);
int remove_container_info(const char *container_name);
int get_container_info(const char *container_name, char *container_ip, 
                       int *host_port, int *container_port);
int container_exists(const char *container_name);
int update_container_ip(const char *container_name);

// Constants

#endif // JSON_H