#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "read.h"
#include "tags.h"
#include "types.h"

// Check opertaion to be performed
OperationType check_operation(char ** argv)
{
	//If the user wants to read the music file
	if (strncmp(*argv,"-r",2) == 0)
	{
		//Return the option read
		return read;
	}
	//If the user wants to edit the music file
	if (strncmp(*argv,"-e",2 )== 0)
	{
		//Return the option edit
		return edit;
	}
	else
		return e_unsupported;

}

// Read and validate the arguments
Status read_and_validate_read_args(char *argv[],TagRead *tagread)
{
	//Edit the mp3_fname wih the ones passed through the argument
	strcpy(tagread -> mp3_fname,  argv[2]);
	printf("%s", tagread->mp3_fname);

	return e_success;
}

// Open file in read mode
Status open_files(TagRead *tagread)
{
	//FILE *fopen(const char *restrict pathname, const char *restrict mode)
	//Open the mp3 file with the read mode
	tagread -> fptr_mp3 =fopen(tagread ->mp3_fname,"r" );

	//If the mp3 file is not successfully
	// Sanity check
	if (tagread -> fptr_mp3 == NULL)
	{
		perror("fopen");
		fprintf(stderr,"ERROR: unable to open the file %s\n",tagread ->mp3_fname);

		return e_failure;
	}
	return e_success;
}

// Calculate the tag size
Status get_size_tag(FILE *fptr)
{
	int idx;
	unsigned char ch = '\0';
	int size = 0;

	for (idx = 0; idx < sizeof(int); idx++)
	{
		//size_t fread(void *restrict ptr, size_t size, size_t nmemb,FILE *restrict stream);
		//Reading from fptr to ch
		//fptr is the mp3 file
		fread(&ch, sizeof(char), 1, fptr);
		size = size|ch;
		
		//Size of Tags
		if ((sizeof(int) - 1)!= idx)
		{
			//Size in bytes
			size <<= (sizeof(char) * 8);
		}
	}
	return size;
}

// Get album art tags
Status album_art(TagRead *tagread)
{
	char s1 = 0xff , s2 = 0xd8 ;
	char e1 = 0xff , e2 = 0xd9 ;
	char ch;
	int flag = 0;
	FILE *fp;

	// Open a file in write mode
	fp = fopen("pictures.jpg","w");

	// Sanity check
	if (fp == NULL)	
		printf("Error to open the file\n");
	
	while (feof(tagread->fptr_mp3) == 0)
	{
		fread(&ch, sizeof(char) , 1, tagread->fptr_mp3);
		
		// Check for ff d8
		while (ch == s1)
		{
			if (ch == s2)
			{
			
				// Write ff d8 to new file
				fwrite(&s1 , 1 ,1 ,fp);
				fwrite(&s2 , 1 , 1 , fp);

				while (1)
				{
					fread(&ch, sizeof(char) , 1 ,tagread->fptr_mp3);
					fwrite(&ch,1 ,1, fp);
					
					// Check for ff d9
					if (flag && ch == e2)
						break;
				
					if(ch == e1)
					
						flag = 1;
					else
						flag = 0;
				}
			}
		}
	}
}

// Read infomation from the file
Status readInfo(int *size, char **ptr, FILE *fptr)
{
	//Declaring variables
	int idx;
	int tag_size;
	char flag_buff[2];

	//Fetching the size of the song name size
	*size = get_size_tag(fptr);

	// Allocate memory  
	//ptr stores the Album Name 
	*ptr = (char*)malloc(*size * sizeof(char));

	// Neglect the flags
	fread(flag_buff , sizeof(char) , 2 ,fptr);
	
	// Get the size
	//Size of the mp3 file stored in the size pointer
	//Size of the mp3 file in the variable size
	// Read byte by byte from the file
	for (idx= 0; idx < (*size); idx++)
		fread(*ptr + idx, sizeof(char), 1, fptr);
	
	return e_success;
}

// Print the tags
Status print_tags(int size, char *ptr)
{
	//Declaring variables
	int idx;
	//Print the song name
	for (idx = 0; idx < size; idx++)
	{
		//Print the song name 
		//Printing one character at a time
		if (ptr[idx] > 31 && ptr[idx] != 92 && ptr[idx] != 127)
			printf("%c", ptr[idx]);
	}
	//Print the next line
	printf("\n");
	
	return e_success;
}

// Get the tags to read information
Status gets_tags_for_read(char*buffer, TagRead *tagread)
{
	//Declaring variables 
	int idx;
	//Character pointer array
	char *ptr_buff[7] ={ALBUM_NAME, ARTIST_NAME, TRACK_NAME,ALBUM_YEAR,TRACK_NO,COMPOSER_NAME,ALBUM_ART};

	//As there is a set of 6 information required
	for(idx=0; idx < 6; idx++)
	{
		//If the song name matches
		if(strncmp(buffer, ptr_buff[idx], 4) == 0)
		{

	printf("\n*************************************\n\n");
			switch(idx)
			{
				
				case 0:
					
					// Function call for read information from the tags
					readInfo( &tagread->album_name_size, &tagread->album_name ,tagread-> fptr_mp3);
					
					// Function call to print inormation from the tags
					printf("ALBUM NAME  :");
					print_tags(tagread->album_name_size, tagread->album_name);
					break;

				case 1:

					// Function call for read information from the tags
					readInfo( &tagread->singer_name_size, &tagread->singer_name ,tagread-> fptr_mp3);
					
					// Function call to print inormation from the tags
					printf("SINGER NAME  :");
					print_tags(tagread->singer_name_size, tagread->singer_name);
					break;

				case 2:
					
					// Function call for read information from the tags
					readInfo( &tagread->song_name_size, &tagread->song_name ,tagread-> fptr_mp3);
					
					// Function call to print inormation from the tags
					printf("SONG NAME  :");
					print_tags(tagread->song_name_size, tagread->song_name);
					break;

				case 3:
					
					// Function call for read information from the tags
					readInfo( &tagread->album_year_size, &tagread->album_year ,tagread-> fptr_mp3);
					
					// Function call to print inormation from the tags
					printf("ALBUM YEAR  :");
					print_tags(tagread->album_year_size, tagread->album_year);
					break;

				case 4:
					
					// Function call for read information from the tags
					readInfo( &tagread->track_number_size, &tagread->track_number ,tagread-> fptr_mp3);
					
					// Function call to print inormation from the tags
					printf("TRACK NUMBER :");
					print_tags(tagread->track_number_size, tagread->track_number);
					break;

				case 5:
					
					// Function call for read information from the tags
					readInfo( &tagread->composer_name_size, &tagread->composer_name ,tagread-> fptr_mp3);
					
					// Function call to print inormation from the tags
					printf("COMPOSER NAME :");
					print_tags(tagread->composer_name_size, tagread->composer_name);
					break;

				case 6:

					// Function call for reading album art info
					album_art(tagread);
					break;
			}
		}
	}
	return e_success;
}

// Get tags main function
Status get_tags(TagRead *tagread, char *argv[], int argc)
{
	//Declaring variables
	int byte_read, idx;
	char buffer[4];
	char ch;

	// Function call to read and validate arguments
	if (argc == 3)
		read_and_validate_read_args(argv, tagread);
	
	// Function call to open file
	open_files(tagread);

	//feof — test end-of-file indicator on a stream
	// Check the tag name
	// int feof(FILE *stream)
	while(feof(tagread -> fptr_mp3)==0)
	{
		//size_t fread(void *restrict ptr, size_t size, size_t nmemb,FILE *restrict stream);
		//Move fptr_mp3 which is the song label to ch variable
		fread(&ch ,sizeof(char) ,1 ,tagread->fptr_mp3);

		//Check for T or A
		if(ch == 'T' || ch == 'A')
		{
			//ch variable to be stored in the buffer array
			buffer[0] = ch;

			// Copy the remaining tag names characters
			for(idx = 1; idx < 4; idx++)
			{
				//Move fptr_mp3 to ch variable
				fread(&ch ,sizeof(char) ,1 ,tagread->fptr_mp3);
				buffer[idx] = ch;

			}
			buffer[idx] = '\0';

			// Function call to get the tags for reading info
			gets_tags_for_read(buffer, tagread);
		}
	}

	// Close file after reading all the song labels
	fclose(tagread->fptr_mp3);
	return e_success;
}