#include <stdio.h>
#include <string.h>
#include "image.h"
#include "util.h"

// Release getters 
// Ubuntu
const char* get_ubuntu_release(const char* version){
	if(strcmp(version, "24.04") == 0) return "noble";
	if(strcmp(version, "22.04") == 0) return "jammy";
	if(strcmp(version, "20.04") == 0) return "focal";
	if(strcmp(version, "18.04") == 0) return "bionic";
	return version;
}

// Debian
const char* get_debian_release(const char* version){
        if(strcmp(version, "12") == 0) return "bookworm";
        if(strcmp(version, "11") == 0) return "bullseye";
        return version;
} 


int parse_image(const char *img, image_info *info){	
	char version[64];
	
	// Parse image
	if(parse_col_seperator(img, info->distro, sizeof(info->distro), 
			            version, sizeof(version)) != 0){
		fprintf(stderr, "Error: couldn't find colon or buffer sizes too small\n");
		return -1;
	}

	// Get release name/number
	const char* release;
	
	// If ubuntu
	if(strcmp(info->distro, "ubuntu") == 0){
		release = get_ubuntu_release(version);
	}
	// If debian
	else if(strcmp(info->distro, "debian") == 0){
                release = get_debian_release(version);
        }
	// If alpine
	else{
		release = version;
	}

	strcpy(info->release, release);
	
	return 0;
}
