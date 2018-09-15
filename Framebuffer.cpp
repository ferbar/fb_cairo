#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "Framebuffer.h"
#include <assert.h>

Framebuffer::Framebuffer()
	: fbp(NULL), xres(0), yres(0), bits_per_pixel(0), screensize(0)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;

    // Open the file for reading and writing
    this->fbfd = open("/dev/fb1", O_RDWR);
    if (this->fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(this->fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(this->fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
	this->xres=vinfo.xres;
	this->yres=vinfo.yres;
	this->bits_per_pixel=vinfo.bits_per_pixel;
	this->xoffset=vinfo.xoffset;
	this->yoffset=vinfo.yoffset;
	this->line_length=finfo.line_length;


    // Figure out the size of the screen in bytes
    //screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	this->screensize = finfo.smem_len;

    // Map the device to memory
    this->fbp = (char *)mmap(0, this->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, this->fbfd, 0);
    if (fbp == MAP_FAILED) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }
    printf("The framebuffer device was mapped to memory successfully.\n");
}

char *Framebuffer::getPointer() {
	return this->fbp;
}

void Framebuffer::paint() {
	// Figure out where in memory to put the pixel
	for (int y = 0; y < this->yres; y++) {
		for (int x = 0; x < this->xres; x++) {
			int b = (x*y)*256/(this->xres*this->yres);
			int g = 0; // x*256/this->xres;     // A little green
			int r = 0; // y*256/this->yres;    // A lot of red
			// printf("%d.%d %02x%02x%02x \n", x,y,r,g,b);
			this->putPixel(x,y,r,g,b);
		}
	}
}

void Framebuffer::putPixel(int x, int y, unsigned short rgb16) {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < this->xres);
	assert(y < this->yres);

	long int location = (x+this->xoffset) * 2 +
			(y+this->yoffset) * this->line_length;
	unsigned short int *p=((unsigned short int*)(this->fbp + location));
	*p=rgb16;
}

void Framebuffer::putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < this->xres);
	assert(y < this->yres);
	if (this->bits_per_pixel == 32) {
		long int location = (x+this->xoffset) * (this->bits_per_pixel/8) +
				   (y+this->yoffset) * this->line_length;
		*(this->fbp + location) = b;
		*(this->fbp + location + 1) = g;
		*(this->fbp + location + 2) = r;
		*(this->fbp + location + 3) = 0; // transparency
	} else if(this->bits_per_pixel == 16) {
		long int location = (x+this->xoffset) * 2 +
			(y+this->yoffset) * this->line_length;
		r >>=3;
		g >>=2;
		b >>=3;
		unsigned short int t = r<<11 | g << 5 | b;
		unsigned short int *p=((unsigned short int*)(this->fbp + location));
		//if(t != 0)
		//if(*p != t) {
			*p=t;
		//}
	} else {
		exit(1);
	}
}

Framebuffer::~Framebuffer() {
    munmap(fbp, screensize);
    close(fbfd);
}

