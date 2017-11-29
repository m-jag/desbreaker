#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "comparefile.h"

bool compareFile(FILE * file1, FILE * file2)
{
        bool sameFile = true;


        //---------- compare number of lines in both files ------------------
        int c1,c2;
        c1 = 0; c2 = 0;
        char ch = 0;
        //printf("File A\n");
        while((ch = fgetc(file1)) != EOF)
        {
	  //printf("%c", ch);
                if (ch == '\n')
                        c1++;
        }

        fseek(file1, 0, SEEK_SET);

        //printf("\nFile B\n");
        while((ch = fgetc(file2)) != EOF)
        {
          //    printf("%c", ch);
                if (ch == '\n')
                        c2++;
        }
        fseek(file2, 0, SEEK_SET);

        //printf("\nc1 : %d, c2 : %d \n", c1, c2);

        if(c1 != c2 || c1 == 0 || c2 == 0)
        {
                //printf("Different number of lines in files!\n");
                //printf("file1 has %d lines and file2 has %d lines\n", c1, c2);
                sameFile = false;
        }
        else
        {
                //printf("Comparing...\n");
                //---------- compare two files character by character ------------------
                char ch_f1, ch_f2;
                int j = 0;
                while(((ch_f1 = fgetc(file1)) != EOF) && ((ch_f2 = fgetc(file2)) != EOF))
                {
                        j++;
                        if(ch_f1 != ch_f2)
                        {
                                //printf("%d-th character is not the same", j);
                                sameFile = false;
                                break;
                        }
                }
        }

        fclose(file1);
        fclose(file2);

        //printf("The files are %sthe same\n", (sameFile? "": "not "));

        return sameFile;
}

int not_main()
{
        FILE * file1 = fopen("a.txt", "r");
        FILE * file2 = fopen("b.txt", "r");

        if (file1 == NULL)
                return 0;
        if (file2 == NULL)
                return 0;

        bool sameFile = compareFile(file1, file2);

        printf("The files are %sthe same\n", (sameFile? "": "not "));
	return 0;
}
