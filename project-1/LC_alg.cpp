#include <iostream>
#include <string>
#include <fstream>

class Process {
	array
	
};

class LLC {
	array of Process
	hash to verify

};

int main(int argc, char[] argv) {
	if(argc != 2) {
		printf("Usage: ./LLC test.txt\n");
	} else {
		FILE* inf = fopen(argv[1], "r");
		if (inf < 0) {
			perror("fopen");
			return EXIT_FAILURE;
		}
		FILE* outf = fopen("result.txt", "w");
		if (outf < 0) {
			perror("fopen");
			return EXIT_FAILURE;
		}
	}
	
	read character by character;
	if the first character is char // ASCII a-Z
		make a logical clock
			counting except "s" and "r"
	else if the first character is int // ASCII 0-9
		verify the clock
			if between two consecutive numbers is a gap larger than 1
				save the position "r"
				save the # of "r" - 1
			using atoi
		

	
	
	
	return 0;
}
