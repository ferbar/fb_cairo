#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Framebuffer.h"
// imagemagick 6
// #include <magick/ImageMagick.h>
// #include <magick/MagickCore.h>
#include <string>
#include "utils.h"
#include <iostream>

#include <time.h>

#include <cairomm/context.h>
#include <cairomm/cairomm.h>

// https://developer.gnome.org/gtkmm-tutorial/stable/sec-drawing-clock-example.html.en
bool cairo_clock(const Cairo::RefPtr<Cairo::Context>& cr)
{
	const int width = 100;
	const int height = 100;
	
	const double m_radius = 0.42;
	const double m_line_width=0.05;

	cr->translate(320-100 -10, 240-100 -10);

	printf("draw clock\n");
	// scale to unit square and translate (0, 0) to be (0.5, 0.5), i.e.
	// the center of the window
	cr->scale(width, height);
	cr->translate(0.5, 0.5);
	cr->set_line_width(m_line_width);


	/* fill green background
	cr->save();
	cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
	cr->paint();
	cr->restore();
	*/
	
	cr->arc(0, 0, m_radius, 0, 2 * M_PI);

	cr->save();
	cr->set_source_rgba(0.612, 0.337, 0.117, 0.9);   // red
	cr->fill_preserve();
	cr->restore();
	cr->stroke_preserve();
	cr->clip();



	//clock ticks
	for (int i = 0; i < 12; i++)
	{
		double inset = 0.05;

		cr->save();
		cr->set_line_cap(Cairo::LINE_CAP_ROUND);

		if(i % 3 != 0)
		{
			inset *= 0.8;
			cr->set_line_width(0.03);
		}

		cr->move_to(
				(m_radius - inset) * cos (i * M_PI / 6),
				(m_radius - inset) * sin (i * M_PI / 6));
		cr->line_to (
				m_radius * cos (i * M_PI / 6),
				m_radius * sin (i * M_PI / 6));
		cr->stroke();
		cr->restore(); // stack-pen-size
	}
	
	// store the current time
	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime (&rawtime);

	// compute the angles of the indicators of our clock
	double minutes = timeinfo->tm_min * M_PI / 30;
	double hours = timeinfo->tm_hour * M_PI / 6;
	double seconds= timeinfo->tm_sec * M_PI / 30;

	cr->save();
	cr->set_line_cap(Cairo::LINE_CAP_ROUND);

	// draw the seconds hand
	cr->save();
	cr->set_line_width(m_line_width / 3);
	cr->set_source_rgba(0.7, 0.7, 0.7, 0.8); // gray
	cr->move_to(0, 0);
	cr->line_to(sin(seconds) * (m_radius * 0.9),
			-cos(seconds) * (m_radius * 0.9));
	cr->stroke();
	cr->restore();


	// draw the minutes hand
	cr->set_source_rgba(0.117, 0.337, 0.612, 0.9);   // blue
	cr->move_to(0, 0);
	cr->line_to(sin(minutes + seconds / 60) * (m_radius * 0.8),
			-cos(minutes + seconds / 60) * (m_radius * 0.8));
	cr->stroke();

	// draw the hours hand
	cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
	cr->move_to(0, 0);
	cr->line_to(sin(hours + minutes / 12.0) * (m_radius * 0.5),
			-cos(hours + minutes / 12.0) * (m_radius * 0.5));
	cr->stroke();
	cr->restore();


	// draw a little dot in the middle
	cr->arc(0, 0, m_line_width / 3.0, 0, 2 * M_PI);
	cr->fill();

	return true;
}


int main(int argc, char*argv[]) {

	printf("argc=%d\n", argc);
	if(argc <= 3) {
		printf("Error: string missing\n");
		exit(1);
	}

	int width=320;
	// int line_length=width;
	int height=240;
	Framebuffer fb;
	// fb.paint();

	/*
	Magick::PixelPacket pipa;
	pipa.red=QuantumRange;
	pipa.green=QuantumRange;
	pipa.blue=QuantumRange;
	*/

	timespec tspec;
	clock_gettime(CLOCK_MONOTONIC, &tspec);
	double start=tspec.tv_sec + tspec.tv_nsec/1000000000.0;

	Cairo::RefPtr<Cairo::ImageSurface> surface =
		Cairo::ImageSurface::create(Cairo::FORMAT_RGB16_565, width, height);
	Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);

	// cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	// cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_RGB16_565, width, height);
	// cairo_t *cr = cairo_create(surface);
	// Fill everything with black
	cr->set_source_rgb(0, 0, 0);
	cr->paint();

	
	clock_gettime(CLOCK_MONOTONIC, &tspec);
	double gate1=tspec.tv_sec + tspec.tv_nsec/1000000000.0;
	
	cr->select_font_face("arial", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
	cr->set_font_size(30);
	cr->set_source_rgb(1, 1, 1);
	cr->move_to(5, 35);
	cr->show_text(argv[1]);

	// SyncAuthenticPixels(image, exception);

	clock_gettime(CLOCK_MONOTONIC, &tspec);
	double gate2=tspec.tv_sec + tspec.tv_nsec/1000000000.0;

	cr->move_to(5, 65);
	cr->show_text(argv[2]);
	cr->move_to(5, 95);
	cr->show_text(argv[3]);
	cr->stroke();

	cairo_clock(cr);

	clock_gettime(CLOCK_MONOTONIC, &tspec);
	// surface->surface_flush();
	unsigned short * pixels = (unsigned short *) surface->get_data();
	double gate3=tspec.tv_sec + tspec.tv_nsec/1000000000.0;
	for(int y=0; y < height; y++) {
		for(int x=0; x < width; x++) {
			// printf("%04x ", pixels[x+y*line_length]);
			fb.putPixel(x,y, pixels[x + y*width]);
			// printf("%04x ", (pix.red >> (8+3)) << 11 | (pix.green >> (8+2)) << 5 | (pix.blue >> (8+3)) );
			//fb.putPixel(x,y,pix.redQuantum() >> 8, pix.greenQuantum() >> 8, pix.blueQuantum() >> 8);
		}
		// printf("\n");
	}


	clock_gettime(CLOCK_MONOTONIC, &tspec);
	double gate4=tspec.tv_sec + tspec.tv_nsec/1000000000.0;
	printf("gate1: %gs\n", gate1-start);
	printf("gate2: %gs\n", gate2-gate1);
	printf("gate3: %gs\n", gate3-gate2);
	printf("gate4: %gs\n", gate4-gate3);

	// cairo_surface_destroy(surface);
	// cairo_destroy(cr);
}
