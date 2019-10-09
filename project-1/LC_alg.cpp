#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <assert.h>
#include <algorithm>

class LLC {
	private:
		struct coord {
			int label;
			int row;
			int col;
		};


	public:
		std::vector<std::vector<int>> p_lc;	// process_lc
		std::vector<int> p;			// 1 process

		std::vector<coord> s_loc;		// send location
		std::vector<coord> r_loc;		// recv location
		std::vector<bool> comp {std::vector<bool> (10, false)};	// indicator whether compared or not
		std::vector<int> s_lc {std::vector<int>(10, 0)};		// send logical time
		std::vector<int> r_lc {std::vector<int>(10, 0)};		// recv logical time
		
		int lc;					// logical time
		int num_p;				// number of processes in LLC, # of rows
		int p_size;				// size of an LLC process, # of columns
		
		void add_e(int lc) {			// add one event to a process
			p.push_back(lc);
		}

		void add_p(std::vector<int> &p) {	// add a process to whole processes
			p_lc.push_back(p);
		}

		void fitting(int num_p, int p_size, std::vector<std::vector<int>> &p_lc) { 		// fitting all processes to have same column 
			for (int i = 0; i < num_p; i++) {
				int tmp_size = p_lc[i].size();
				if(tmp_size < p_size) {
					p_lc[i].resize(p_size);
					for (int j = tmp_size; j < p_size; j++) {
						p_lc[i][j] = 0;
					}
				}
			}
		}
		
		void set_s_loc(int& label, int& row, int& col) {
			coord coord;
			coord.label = label;
			coord.row = row;
			coord.col = col;
			s_loc.push_back(coord);
		}

		void set_r_loc(int& label, int& row, int& col) {
			coord coord;
			coord.label = label;
			coord.row = row;
			coord.col = col;
			r_loc.push_back(coord);
		}
	
};


int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: ./LLC test.txt\n");
		return 0;
	}
	FILE* inf = fopen(argv[1], "r");
	if (inf != NULL) {
	
		LLC LLC;
		LLC.num_p = 0;
		LLC.p_size = 0;
		char c = fgetc(inf);
		while(c != EOF) {
			LLC.lc = 0;
			int tmp_col = 0;
			while (c != '\n' || '\0' || EOF) {
				printf("c = %c\n", c);					//TODO test
				if (c == 'r') {
					c = fgetc(inf);					// read next num char, 0~9
					int n = c - '0';				// a char to an int
					std::cout << "n = " << n << std::endl;			// TODO test
					assert (n >= 0 && n <= 9);				// if n is not between 0 and 9, exit failure
					LLC.lc++;
					if (LLC.r_lc[n] == 0) {					// if recv lc is not updated yet,
						LLC.add_e(LLC.lc);
						LLC.set_r_loc(n, LLC.num_p, tmp_col);		// save r_loc
						LLC.r_lc[n] = LLC.lc;				// save cur_lc to r_lc[n]
					} else {						// if recv lc is updated already,
						LLC.r_lc[n] = std::max(LLC.r_lc[n], LLC.lc);	// r_lc[n] = max(r_lc, cur_lc)
						LLC.lc = LLC.r_lc[n];				// update cur_lc
						LLC.comp[n] = true;				// comp[n] = true
					}
					tmp_col++;
				} else if (c == 's') {
					c = fgetc(inf);						// read next num char, 0~9
					int n = c - '0';					// a char to an int
					std::cout << "n = " << n << std::endl;			// TODO test
					assert (n >= 0 && n <= 9);				// if n is not between 0 and 9, exit failure
					LLC.lc++;
					LLC.add_e(LLC.lc);
					LLC.set_s_loc(n, LLC.num_p, tmp_col);			// save s_loc
					LLC.s_lc[n] = LLC.lc;					// save cur_lc to r_lc[n]
					tmp_col++;
				} else if (c >= 'a' && c <= 'z') {				// if c = 'a' ~ 'z' except 's' and 'r'
					LLC.lc++;						// lc + 1
					LLC.add_e(LLC.lc);					// save cur_lc to current process
					tmp_col++;
				} else if (c == ' ') {						// if c = ' ', continue 
//					continue;				
				} else if (c == 'N') {						// if c = 'N', add '0' to current process
					LLC.add_e(0);
					tmp_col++;
					while (c != ' ' || '\n' ) {
						c = fgetc(inf);
					}
				} else if (c == '\n') {
					break;
				} else	{
					printf("%d, wrong input signals\n", c);
					return EXIT_FAILURE;
				}
				c = fgetc(inf);
			}
			LLC.p_size = std::max(LLC.p_size, tmp_col);
			printf("tmp_col = %d\n", tmp_col);
			printf("p_size = %d\n", LLC.p_size);
			LLC.num_p++;
			LLC.add_p(LLC.p);
			LLC.p.clear();
			c = fgetc(inf);					//TODO tmp
		}

		for (int i = 0; i < LLC.num_p; i++) {
			printf("p[%d] : ", i);
			for (int j = 0; j < LLC.p_size; j++) {
				printf("%d ", LLC.p_lc[i][j]);
			}
			printf("\n");
		}

		for (int i = 0; i < 5; i++) {
			printf("recv%d is in row[%d]col[%d]\n", LLC.r_loc[i].label, LLC.r_loc[i].row, LLC.r_loc[i].col);
		}
				
	} else {
		std::cout << "cannot open file" << std::endl;
	}
	return 0;
}



/*
			
//TODO TODO TODO

	if the first character is char // ASCII a-Z
		make a logical clock
	else if the first character is int // ASCII 0-9
		verify the clock
			if between two consecutive numbers is a gap larger than 1
				save the position "r"
				save the # of "r" - 1
			using atoi
		

	
	
	
	return 0;
}
*/
