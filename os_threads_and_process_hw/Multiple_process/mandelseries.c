/*
	brian Jimenez
	1001388904
*/
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include<sys/time.h>
#include<sys/wait.h>
#include<unistd.h>
#include"bitmap.h"
#define max 50
int main(int argc,char *argv[])
{
	//for time to calculate, got from git hub
	struct timeval begin;
  	struct timeval end;
	gettimeofday( &begin, NULL );
	//created an array that will store all the information we need to pass to the exec function
	char *all_info[50];
	all_info[0]="./mandel";
	all_info[1]="-x";
	all_info[2]="0.286932";
	all_info[3]="-y";
	all_info[4]="0.014287";
	all_info[5]="-s";
	all_info[6]="2";
	all_info[7]="-m";
	all_info[8]="2000";
	all_info[9]="-W";
	all_info[10]="1024";
	all_info[11]="-H";
	all_info[12]="1024";
	all_info[13]="-o";
	all_info[14]="mandel3.bmp";
	all_info[15]=NULL;

	//need to get the number of processess to run at same time from the user
	//need to convert it to an int
	int numb_of_process=atoi(argv[1]);
	//numb is the total numb of iterations it will go through all 50 times
	//need to do something about the remainders if its not divisible by 50
	int i,numb=max/numb_of_process;
	//since we have the number of process we can go through the whole thing and need
        //to divide it by 50 since thats the most we go through
	//count will be aued to store total times it produces an image
	int count=0;
	//keep tracks of all the files
	int numb_of_files=0;
	//array used to store the zoom since it will change every time
	char ss[20];
	//the 1st for loop will be incharge of going through all 50 depending how many we
	//do at the same time
	for(i=0;i<numb;i++)
	{
		//this inner for loop will be in charge of doing all the necessary things for those process that we run at same time
		//we will run at the same time
		int j;
		//count keeps track of all times the process are created. need a total of 50
		count+=numb_of_process;
		for(j=0;j<numb_of_process;j++)
		{
			numb_of_files++;
			float s=0;
			//if its the 1st file it will produce the big image
			//if not 1st image the zoom will change in size everytime
			if(numb_of_files==1)
			{
				s=2.00/1;
			}
			else
			{
				s=2.00/(numb_of_files+(1*(numb_of_files*230)));
			}
			//here converting the zoom to a string to be put in the exec
			sprintf(ss,"%f",s);
			//put in the array at postion 6
			all_info[6]=ss;
			//here is an array that will keep track of the number of files
			//will also be inserted in the array that will be passed to the exec function
			char pp[10];
			sprintf(pp,"%d",numb_of_files);
			char vv[100]="";
			strcat(vv,"mandel");
			strcat(vv,pp);
			strcat(vv,".bmp");
			all_info[14]=vv;
			//next line will do fork to have two processes running at the same time
			//if its the child process it will go in if statement in do the exec call
			pid_t pidd=fork();
			if(pidd==0)
			{
				//printf("im going to do something\n");
				execvp(all_info[0],all_info);
				printf("did it wonrg\n");
			}
			//wait(NULL);
			//pause();
		}
		int m;
		//this for loop is neccesary because each parent has to wait on their kid and so
		//we will fork depending on the number of process the user wants to do it.
		for(m=0;m<numb_of_process;m++)
		{
			//the parent will wait till the child is done
			int status;
			wait(&status);
		}
	}
	//this if statement will be useful in case there is any images left to be excuted, the "remainders"
	//if not all 50 are executed, will produce the image but one at a time
	if((50-count)!=0)
	{
		for(i=0;i<(50-count);i++)
		{
			numb_of_files++;
			float s=0;
			//if its the 1st file it will produce the big image
			//if not 1st image the zoom will change in size everytime
			if(numb_of_files==1)
			{
				s=2.00/1;
			}
			else
			{
				s=2.00/(numb_of_files+(1*(numb_of_files*230)));
			}
			//here converting the zoom to a string to be put in the exec
			sprintf(ss,"%f",s);
			//put in the array at postion 6
			//printf("%s\n",ss);
			//here is an array that will keep track of the number of files
			//will also be inserted in the array that will be passed to the exec function
			all_info[6]=ss;
			char pp[10];
			sprintf(pp,"%d",numb_of_files);
			char vv[100]="";
			strcat(vv,"mandel");
			strcat(vv,pp);
			strcat(vv,".bmp");
			all_info[14]=vv;
			//next line will do fork to have two processes running at the same time
			//if its the child process it will go in if statement in do the exec call
			pid_t pidd=fork();
			if(pidd==0)
			{
				//printf("im going to do something\n");
				execvp(all_info[0],all_info);
				printf("did it wonrg\n");
			}
			//wait(NULL);
			//pause();
		}
		int m;
		//this for loop is neccesary because each parent has to wait on their kid and so
		//we will fork depending on the number of process the user wants to do it.
		for(m=0;m<numb_of_process;m++)
		{
			//the parent will wait till the child is done
			int status;
			wait(&status);
		}
	}
	//else just continue and terminate
	else
	{

	}
	 gettimeofday( &end, NULL );

  	int time_duration = ( ( end.tv_sec - begin.tv_sec ) * 1000000 + ( end.tv_usec - begin.tv_usec ) );

	printf("Duration: %d\n", time_duration );
	return 0;

}
