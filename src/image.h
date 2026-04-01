#ifndef IMAGE_H
#define IMAGE_H

typedef struct {
	char distro[64];
	char release[64];
} image_info;

int parse_image(const char *img, image_info *info);

#endif
