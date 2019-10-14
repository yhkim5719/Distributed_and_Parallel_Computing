#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <assert.h>
#include <algorithm>
#include "LC.h"

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: ./LC testfile.txt\n");
		return 0;
	}
	
	FILE* f = fopen(argv[1], "r");
	FILE* out = fopen("output.txt", "w");

	char c = fgetc(f);
	if ( c >= '1' && c <= '9' ) {
		fclose(f);
		create_lc_map(argv[1]);
		verify_recv(p_lc);
		update_send(p_lc);
		print_map(p_lc, out);
	} else {
		fclose(f);
		create_event_map(argv[1]);
		update_p_lc(p_lc);
		print_lc(p_lc, out);
	}
	return 0;
}

