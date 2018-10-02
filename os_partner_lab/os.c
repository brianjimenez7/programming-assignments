// The MIT License (MIT)
//
// Copyright (c) 2016, 2017 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdint.h>
#include<strings.h>
#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments
int i,j;
char BS_OEMName[8];
int16_t BPB_RootEntCnt;
char BS_VolLab[11];
int32_t BPB_RootClus;
int32_t RootDirSectors = 0;
int32_t FirstDataSector = 0;
int32_t FirstSectorofCluster = 0;
int16_t B_P_S;
int16_t B_P_S_copy;
int8_t S_P_C;
int8_t S_P_C_copy;
int16_t R_S_C;
int16_t R_S_C_copy;
int8_t N_FATS;
int8_t N_FATS_copy;
int32_t F_Z_32;
int32_t F_Z_32_copy;
char DIR_VOL_NAME[11];
FILE *fp;
int current_num = 0;
int check_num = 0;
char checking_files[10][12];
struct __attribute__((__packed__)) DirectoryEntry
{
    char DIR_NAME[11];
    uint8_t DIR_Attr;
    u_int8_t Unused1[8];
    u_int16_t DIR_FirstClusterHigh;
    u_int8_t Unused2[4];
    uint16_t DIR_FirstClusterLow;
    uint32_t DIR_FileSize;
};
struct DirectoryEntry dir[16];
int LBAToOffset(int32_t sector)
{
  return ((sector-2) * B_P_S) + (B_P_S * R_S_C)+(N_FATS * F_Z_32 * B_P_S);
}
int16_t NextLB(uint32_t sector)
{
  uint32_t FATAdderess = (B_P_S * R_S_C) + (sector * 4);
  int16_t val;
  fseek(fp, FATAdderess, SEEK_SET);
  fread(&val, 2 , 1, fp);
  return val;
}
int slashes(char *arg)
{
  int x;
  int num = 1;
  for(x = 0; x < strlen(arg); x++)
  {
      if(arg[x] == '/')
      {
        num++;
      }
  }
  return num;
}
void put_together_3(char *files, char *input, int dir_pos,int* checker,int *current_dir)
{
  int n = strlen(files);
	j = 0;
	char ch1[n];
  char new_char[n+1];
	int k;
	int stop = 0;
  int space = 0;
	for(k = 0; k < 12; k++)
	{
    if(files[k]==' ')
    {
      space=1;
    }
		if(files[k]!=' ')
		{
			ch1[j] = files[k];
			j++;

		}
		else
		{
      space = 1;
      if(stop == 0)
			{
				ch1[j] = '.';
				stop = 1;
				j++;
			}
		}
	}
  if(space==0)
  {
    int y;
    int l = 0;
    int size = n+1;
    //printf("size is : %d %d\n",strlen(files),n);
    //printf("file name is:*%s*\n",files);
    for(y = 0; y<size; y++)
    {
      if(size-y == 4)
      {
        new_char[y] = '.';
      }
      else
      {
        new_char[y] = files[l];
        l++;
      }
    }
    if(strcasecmp(new_char,input)==0)
    {
      *checker=1;
    }
    //exit(0);
  }
  else
  {
    if(strcasecmp(ch1,input)==0)
    {
      *checker=1;
    }
  }
}
void put_together_2(char *files, char *input, int dir_pos,int* checker)
{
  int n = strlen(files);
	j = 0;
	char ch1[n];
  char new_char[n+1];
	int k;
	int stop = 0;
  int space = 0;
	for(k = 0; k < 12; k++)
	{
    if(files[k]==' ')
    {
      space=1;
    }
		if(files[k]!=' ')
		{
			ch1[j] = files[k];
			j++;

		}
		else
		{
      space = 1;
      if(stop == 0)
			{
				ch1[j] = '.';
				stop = 1;
				j++;
			}
		}
	}
  if(space==0)
  {
    int y;
    int l = 0;
    int size = n+1;
    //printf("size is : %d %d\n",strlen(files),n);
    //printf("file name is:*%s*\n",files);
    for(y = 0; y<size; y++)
    {
      if(size-y == 4)
      {
        new_char[y] = '.';
      }
      else
      {
        new_char[y] = files[l];
        l++;
      }
    }
    if(strcasecmp(new_char,input)==0)
    {
      *checker=1;
      printf("attribute = %d\n", dir[dir_pos].DIR_Attr);
      printf("DIR_FirstClusterLow = %d\n",dir[dir_pos].DIR_FirstClusterLow);
      printf("DIR_FileSize = %d\n", dir[dir_pos].DIR_FileSize);
    }
    //exit(0);
  }
  else
  {
    if(strcasecmp(ch1,input)==0)
    {
      *checker=1;
      printf("attribute = %d\n", dir[dir_pos].DIR_Attr);
      printf("DIR_FirstClusterLow = %d\n",dir[dir_pos].DIR_FirstClusterLow);
      printf("DIR_FileSize = %d\n", dir[dir_pos].DIR_FileSize);
    }
  }
  /*int n = sizeof(files);
	j = 0;
	char ch1[n];
	int k;
	int stop = 0;
	for(k = 0; k < 12; k++)
	{
		if(files[k]!=' ')
		{
			ch1[j] = files[k];
			j++;
		}
		else
		{
      if(stop == 0)
			{
				ch1[j] = '.';
				stop = 1;
				j++;
			}
		}
	}
  if(strcasecmp(ch1,input)==0)
  {
    *checker=1;
    printf("attribute = %d\n", dir[dir_pos].DIR_Attr);
    printf("DIR_FirstClusterLow = %d\n",dir[dir_pos].DIR_FirstClusterLow);
    printf("DIR_FileSize = %d\n", dir[dir_pos].DIR_FileSize);
  }*/

}
void put_together(char *files)
{
	int n = strlen(files);
	j = 0;
	char ch1[n];
  char new_char[n+1];
	int k;
	int stop = 0;
  int space = 0;
	for(k = 0; k < 12; k++)
	{
    if(files[k]==' ')
    {
      space=1;
    }
		if(files[k]!=' ')
		{
			ch1[j] = files[k];
			j++;

		}
		else
		{
      space = 1;
      if(stop == 0)
			{
				ch1[j] = '.';
				stop = 1;
				j++;
			}
		}
	}
  if(space==0)
  {
    int y;
    int l = 0;
    int size = n+1;
    //printf("size is : %d %d\n",strlen(files),n);
    //printf("file name is:*%s*\n",files);
    for(y = 0; y<size; y++)
    {
      if(size-y == 4)
      {
        new_char[y] = '.';
      }
      else
      {
        new_char[y] = files[l];
        l++;
      }
    }
    printf("%s ", new_char);
    //exit(0);
  }
  else
  {
	    printf("%s ", ch1);
  }

}
int main()
{
  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  int set = 0;
  int root_dir;
  int back_dir[100];
  int dir_counter = 0;
  int current_dir = 0;
  int low_dir;
  int dot_dir;
  int correct = 0;
  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    if(token[0] == NULL)
    {

	continue;


    }
    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality

    if((strcasecmp(token[0],"exit")==0))
    {
	exit(0);

    }


    if(strcasecmp(token[0],"open")== 0)
    {
	     if(token[1]==NULL)
	     {
		        printf("Error File System not here(change later)\n");
		          continue;
	     }
	     if(set == 1)
	     {
		       printf("Error File System open already(change later)\n");
		         continue;
	     }
	fp = fopen(token[1],"r");
	if(fp==NULL)
	{
		printf("Error File System not here actually checked in the file system-(change later)\n");
	}
	else
	{
		set = 1;
		fseek(fp, 11, SEEK_SET);
		fread(&B_P_S,1,2,fp);

		fseek(fp, 13, SEEK_SET);
		fread(&S_P_C,1,1,fp);

		fseek(fp, 14, SEEK_SET);
		fread(&R_S_C,1,2,fp);

		fseek(fp, 16, SEEK_SET);
		fread(&N_FATS,1,1,fp);

		fseek(fp, 36, SEEK_SET);
		fread(&F_Z_32,1,4,fp);

    fseek(fp,71,SEEK_SET);
    fread(&DIR_VOL_NAME,1,11,fp);

		//calculating the address of the root_dir and storing said value
		root_dir = (N_FATS*F_Z_32*B_P_S)+(R_S_C*B_P_S);
		current_dir = root_dir;
		low_dir = root_dir;
		fseek(fp, root_dir, SEEK_SET);
		memset(&dir,0,16*sizeof(struct DirectoryEntry));
		for(i = 0; i < 16; i++)
		{
			fread(&dir[i],1,32,fp);
		}
		memset(&back_dir,0,100);
	 }

   	}
	if((strcasecmp(token[0],"info"))==0)
	{
		if(set == 0)
		{
			printf("File System Closed\n");
		}
		else
		{
			            printf("BPB_BytsPersec: %d\n", B_P_S);
                	printf("BPB_BytsPersec: %x\n\n", B_P_S);

                	printf("BPB_SecPerClus: %d\n", S_P_C);
                	printf("BPB_SecPerClus: %x\n\n", S_P_C);

                	printf("BPB_RsvdSecCnt: %d\n", R_S_C);
                	printf("BPB_RsvdSecCnt: %x\n\n", R_S_C);

                	printf("BPB_NumFATS: %d\n", N_FATS);
                	printf("BPB_NumFATS: %x\n\n", N_FATS);

                	printf("BPB_FATSz32: %d\n", F_Z_32);
                	printf("BPB_FATSz32: %x\n\n", F_Z_32);
		}
	}
	if((strcasecmp(token[0],"close"))==0)
	{

		if(set == 0)
		{
			printf("file system already closed\n");
		}
		else
		{
			set = 0;
			fclose(fp);

		}

	}

	if((strcasecmp(token[0],"ls"))==0)
	{
		char files[12];
		if(set == 0)
		{
			printf("file system is not open\n");
			continue;

		}
		if(token[1] == NULL)
		{
			for(i = 0; i < 16; i++)
			{
				if(dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)
				{
					strncpy(files,dir[i].DIR_NAME,11);
					files[11] = '\0';
					if(dir[i].DIR_Attr == 0x10 )
					{
						if(files[0] != -27)
						{
							int u;
							for(u = 0; u < 12; u++)
							{
								if(files[u] == ' ')
								{
									files[u] = '\0';
								}
							}
							printf("%s ",files);
							//strncpy(current_files[current_num],files,11);
							current_num++;
						}
					}
					else
					{
						if(files[0] != -27)
						{
							put_together(files);
						}
					}

				}

			}
			printf("\n");


		}
		else if(strcasecmp(token[1],"..")==0)
		{
        int h;
        char input_dir[11];
        char checker[11];
        printf("here\n");
        for(h = 0; h<16; h++)
        {
                  strncpy(checker,dir[h].DIR_NAME,11);
  				        checker[11] = '\0';
  				        for(j = 0; j < 12; j++)
  				        {
  					             if(checker[j] == ' ')
  					             {
  						                //null terminaing the string in the files system for later comparison
  						                checker[j] = '\0';
  					             }
  				        }
                  if(strcasecmp(token[1],checker)==0)
                  {
                    int offset = 0;
                    if(dir[h].DIR_FirstClusterLow == 0)
                    {
                      offset = LBAToOffset(2);
                    }
                    else
                    {
                      current_dir = dir[h].DIR_FirstClusterLow;
                      offset = LBAToOffset(current_dir);
                    }

                    fseek(fp, offset, SEEK_SET);
        						int w;
        						for(w = 0; w < 16; w++)
        						{
        							memset(&dir[w].DIR_NAME, 0, 11);
        							fread(&dir[w],1,32,fp);
        						}

                    //printing under here
                    for(i = 0; i < 16; i++)
                    {
                      if(dir[i].DIR_Attr == 0x01 || dir[i].DIR_Attr == 0x10 || dir[i].DIR_Attr == 0x20)
                      {
                        strncpy(files,dir[i].DIR_NAME,11);
                        files[11] = '\0';
                        if(dir[i].DIR_Attr == 0x10 )
                        {
                          if(files[0] != -27)
                          {
                            int u;
                            for(u = 0; u < 12; u++)
                            {
                              if(files[u] == ' ')
                              {
                                files[u] = '\0';
                              }
                            }
                            printf("%s ",files);
                            //strncpy(current_files[current_num],files,11);
                            current_num++;
                          }
                        }
                        else
                        {
                          if(files[0] != -27)
                          {
                            put_together(files);
                          }
                        }

                      }

                    }
                    printf("\n");
                    //end here

                    //set back here
                    fseek(fp, dot_dir, SEEK_SET);
        						int t;
        						for(t = 0; t < 16; t++)
        						{
        							memset(&dir[t].DIR_NAME, 0, 11);
        							fread(&dir[t],1,32,fp);
        						}

                    //end setback



                  }


        }
    }
  }
	//if the user inputs cd this will change the dir.
	if(strcasecmp(token[0],"cd") == 0)
	{
    if(set==0)
    {
      printf("FILE systm is not yet opened!\n");
      continue;
    }
    else
    {
      if(token[1]==NULL)
      {
        int no_offset=LBAToOffset(2);
        fseek(fp, no_offset, SEEK_SET);
        int e;
        for(e = 0; e < 16; e++)
        {
          memset(&dir[e].DIR_NAME, 0, 11);
          fread(&dir[e],1,32,fp);
        }
        continue;

      }
      else
      {

        int num_dirs = slashes(token[1]);
        int arg_position = 0;
        int number = 0;

  while(num_dirs > 0)
  {
        //printf("here\n");
        //printf("%d\n",num_dirs);
        int b;
        char input[12];
        memset(&input,0,12);
        int pos = 0;
        for(b = arg_position; b < strlen(token[1]); b++)
        {
          if(token[1][b] == '/')
          {
            arg_position = b+1;
            break;
          }
          else
          {
              input[pos] = token[1][b];
              //printf("%c\n",token[1][b]);
              pos++;
          }

        }
        //printf("%d ) %s\n",num_dirs,input);
        char new_dir[11];
        char check_file[12];
  			char input_file[12];
  			strcpy(input_file,token[1]);
  			for(i = 0; i < 16; i++)
  			{
  				strncpy(check_file,dir[i].DIR_NAME,11);
  				check_file[11] = '\0';
  				//parsing the strings that are in the file system
  				for(j = 0; j < 12; j++)
  				{
  					if(check_file[j] == ' ')
  					{
  						//null terminaing the string in the files system for later comparison
  						check_file[j] = '\0';
  					}
  				}
          //memcpy( new_dir, token[1], strlen( token[1] ) );
          memcpy(new_dir, input, strlen(input));
          strcpy(new_dir,input);
          //printf("%s\n", token[1]);
          if(strcasecmp(check_file, new_dir)==0)
          {
            int current_offset=0;
            correct = 1;
            if(strcasecmp(new_dir,"..")==0)
            {
              if(dir[i].DIR_FirstClusterLow==0)
              {
                current_offset=LBAToOffset(2);
              }
              else
              {
                current_dir = dir[i].DIR_FirstClusterLow;
                current_offset = LBAToOffset(current_dir);
                dot_dir = current_offset;
              }
  						fseek(fp, current_offset, SEEK_SET);
  						int w;
  						for(w = 0; w < 16; w++)
  						{
  							memset(&dir[w].DIR_NAME, 0, 11);
  							fread(&dir[w],1,32,fp);
  						}
            }
            else
            {
              current_dir = dir[i].DIR_FirstClusterLow;
              int current_offset = LBAToOffset(dir[i].DIR_FirstClusterLow);
              dot_dir = current_offset;
  						fseek(fp, current_offset, SEEK_SET);
  						int w;
  						for(w = 0; w < 16; w++)
  						{
  							memset(&dir[w].DIR_NAME, 0, 11);
  							fread(&dir[w],1,32,fp);
  						}
            }
          }
        }
        if(correct != 1)
        {
          printf("no such directory or file: %s\n", token[1]);

        }
        correct = 0;
        num_dirs--;
  }

      }
    }

	}
  if(strcasecmp(token[0],"stat")==0)
  {
    if(set==0)
    {
      printf("no file opened\n");
    }
    else
    {
      if(token[1]==NULL)
      {
        printf("Error: No file is given\n");
      }
      else
      {
        int io;
        char files[12];
        int checker=0;
        for(io = 0; io < 16; io++)
  			{
  				if(dir[io].DIR_Attr == 0x01 || dir[io].DIR_Attr == 0x10 || dir[io].DIR_Attr == 0x20)
  				{
  					strncpy(files,dir[io].DIR_NAME,11);
  					files[11] = '\0';
  					if(dir[io].DIR_Attr == 0x10 )
  					{
  						if(files[0] != -27)
  						{
  							int u;
  							for(u = 0; u < 12; u++)
  							{
  								if(files[u] == ' ')
  								{
  									files[u] = '\0';
  								}
  							}
                if(strcasecmp(files,token[1])==0)
                {
                  checker=1;
                  printf("attribute=%d\n",dir[io].DIR_Attr);
                  printf("Starting cluster number: %d\n",dir[io].DIR_FirstClusterLow);
                  printf("FILE size=0\n");
                  break;
                }
  						}
  					}
  					else
  					{
  						if(files[0] != -27)
  						{
  							put_together_2(files,token[1],io,&checker);
  						}
  					}

  				}



  			}
        if(checker==0)
        {
          printf("Error: File not found\n");
        }
        //printf("has file\n");
      }
    }
  }
  if(strcasecmp(token[0],"get")==0)
  {
    if(set==0)
    {
      printf("Error: nothing is opened\n");
    }
    else
    {
      if(token[1]==NULL)
      {
        printf("Error: No file is listed\n");
      }
      else
      {
        int io;
        char files[12];
        int checker=0;
        for(io = 0; io < 16; io++)
  			{
  				if(dir[io].DIR_Attr == 0x01 || dir[io].DIR_Attr == 0x10 || dir[io].DIR_Attr == 0x20)
  				{
  					strncpy(files,dir[io].DIR_NAME,11);
  					files[11] = '\0';
  					if(dir[io].DIR_Attr == 0x10 )
  					{
  						if(files[0] != -27)
  						{
  							int u;
  							for(u = 0; u < 12; u++)
  							{
  								if(files[u] == ' ')
  								{
  									files[u] = '\0';
  								}
  							}
                if(strcasecmp(files,token[1])==0)
                {
                  checker=1;
                  printf("a directoty\n");
                  break;
                }
  						}
  					}
  					else
  					{
  						if(files[0] != -27)
  						{
  							put_together_3(files,token[1],io,&checker,&current_dir);
                if(checker==1)
                {
                  FILE *fpp=fopen(token[1],"wb");
                  int c_offset=LBAToOffset(dir[io].DIR_FirstClusterLow);
                  int cluster=dir[io].DIR_FirstClusterLow;
                  int bytes=dir[io].DIR_FileSize;
                  unsigned char* temp=(char*)malloc(bytes);
                  while(bytes>512)
                  {
                    fseek(fp,c_offset,SEEK_SET);
                    fread(temp,1,512,fp);
                    bytes=bytes-512;
                    cluster=NextLB(cluster);
                    c_offset=LBAToOffset(cluster);
                    fwrite(temp, 1, 512, fpp);
                  }
                  if(bytes>0)
                  {
                    fseek(fp,c_offset,SEEK_SET);
                    fread(temp,1,bytes,fp);
                    fwrite(temp, 1, bytes, fpp);
                  }
                  fclose(fpp);
                  break;
                }
  						}
  					}
  				}
  			}
        if(checker==0)
        {
          printf("Error: File not found\n");
        }
      }
    }
  }
  if(strcmp(token[0],"read")==0)
  {
    if(set==0)
    {
      printf("NO FILE SYSTEM OPENED\n");
    }
    else
    {
      if(token[1]==NULL||token[2]==NULL||token[3]==NULL)
      {
        printf("Not all parameters given to do commmand!\n");
      }
      else
      {
        int io;
        char files[12];
        int checker=0;
        for(io = 0; io < 16; io++)
  			{
  				if(dir[io].DIR_Attr == 0x01 || dir[io].DIR_Attr == 0x10 || dir[io].DIR_Attr == 0x20)
  				{
  					strncpy(files,dir[io].DIR_NAME,11);
  					files[11] = '\0';
  					if(dir[io].DIR_Attr == 0x10 )
  					{
  						if(files[0] != -27)
  						{
  							int u;
  							for(u = 0; u < 12; u++)
  							{
  								if(files[u] == ' ')
  								{
  									files[u] = '\0';
  								}
  							}
                if(strcasecmp(files,token[1])==0)
                {
                  checker=1;
                  int c_offset=LBAToOffset(dir[io].DIR_FirstClusterLow);
                  int starting=atoi(token[2]);
                  c_offset=c_offset+atoi(token[2]);
                  int cluster=dir[io].DIR_FirstClusterLow;
                  int bytes=atoi(token[3]);
                  unsigned char* temp=(char*)malloc(bytes);
                  int temp2=0;
                  int bbytes;
                  int tracker=0;
                  for(bbytes=0;bbytes<bytes;bbytes++)
                  {
                    if(starting<512)
                    {
                      fseek(fp,c_offset,SEEK_SET);
                      fread(&temp2,1,1,fp);
                      printf("%x ",temp2);
                      starting++;
                      c_offset++;
                    }
                    else
                    {
                      starting=1;
                      cluster=NextLB(cluster);
                      c_offset=LBAToOffset(cluster);
                      fseek(fp,c_offset,SEEK_SET);
                      c_offset++;
                      fread(&temp2,1,1,fp);
                      printf("%x ",temp2);
                      starting++;
                    }
                  }
                  printf("\n");
                  break;
                }
  						}
  					}
  					else
  					{
  						if(files[0] != -27)
  						{
  							put_together_3(files,token[1],io,&checker,&current_dir);
                if(checker==1)
                {
                  int c_offset=LBAToOffset(dir[io].DIR_FirstClusterLow);
                  int starting=atoi(token[2]);
                  c_offset=c_offset+atoi(token[2]);
                  int cluster=dir[io].DIR_FirstClusterLow;
                  int bytes=atoi(token[3]);
                  unsigned char* temp=(char*)malloc(bytes);
                  int temp2=0;
                  int bbytes;
                  int tracker=0;
                  for(bbytes=0;bbytes<bytes;bbytes++)
                  {
                    if(starting<512)
                    {
                      fseek(fp,c_offset,SEEK_SET);
                      fread(&temp2,1,1,fp);
                      printf("%x ",temp2);
                      starting++;
                      c_offset++;
                    }
                    else
                    {
                      starting=1;
                      cluster=NextLB(cluster);
                      c_offset=LBAToOffset(cluster);
                      fseek(fp,c_offset,SEEK_SET);
                      c_offset++;
                      fread(&temp2,1,1,fp);
                      printf("%x ",temp2);
                      starting++;
                    }
                  }
                  printf("\n");
                  break;
                }
  						}
  					}
  				}
  			}
        if(checker==0)
        {
          printf("Error: File not found\n");
        }
      }
    }
  }
  if(strcasecmp(token[0],"volume")==0)
  {
    if(set==0)
    {
      printf("NO FILE SYSTEM OPENED\n");
    }
    else
    {
      printf("VOLUME Nameis:'%s'\n",DIR_VOL_NAME);
    }
  }
    free( working_root );
  }
  return 0;
}
