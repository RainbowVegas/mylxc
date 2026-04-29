#ifndef NETWORK_H
#define NETWORK_H

// Network functions
int setup_port_mapping(const char *container_name, int host_port, int container_port);
char* get_ip(const char *container_name);
int iptable_cleanup(const char *container_name);

// Constants

#endif // NETWORK_H
