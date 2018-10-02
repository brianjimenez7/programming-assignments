/*
	brian jimenez
	1001388904
*/
#include "bitmap.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/time.h>
//Added this struct so that i add all the information needed to compute the image
//also added the total threads along with specif thread to divide the work done by each thread
struct info
{
	double xmin,xmax,ymin,ymax;
	int max,total_threads,current_thread;
	float scale;
	struct bitmap *bm;
};

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void *compute_image(void *all_info);

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	//added this new one since we are asking for threads now
	printf("-n <threads> The total number of threads you want (default=1)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");

}

int main( int argc, char *argv[] )
{
	struct timeval begin;
  	struct timeval end;

  	gettimeofday( &begin, NULL );
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	//added a new variable to determine the number of threads
	//if no number of threads is given, 1 thread will de automatic
	int  number_of_threads=1;


	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"n:x:y:s:W:H:m:o:h"))!=-1) {
		switch(c) {

			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				printf("goes in here\n");
				show_help();
				exit(1);
				break;
			//added new parameter as well for the number of threads
			//will convert the number of threads to an int from a string
			case 'n':
				number_of_threads=atoi(optarg);
				break;
		}
	}
	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);
	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));
	//now since i have to mess with threads, i have to initilize how many threads i want
	//will use the variable number_of_threads to determine that
	pthread_t threads[number_of_threads];
	//initialized a the struct that i have created
	//and have a struct for each thread so created an array of that struct
	struct info all_information[number_of_threads];
	int i;
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s number of threads=%d\n",xcenter,ycenter,scale,max,outfile,number_of_threads);
	//within this for loop will add all the information such as xmin,xmax etc.
	//to the struct that contains all those info to be passed to the compute image where each thread
	//needs all that info to produce part of the image
	//its important to have a struct bitmap in the struct bc most functions use that type of struct
	//also important to pass the specif thread it is to divide the task
	for(i=0;i<number_of_threads;i++)
	{
		//add the info to the struct that i have created
		//all the info the right is same from the old parameters compute_image had
		all_information[i].xmin=xcenter-scale;
		all_information[i].xmax=xcenter+scale;
		all_information[i].ymin=ycenter-scale;
		all_information[i].ymax=ycenter+scale;
		all_information[i].max=max;
		all_information[i].bm=bm;
		//these are new things i added to determine what each thread will do what height
		all_information[i].current_thread=i;
		all_information[i].total_threads=number_of_threads;
		//here i am sending each thread to compute the image
		//each thread will contain the same info but will produce part of the image
		//only thing that changes is the current thread which will be used to determine
		//what part of the image that thread will do
		pthread_create(&threads[i],NULL,compute_image,(void *)&all_information[i]);
	}

	// Display the configuration of the image.
	int ii;
	for(ii=0;ii<number_of_threads;ii++)
	{
		//added pthread_join because it will suspend execution for that thread
		//Will wait until its done
		//this is simialr to when you have to wait in when forking
		pthread_join(threads[ii],NULL);
	}



	// Compute the Mandelbrot image
	//compute_image(bm,xcenter-scale,xcenter+scale,ycenter-scale,ycenter+scale,max);

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}
	gettimeofday( &end, NULL );

 	int time_duration = ( ( end.tv_sec - begin.tv_sec ) * 1000000 + ( end.tv_usec - begin.tv_usec ) );

  	printf("Duration: %d\n", time_duration );
	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/
//add an integer of threads to the struct
void *compute_image(void *all_info)
{
	int i,j;
	//created another struct and set it equal to the one i passed through
	struct info *in_function_info=(struct info*)all_info;
	//here getting the total threads and the current thread
	//will be using total an current to assign each thread a range of height
	int total=in_function_info->total_threads;
	int current=in_function_info->current_thread;
	struct bitmap *bm=in_function_info->bm;
	int width = bitmap_width(bm);
	int height = bitmap_height(bm);
	//range will be used to split how the work between each thread
	int range=height/total;
	//numb_start is where that specific thread will start in height
	int numb_start=current*range;
	//numb_stop is used to see where that specific thread will end in height
	int numb_stop=(current+1)*range;
	int check=0;
	//this if statement is used to see if its the last thread or not
	if(current==(total-1))
	{
		//will go in here if the numb_stop and height is not same, means that
		//the last thread has to do the rest of the image since we need all the image
		if(numb_stop!=height)
		{
			check=1;
		}
	}

	// For every pixel in the image...
	//if its not the last thread or if the last thread is on track will go in here
	//and produce that specific part of that image
	if(check==0)
	{
		for(j=numb_start;j<numb_stop;j++) {

			for(i=0;i<width;i++) {

				// Determine the point in x,y space for that pixel.
				double x = in_function_info->xmin + i*(in_function_info->xmax-in_function_info->xmin)/width;
				double y = in_function_info->ymin + j*(in_function_info->ymax-in_function_info->ymin)/height;

				// Compute the iterations at that point.
				int iters = iterations_at_point(x,y,in_function_info->max);
				//printf("iters is: %d\n",iters);
				// Set the pixel in the bitmap.
				bitmap_set(bm,i,j,iters);
			}
		}
	}
	//else the last thread will go in here and finish the whole height that is left
	else
	{
		for(j=numb_start;j<height;j++)
		{
			for(i=0;i<width;i++) {

				// Determine the point in x,y space for that pixel.
				double x = in_function_info->xmin + i*(in_function_info->xmax-in_function_info->xmin)/width;
				double y = in_function_info->ymin + j*(in_function_info->ymax-in_function_info->ymin)/height;

				// Compute the iterations at that point.
				int iters = iterations_at_point(x,y,in_function_info->max);
				//printf("iters is: %d\n",iters);
				// Set the pixel in the bitmap.
				bitmap_set(bm,i,j,iters);
			}
		}
	}
	return 0;
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}
