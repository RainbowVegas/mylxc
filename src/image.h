#ifndef IMAGE_H
#define IMAGE_H

// Structs
// Image information struct
typedef struct {
	char distro[64];
	char release[64];
} image_info;

// Image functions
int parse_image(const char *img, image_info *info);

// Constants

#endif
