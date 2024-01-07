#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "tags.h"
#include "read.h"
#include "edit.h"

int main(int argc, char *argv[])
{
	//Declaring variables
	int option;
	char ch;
	//Declaring variables of TagRead and TagEdit
	TagRead tagread;
	TagEdit tagedit;

	// Read the option for operation
	if (argc == 1)
	{
		printf("Select the option:\n1. Read\n2. Edit\nEnter option: ");
		scanf("%d", &option);
	}

	else
		//After selecting the option
		//Call the check_operation function
		//Passing through the cmd line paranthesis ( -r )
		option = check_operation(&argv[1]);
	
	
	switch (option)
	{
		//For reading the mp3 file
		case read:

			// Read the file name
			if (argc == 1)
			{
				printf("Enter the file name: ");
				scanf("%s", tagread.mp3_fname);
			}

			// Function call for reading the tags
			get_tags(&tagread, argv, argc);
			break;
	
	
		//For editing the file name
		case edit:
			
			// Read the file name
			if (argc == 1)
			{
				printf("Enter the file name: ");
				scanf("%s", tagedit.mp3_fname);
			}
			
			// Function call for editting the tags
			edit_tags(&tagedit, argv, argc);
			break;

		default:
			printf("Invalid option\n");
	}

	// No error return e_success;
	return e_success;

}