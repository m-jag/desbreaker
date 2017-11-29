#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <getopt.h> 

#include "DES.h"
#include "comparefile.h"

//////////////////////////////////////////////////////
//                 GLOBAL VARIABLES                //
////////////////////////////////////////////////////

static FILE * output = NULL;
static FILE * pt_file = NULL;
static FILE * ct_file = NULL;
static FILE * temp_file = NULL;
static bool complete = false;

//////////////////////////////////////////////////////
//                 FUNCTIONS                       //
////////////////////////////////////////////////////

// Usage 
static void usage(int status)
{
    if(status == EXIT_SUCCESS)
    {
        fprintf(stdout,"Usage: desbreaker [OPTION] -p= PLAINTEXT FILE -c= PLAINTEXT FILE\n"
                "Encrypt or Descrypt with DES.\n\n"
                " -p, --plaintext=PLAINTEXT       plaintext file to match to decrypted ciphertext\n"
                " -c, --ciphertext=CIPHERTEXT     decrypt DES from file\n"
                " -o, --output=FILE write results to FILE\n"
                " -h, --help        display this help\n");
    }
    else
    {
        fprintf(stderr, "Try 'desbreaker --help' for more information.\n");
    }
    exit(status);
}

static void runChild(uint64_t start_key, uint64_t batchsize)
{
    for (uint64_t key = start_key; !complete && key < start_key + batchsize; key++)
    {
        printf("%lu\n", key);
        //
        // 1. Verify parity bits of the key
        //
        if(!key_parity_verify(key))
        {
            //Bad Key
            //printf("The key you used is malformated\n"); // More error msg in function
            continue;
        }

        //
        // 2. Get the 16 subkeys
        //

        uint64_t a_key[16];
        a_key[0] = key;
        uint64_t next_key;

        for(int ii = 0; ii < 16; ii++)
        {
            key_schedule(&a_key[ii], &next_key, ii);
            if(ii != 15)
                a_key[ii + 1] = next_key;
        }

        //
        // 3. 16 Rounds of enc/decryption
        //

        size_t amount; // Used for fwrite
        uint64_t data;
	
	temp_file = fopen("temp.txt", "rw");
	
	fseek(temp_file, 0, SEEK_SET);
        fseek(pt_file, 0, SEEK_SET);    // move the file pointer back to the start of the file (if not currently there)
	
        while((amount = fread(&data, 1, 8, pt_file)) > 0)
        {
            if(amount != 8)
                data = data << (8 * (8 - amount));

            // Initial permutation
            Permutation(&data, true);
	    
	    for(int ii = 0; ii < 16; ii++)
	      rounds(&data, a_key[ii]);

            // Final permutation
            Permutation(&data, false);

            if(amount != 8)
                data = data << (8 * (8 - amount));


            // Write output
            fwrite(&data, 1, amount, temp_file);
            data = 0;
        }
        // compare files
        fseek(temp_file, 0, SEEK_SET);
	
	fseek(ct_file, 0, SEEK_SET);    // move the file pointer back to the start of the file (if not currently there)
	bool same = false;
	if ((same = compareFile(temp_file, ct_file)))
	  {
	    same ? printf("Its the same") : printf("Wups");
	    complete = true;
	    printf("Key found!\n Key : ");
	    printbits(key);
	    printf("\n");
	    break;
	  }
    }
    fclose(temp_file);
}

// Main
int main(int argc, char ** argv)
{
    //////////////////////////////////////////////////////
    //                 OPTION PARSER                   //
    ////////////////////////////////////////////////////

    int optc = 0;

    const char* short_opts = "p:c:ho";

    const struct option long_opts[] =
    {
        {"plaintext",        required_argument, NULL, 'p'},
        {"ciphertext",       required_argument, NULL, 'c'},
        {"help",           no_argument, NULL, 'h'},
        {"output",   required_argument, NULL, 'o'},
        {NULL,                       0, NULL,   0}
    };	
	
    while((optc = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1)
    { 
        switch(optc)
        {

        case 'h': // Help
            usage(EXIT_SUCCESS);
            break;

        case 'p': // Plaintext file
            pt_file = fopen(optarg, "r");
            if(pt_file == NULL)
            {
                fprintf(stderr,
                        "Error: don't have permission to read the plaintext file");
                exit(EXIT_FAILURE);
            }
            break;

        case 'c': // Output file
            ct_file = fopen(optarg, "rw");
            if(ct_file == NULL)
            {
                fprintf(stderr,
                        "Error: don't have permission to read the ciphertext file");
                exit(EXIT_FAILURE);
            }
            break;

        case 'o': // Output file
            output = fopen("out.txt", "w");
            if(output == NULL)
            {
                fprintf(stderr,
                        "Error: don't have permission to write output file");
                exit(EXIT_FAILURE);
            }
            break;

        default : // No arguments
            usage(EXIT_FAILURE); 
        }
    }

    //////////////////////////////////////////////////////
    //                CHECK ARGUMENTS                  //
    ////////////////////////////////////////////////////
    
    if(pt_file == NULL)
    {
        fprintf(stderr, "Error: no plaintext\n");
        usage(EXIT_FAILURE);
    }

    if(ct_file == NULL)
    {
        fprintf(stderr, "Error: no ciphertext\n");
        usage(EXIT_FAILURE);
    }

    //////////////////////////////////////////////////////
    //                      APP                        //
    ////////////////////////////////////////////////////


    // Vars
    uint64_t key = 0;
    uint64_t batchsize = 1000;

    runChild(key, batchsize);

    printf("Key Found : %s\n", (complete ? "True" : "False"));

    fclose(pt_file);
    fclose(ct_file);

    return EXIT_SUCCESS;
}
