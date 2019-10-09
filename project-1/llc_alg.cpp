#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <vector>

struct Process {
	int label;				// (label)th process
	int len;				// length of lc
	std::vector<int> lc;			// vector of logical clock time, int
};

struct LLC {
	int num_p;				// number of processes in LLC
	int p_size;				// size of an LLC process
	std::vector<Process> LC;		// whole processes
};

void enlarge(int n) {			// when a larger process exists, update the size and fill them with 0
	if (lc.size() < n) {
		int presize = lc.size();
		lc.resize(n);
		for (int i = presize; i < lc.size(); i++) {
			lc[i] = 0;
		}
	}
	len = lc.size();
}
		
		void set_label(int label) {		// set label
			lc.label = label;
		}

};

class LLC {
	private:
		int num_p;				// number of processes in LLC
		int p_size;				// size of an LLC process
		std::vector<Process> LLC;
	public:
		void add_p(Process p) {
			if (p.len > LLC.p_size) {
				for (int i = 0; i < num_p; i++) {
					LLC[i].enlarge(p.len);
				}
				p_size = p.len;
			} else if (p.len < LLC.p_size) {
				p.enlarge(LLC.p_size);
			}
			LLC.push_back(p);
			num_p++;
		}
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
