#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <assert.h>
#include <algorithm>

#define SENDER 1
#define RECEIVER 2
#define INTERNAL 3
#define NULL_EVENT 4

struct event {
	int type;
	int lc;
	bool valid;
};

struct coord {
	int label;
	int row;
	int col;
};

std::vector<std::vector<event>> p_lc;	// process_lc
std::vector<event> p;			// 1 process

std::vector<coord> s_loc;		// send location
std::vector<coord> r_loc;		// recv location
		
int lc;					// logical time
int num_p;				// number of processes in LLC, # of rows
int p_size;				// size of an LLC process, # of columns

event create_e(int type, int lc, bool valid) {
	event e;
	e.type = type;
	e.lc = lc;
	e.valid = valid;
	
	return e;
}

void add_e(event e) {			// add one event to a process
	p.push_back(e);
}

void add_p(std::vector<event> &p) {	// add a process to whole processes
	p_lc.push_back(p);
}

void set_s_loc(int label, int row, int col) {
//	if (p_lc[row][col].type == 1) {
		coord c;
		c.label = label;
	        c.row = row;
	        c.col = col;
		s_loc.push_back(c);
//	} else {
//		printf("wrong sender position\n");
//	}
}

void set_r_loc(int label, int row, int col) {
//	if (p_lc[row][col].type == 2) {
		coord c;
		c.label = label;
	        c.row = row;
	        c.col = col;
		r_loc.push_back(c);
//	} else {
//		printf("wrong receiver position\n");
//	}
}

void create_event_map(const char* file) {
	FILE* f = fopen(file, "r");

	if (!f) {
		perror("fopen");
		exit(1);
	}
	num_p = 0;
	p_size = 0;
	bool tmp_valid = false;
	char c;

	while(c != EOF) {
		lc = 0;
		int tmp_p_size = 0;
		while ((c = fgetc(f))!= '\n' || EOF) {
			printf("c = %c\n", c);					//TODO test
			event tmp_e;
			if (c == 'r') {
				c = fgetc(f);					// read next num char, 0~9
				int n = c - '0';				// a char to an int
				std::cout << "n = " << n << std::endl;			// TODO test
				assert (n >= 0 && n <= 9);				// if n is not between 0 and 9, exit failure
				lc++;							// lc + 1
				tmp_e = create_e(RECEIVER, lc, tmp_valid);		// create event
				add_e(tmp_e);						// add event to cur_process
				set_r_loc(n, num_p, tmp_p_size);			// save receiver location
				printf("r_loc[%d] = row[%d] col[%d]\n", r_loc[0].label, r_loc[0].row, r_loc[0].col);
				tmp_p_size++;
			} else if (c == 's') {
				c = fgetc(f);					// read next num char, 0~9
				int n = c - '0';				// a char to an int
				std::cout << "n = " << n << std::endl;			// TODO test
				assert (n >= 0 && n <= 9);				// if n is not between 0 and 9, exit failure
				lc++;							// lc + 1
				tmp_e = create_e(SENDER, lc, tmp_valid);		// create event
				add_e(tmp_e);						// add event to cur_process
				set_s_loc(n, num_p, tmp_p_size);			// save sender location
				tmp_p_size++;
			} else if (c >= 'a' && c <= 'z') {				// if c = 'a' ~ 'z' except 's' and 'r'
				lc++;							// lc + 1
				tmp_e = create_e(INTERNAL, lc, tmp_valid);		// create event
				add_e(tmp_e);						// add event to cur_process
				tmp_p_size++;
			} else if (c == ' ') {						// if c = ' ', do nothing				
			} else if (c == '\n') {						// if c = '\n', break
				break;
			} else if (c == '0') {						// if c = '0', add NULL_event to current process
				tmp_e = create_e(NULL_EVENT, 0, true);
				add_e(tmp_e);
				tmp_p_size++;
			} else if (c == -1) {
				break;
			} else {
				printf("%d, wrong input signals\n", c);
				exit(1);
			}
		}
		if ( c == EOF) {continue;}
		p_size = std::max(p_size, tmp_p_size);
		printf("tmp_p_size = %d\n", tmp_p_size);
		printf("p_size = %d\n", p_size);
		printf("num_p = %d\n", num_p);
		num_p++;
		add_p(p);
		p.clear();
	}

}

void print_map(const std::vector<std::vector<event>> &p_lc) {		//TODO test
	for (int i = 0; i < num_p; i++) {
		printf("p[%d] : ", i);
		for (int j = 0; j < p_size; j++) {
			printf("%d-%d -> ", p_lc[i][j].lc, p_lc[i][j].type);
		}
		printf("\n");
	}
		printf("r_loc.size = %lu\n", r_loc.size());
	for (int i = 0; i < r_loc.size(); i++) {
		printf("recv%d is in row[%d]col[%d]\n", r_loc[i].label, r_loc[i].row, r_loc[i].col);
	}
}


int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: ./LC test.txt\n");
		return 0;
	}

	create_event_map(argv[1]);
	print_map(p_lc);
				
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


/*
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
*/		
