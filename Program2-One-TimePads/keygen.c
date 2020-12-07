#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char const *argv[]) {

	// declare variables needed for looping and char generation
	int i = 0;
	char randChar;

	// seed rand w/ system time
	srand(time(0));

	// check if proper number of arguments has been passed
	if (argc != 2) {
		fprintf(stderr, "Error. Invalid number of arguments.\n");
		exit(1);
	}
	// check if argument passed is valid
	else {

		int length = strlen(argv[1]);
		
		// if first char in string arg is 0, it is invalid length
		if (argv[1][0] == 48) {
			fprintf(stderr, "Error. Invalid argument provided for keygen length.\n");
			exit(1);
		}
		// check rest of char in string array argument
		else {
			for (i = 1; i < length; i++) {
				// if char is not between 0-9, it is invalid
				if (argv[1][i] < 48 || argv[1][i] > 57) {
					fprintf(stderr, "Error. Invalid argument provided for keygen length.\n");
					exit(1);
				}
			}
		}
	}

	// generate random char for key
	for (i = 0; i < atoi(argv[1]); i++) {

		// generate number from 0 to 27
		randChar = rand() % 27;

		// if num is 0, print space char
		if (randChar == 0) {
			printf("%c", 32);
		}
		// else, add 64 to get char dec, from char ASCII value 65-90 (upper-case Alphabet)
		else {
			randChar += 64;
			printf("%c", randChar);
		}
	}

	// print newline char at end of key generation output
	printf("\n");
	// flush stdout
	fflush(stdout);

	return 0;
}
