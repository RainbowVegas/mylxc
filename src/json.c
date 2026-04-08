#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "network.h"
#include "json.h"
#include <cjson/cjson.h>

// Helper function to read JSON file and return root object
cJSON* get_json_root(){
    // Open existing JSON file
    FILE *fp = fopen("metadata.json", "rb");
    if(fp == NULL){
        fprintf(stderr, "Error: Unable to open the file\n");
        return NULL;
    }

    // Get length of file
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    // Move pointer back to front of file
    fseek(fp, 0, SEEK_SET);
    // Read data into buffer
    char *data = malloc(len + 1);
    fread(data, 1, len, fp);
    data[len] = '\0';
    fclose(fp);
    
    // Parse JSON file
    cJSON *root = cJSON_Parse(data);
    free(data);
    
    return root;  // Return root (NULL if parsing failed)
}

// Helper function to write JSON data back to file
int write_back_json(cJSON *root){
    // Write updated JSON back to file
    char *json_str = cJSON_Print(root);
    FILE *fp = fopen("metadata.json", "w");
    if(fp == NULL){
        fprintf(stderr, "Error: Unable to open the file\n");
        return 1;
    }
    fputs(json_str, fp);
    fclose(fp);
    free(json_str);
    return 0;
}

/**
 * store_container_info - Store container information in JSON file
 * @container_name: Name of the container
 * @container_ip: IP address of the container
 * @host_port: Host port number
 * @container_port: Container port number
 * 
 * This function retrieves the current JSON data, adds a new entry for the container with 
 * its IP address and port mappings, and writes the updated JSON back to the file. If the 
 * JSON file does not exist, it creates a new one.
 * 
 * Return: 0 on success, non-zero on failure
 */
int store_container_info(const char *container_name, const char *container_ip, 
                         int host_port, int container_port){
    // Get json root
    cJSON *root = get_json_root(); 
    // Create root object if it doesn't exist
    if(root == NULL){
        root = cJSON_CreateObject();
    }

    // Create container entry object
    cJSON *container = cJSON_CreateObject();
    cJSON_AddStringToObject(container, "ip_address", container_ip);
    cJSON_AddNumberToObject(container, "host_port", host_port);
    cJSON_AddNumberToObject(container, "container_port", container_port);

    // Add container to root object using container_name as key
    cJSON_AddItemToObject(root, container_name, container);

    // Write json back to file
    if(write_back_json(root) != 0){
        cJSON_Delete(root);
        return 1;
    }
    cJSON_Delete(root);
    
    return 0;
}

/**
 * remove_container_info - Remove container information from JSON file
 * @container_name: Name of the container to remove
 *
 * This function retrieves the current JSON data, removes the entry for the specified container, and writes the updated JSON back to the file.
 *
 * Return: 0 on success, non-zero on failure
 */
int remove_container_info(const char *container_name){
    // Get json root
    cJSON *root = get_json_root();
    if(root == NULL){
        fprintf(stderr, "Error: JSON file not found or empty\n");
        return 1;
    }

    // Remove container entry from root object
    cJSON_DeleteItemFromObject(root, container_name);

    // Write back json
    if(write_back_json(root) != 0){
        cJSON_Delete(root);
        return 1;
    }
    cJSON_Delete(root);

    return 0;
}

/**
* get_container_info - Retrieve container information from JSON file
* @container_name: Name of the container to retrieve information for
* @container_ip: Buffer to store the retrieved IP address
* @host_port: Buffer to store the retrieved host port
* @container_port: Buffer to store the retrieved container port
*
* This function retrieves the current JSON data, looks up the entry for the specified 
* container, and fills the provided buffers with the container's IP address and port 
* information. The port numbers are converted to strings before being stored in the 
* provided buffers.
*
* Return: 0 on success, non-zero on failure
*/
int get_container_info(const char *container_name, char *container_ip, 
                       int *host_port, int *container_port){
    // Get json root
    cJSON *root = get_json_root();
    if(root == NULL){
        fprintf(stderr, "Error: JSON file not found or empty\n");
        return 1;
    }

    // Get container entry from root object
    cJSON *container = cJSON_GetObjectItem(root, container_name);
    if(container == NULL){
        fprintf(stderr, "Error: Container not found in JSON\n");
        cJSON_Delete(root);
        return 1;
    }

    // Get IP address and port mappings
    cJSON *ip_item = cJSON_GetObjectItem(container, "ip_address");
    cJSON *host_port_item = cJSON_GetObjectItem(container, "host_port");
    cJSON *container_port_item = cJSON_GetObjectItem(container, "container_port");

    // Check if all required fields are present
    if(ip_item == NULL || host_port_item == NULL || container_port_item == NULL){
        fprintf(stderr, "Error: Missing fields in JSON for container\n");
        cJSON_Delete(root);
        return 1;
    }

    // Copy container information to provided buffers
    strcpy(container_ip, ip_item->valuestring);
    *host_port = host_port_item->valueint;
    *container_port = container_port_item->valueint;

    cJSON_Delete(root);
    return 0;
}

/**
 * container_exists - Check if a container entry exists in the JSON file
 * @container_name: Name of the container to check
 *
 * This function retrieves the current JSON data and checks if an entry for the specified container exists.
 *
 * Return: 1 if the container exists, 0 if it does not exist or if there was an error
 */
int container_exists(const char *container_name){
    // Get json root
    cJSON *root = get_json_root();
    if(root == NULL){
        fprintf(stderr, "Error: JSON file not found or empty\n");
        return 0;
    }

    // Check if container entry exists in root object
    cJSON *container = cJSON_GetObjectItem(root, container_name);
    int exists = (container != NULL);
    cJSON_Delete(root);
    
    return exists;

}

/** 
 * update_container_ip - Update the IP address of a container in the JSON file
 * @container_name: Name of the container to update
 *
 * This function retrieves the current JSON data, updates the IP address for the specified container, 
 * and writes the updated JSON back to the file.
 *
 * Return: 0 on success, non-zero on failure
 */
int update_container_ip(const char *container_name){
    // Get json root
    cJSON * root = get_json_root();
    if(root == NULL){
        fprintf(stderr, "Error: JSON file not found or empty\n");
        return 1;
    }

    // Get container entry from root object
    cJSON *container = cJSON_GetObjectItem(root, container_name);
    if(container == NULL){
        fprintf(stderr, "Error: Container not found in JSON\n");
        cJSON_Delete(root);
        return 1;
    }

    // Get new IP address of container
    char *new_container_ip = get_ip(container_name);
    if(new_container_ip == NULL){
        return 1;
    }

    // Update IP address in json
    cJSON *old_container_ip = cJSON_GetObjectItem(container, "ip_address");
    cJSON_SetStringValue(old_container_ip, new_container_ip);

    // Write back json
    if(write_back_json(root) != 0){
        cJSON_Delete(root);
        free(new_container_ip);
        return 1;
    }
    
    cJSON_Delete(root);
    free(new_container_ip);

    return 0;
}