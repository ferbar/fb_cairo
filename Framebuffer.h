
class Framebuffer {
public:
	Framebuffer();
	~Framebuffer();
	char *getPointer();
	void paint();
	void putPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void putPixel(int x, int y, unsigned short rgb16);
	int fbfd;
	char *fbp;
	int xres;
	int yres;
	int xoffset;
	int yoffset;
	int bits_per_pixel;
	int line_length;
	size_t screensize;
};
