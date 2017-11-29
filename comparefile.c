#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "comparefile.h"

bool compareFile(FILE * file1, FILE * file2)
{
        bool sameFile = true;


        //---------- compare number of lines in both files ------------------
        char c1 = 0, c2 = 0;
	int file1ok, file2ok;
	uint64_t bytes_read = 0, errors = 0;
	while(1)
	  {
	    file1ok = fread(&c1, sizeof(char), 1, file1);
	    file2ok = fread(&c2, sizeof(char), 1, file2);
	    if (file1ok && file2ok)
	      {
		if ((c1 ^ c2) != 0)
		  {
		    errors++;
		    sameFile = false;
		  }
		bytes_read++;
		//printf("%c", c2);
	      }
	    else
	      {
		break;
	      }
	  }

        //printf("\nThe files are %sthe same.\n", (sameFile? "": "not "));
	//printf("\nResult: %lu bits compared. %lu equal bits found.\n", bytes_read, bytes_read-errors);
	return  sameFile;
}

int not_main()
{
        FILE * file1 = fopen("cipher.txt", "rb");
        FILE * file2 = fopen("temp.txt", "rb");

        if (file1 == NULL)
                return 0;
        if (file2 == NULL)
                return 0;

        bool sameFile = compareFile(file1, file2);

	
        fclose(file1);
        fclose(file2);
        printf("The files are %sthe same\n", (sameFile? "": "not "));
	return 0;
}
