#include <string.h>

#define UNDECIDED 0
#define SENDER 1
#define RECEIVER 2
#define INTERNAL 3
#define NULL_EVENT 4

struct event {
	int type;
	int label;
	int lc;
	bool valid;
};

struct coord {
	int label;
	int row;
	int col;
};

std::vector<std::vector<event>> p_lc;	// process_lc
std::vector<event> p;			// tmp_process for p_lc

std::vector<coord> s_loc;		// send location
std::vector<coord>::iterator s_it;
std::vector<coord> r_loc;		// recv location
std::vector<coord>::iterator r_it;
		
int lc;					// logical time
int num_p;				// number of processes in LLC, # of rows
int p_size;				// size of an LLC process, # of columns

/*
 * char* outfileName(const char* fileName) {
	char* outName;
	strcpy(outName, fileName);
	strcat (outName, ".out");

	return outName;
}
*/
event create_e(int type, int label, int lc, bool valid) {	// type : send, recv, internal, null
	event e;						
	e.label = label;					// label for send and recv, e.g. "r1" means label = '1'
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
	for (s_it = s_loc.begin(); s_it != s_loc.end(); s_it++) {
		if (s_it->label == label) {
			printf("You have same sender label.\n");
			exit(1);
		}
	}
	coord c;
	c.label = label;
        c.row = row;
        c.col = col;
	s_loc.push_back(c);
}

void set_r_loc(int label, int row, int col) {
	for (r_it = r_loc.begin(); r_it != r_loc.end(); r_it++) {
		if (r_it->label == label) {
			printf("You have same receiver label.\n");
			exit(1);
		}
	}
	coord c;
	c.label = label;
        c.row = row;
        c.col = col;
	r_loc.push_back(c);
}

bool check_lc(std::vector<std::vector<event>> &p_lc) {
	for (int i = 0; i < num_p; i++) {
//		printf("p[%d] : ", i);
		for (int j = 0; j < p_size; j++) {
			if (p_lc[i][j].valid == false) {
				return false;
			}
//			printf("%6s ", p_lc[i][j].valid ? "true" : "false");

		}
//		printf("\n");
	}
	return true;
}

void fitting(int num_p, int p_size, std::vector<std::vector<event>> &p_lc) { 		// fitting all processes to have same column 
	event tmp_e = create_e(NULL_EVENT, -1, 0, true);
	for (int i = 0; i < num_p; i++) {
		int tmp_size = p_lc[i].size();
		if(tmp_size < p_size) {
			p_lc[i].resize(p_size);
			for (int j = tmp_size; j < p_size; j++) {
				p_lc[i][j] = tmp_e;
			}
		}
	}
}

int char_to_lc (char c) {
	assert(c >= '0' && c <= '9');
	int n = c - '0';
	return n;
}

void create_lc_map(const char* file) {
	FILE* f = fopen(file, "r");

	if (!f) {
		perror("fopen");
		exit(1);
	}
	num_p = 0;
	p_size = 0;
	bool tmp_valid = true;
	char c;

	while((c = fgetc(f)) != EOF) {
		int tmp_p_size = 0;
		while (c != '\n' || EOF) {
//			printf("c = %c\n", c);					//TODO test
			event tmp_e;
			if (c == '0') {
				tmp_e = create_e(NULL_EVENT, -1, 0, true);
				add_e(tmp_e);
				tmp_p_size++;
			} else if (c <= '9' && c >= '1') {
				lc = char_to_lc(c);				// a char to an int
				c = fgetc(f);
				while(c <= '9' && c >= '0') {
					lc *= 10;
					lc += char_to_lc(c);
					c = fgetc(f);
				}
//				printf("c = %c\n", c);					//TODO test
//				std::cout << "n = " << n << std::endl;			// TODO test
//				assert (n >= 1 && n <= 9);				// if n is not between 0 and 9, exit failure
//				printf("cur_lc = %d\n", lc);				// TODO test
				tmp_e = create_e(0, -1, lc, tmp_valid);			// tmp_event = undecided, no_label(-1), lc, true
				add_e(tmp_e);						// add event to cur_process
				tmp_p_size++;
			} else if (c == ' ') {						// if c = ' ', do nothing			
//				continue;	
			} else if (c == '\n') {						// if c = '\n', break
				break;
			} else {
				printf("%d, wrong input!\n", c);
				exit(1);
			}
			if (c == '\n') {
				continue;
			} else {
				c = fgetc(f);
			}
//			c = fgetc(f);
		}
		if ( c ==  EOF) {continue;}
		p_size = std::max(p_size, tmp_p_size);
//		printf("tmp_p_size = %d\n", tmp_p_size);
//		printf("p_size = %d\n", p_size);
//		printf("num_p = %d\n", num_p);
		if (tmp_p_size != 0) {
		num_p++;
		add_p(p);
		}
		p.clear();
	}
	fitting(num_p, p_size, p_lc);
	
	if (fclose(f)) {
		perror("fclose");
		exit(1);
	}
}

void verify_recv(std::vector<std::vector<event>> &p_lc) {
	int label = 1;
	for (int i = 0; i < num_p; i++) {
		int prev_lc = 0;
		for (int j = 0; j < p_size; j++) {
			if (p_lc[i][j].lc - prev_lc > 1) {		// if gap between cur logical time with previous logical time is larger than 1,
				p_lc[i][j].type = RECEIVER;		// cur event type is receive
				p_lc[i][j].label = label;
				prev_lc = p_lc[i][j].lc;	
				set_r_loc(label, i, j);			// add recv location
				label++;
			} else if (p_lc[i][j].lc - prev_lc == 1) {
				prev_lc = p_lc[i][j].lc;
			} else if (p_lc[i][j].type == 4) {
				continue;
			} else {
				printf("wrong sequence at row[%d], between col[%d]~col[%d]\n", i, j-1, j);
			}
		}
	}
	printf("In these processes, %d receive event(s) exist(s).\n", label-1);
}

void update_send(std::vector<std::vector<event>> &p_lc) {
	int target_lc;
	bool set;
//	printf("r_loc[0].label, r, c = %d, %d, %d\n", r_loc[0].label, r_loc[0].row, r_loc[0].col);
//	printf("r_loc.size() = %d\n", r_loc.size());
	for (int k = 1; k <= r_loc.size(); k++) {		// from recv label 1 to # of recv label, find appropriate logical time in whole process	
		set = false;
		target_lc = p_lc[r_loc[k - 1].row][r_loc[k - 1].col].lc - 1;
		for (int i = 0; i < num_p; i++) {
			for (int j = 0; j < p_size; j++) {
//				if (p_lc[i][j].lc == target_lc /*&& !set*/) {
				if (p_lc[i][j].lc == target_lc && !set) {
					set_s_loc(k, i, j);
					p_lc[i][j].type = SENDER;
					p_lc[i][j].label = k;
					set = true;
				} else if (p_lc[i][j].type == 0) {
					p_lc[i][j].type = INTERNAL;
				} else {
					continue;
				}
			}
		}
	}
	
	if (r_loc.size() == 0) {
		for (int i = 0; i < num_p; i++) {
			for (int j = 0; j < p_size; j++) {
				if (p_lc[i][j].type == 0) {
					p_lc[i][j].type = INTERNAL;
				}
			}
		}
		set = true;
	}
	if (set == false) {
		printf("Incorrect Process, not enough send event.\n");
		exit(1);
	}
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
//			printf("c = %c\n", c);					//TODO test
			event tmp_e;
			if (c == 'r') {
				c = fgetc(f);					// read next num char, 0~9
				int n = c - '0';				// a char to an int
//				std::cout << "n = " << n << std::endl;			// TODO test
				assert (n >= 0 && n <= 9);				// if n is not between 0 and 9, exit failure
				lc++;							// lc + 1
				tmp_e = create_e(RECEIVER, n, lc, tmp_valid);		// create event
				add_e(tmp_e);						// add event to cur_process
				set_r_loc(n, num_p, tmp_p_size);			// save receiver location
				tmp_p_size++;
			} else if (c == 's') {
				c = fgetc(f);					// read next num char, 0~9
				int n = c - '0';				// a char to an int
//				std::cout << "n = " << n << std::endl;			// TODO test
				assert (n >= 0 && n <= 9);				// if n is not between 0 and 9, exit failure
				lc++;							// lc + 1
				tmp_e = create_e(SENDER, n, lc, tmp_valid);		// create event
				add_e(tmp_e);						// add event to cur_process
				set_s_loc(n, num_p, tmp_p_size);			// save sender location
				tmp_p_size++;
			} else if (c >= 'a' && c <= 'z') {				// if c = 'a' ~ 'z' except 's' and 'r'
				lc++;							// lc + 1
				tmp_e = create_e(INTERNAL, -1, lc, tmp_valid);		// create event
				add_e(tmp_e);						// add event to cur_process
				tmp_p_size++;
			} else if (c == ' ') {						// if c = ' ', do nothing				
			} else if (c == '\n') {						// if c = '\n', break
				break;
			} else if (c == '0') {						// if c = '0', add NULL_event to current process
				tmp_e = create_e(NULL_EVENT, -1, 0, true);
				add_e(tmp_e);
				tmp_p_size++;
			} else if (c == -1) {
				break;
			} else {
				printf("%d, wrong input!\n", c);
				exit(1);
			}
		}
		if ( c == EOF) {continue;}
		p_size = std::max(p_size, tmp_p_size);
//		printf("tmp_p_size = %d\n", tmp_p_size);
//		printf("p_size = %d\n", p_size);
//		printf("num_p = %d\n", num_p);
		num_p++;
		add_p(p);
		p.clear();
	}
	fitting(num_p, p_size, p_lc);
	if (fclose(f)) {
		perror("fclose");
		exit(1);
	}
}


void update_p_lc(std::vector<std::vector<event>> &p_lc) {				// update logical time and validity
	std::vector<coord> flag_c;
	bool prev = true;
	while(!check_lc(p_lc)) {
		for (int i = 0; i < num_p; i++) {
			for (int j = 0; j < p_size; j++) {
				if (p_lc[i][j].type == INTERNAL) {			// if cur event is internal, then it is valid
					if (prev == true) {
						p_lc[i][j].valid = true;
						prev = p_lc[i][j].valid;
						if (j == 0) {
							p_lc[i][j].lc = 1;
						} else {
							p_lc[i][j].lc = p_lc[i][j - 1].lc + 1;
						}
					}
				} else if (p_lc[i][j].type == SENDER) {			// if cur_event is send, it is valid
					if (prev == true) {
						p_lc[i][j].valid = true;				
						prev = p_lc[i][j].valid;
						if (j == 0) {
							p_lc[i][j].lc = 1;
						} else {
							p_lc[i][j].lc = p_lc[i][j - 1].lc + 1;
						}
						for (r_it = r_loc.begin(); r_it != r_loc.end(); r_it++) {	// searching recv location vector to update logical time
							if (r_it->label == p_lc[i][j].label) {
								p_lc[r_it->row][r_it->col].lc = std::max(p_lc[r_it->row][r_it->col].lc, p_lc[i][j].lc + 1);	// compare sender_lc + 1 with origin time
								p_lc[r_it->row][r_it->col].valid = true;	// update recv event validity to true
								break;
							}
						}
						if (r_it == r_loc.end()){
							printf("send event at [%d][%d] does not match any receive event.\n", i, j);
							exit(1);
						}
					}
				} else if (p_lc[i][j].type == RECEIVER) {			
					if (p_lc[i][j].valid == false) {				// if this is recv and not valid, searching flags whether this has been visited
						std::vector<coord>::iterator it = flag_c.begin();	// if visited, incorrect process, else add to flag
						for (;it != flag_c.end(); it++) {			
							if (it->row == i && it->col == j) {
								printf("Incorrect Process!\n");
								exit(1);
							}
						}
						coord flag;
						flag.label = 0;
						flag.row = i;
						flag.col = j;
						flag_c.push_back(flag);
					}
					if (j != 0) {
						p_lc[i][j].lc = std::max(p_lc[i][j].lc, p_lc[i][j - 1].lc + 1);	//if visited, lc value has been already updated by sender, so compare lc in this process with lc from sender
					} else {
						p_lc[i][j].lc = std::max(p_lc[i][j].lc, 1);
					}
//					p_lc[i][j].valid = true;
					prev = p_lc[i][j].valid;
				}
			}
		}
	}
	for (int i = 0; i < num_p; i++) {
		for (int j = 1; j < p_size; j++) {
			if (p_lc[i][j].type == NULL_EVENT) {
				for (; j < p_size; j++) {
					p_lc[i][j].lc = 0;
				}
			} else { 
				p_lc[i][j].lc = std::max(p_lc[i][j].lc, p_lc[i][j - 1].lc +1);
			}
		}
	}	
}

void print_lc(const std::vector<std::vector<event>> &p_lc, FILE* fp) {		//TODO test
//	FILE* fp = fopen(outfile, "w");

	for (int i = 0; i < num_p; i++) {
		fprintf(fp, "p[%d] : ", i);
		for (int j = 0; j < p_size; j++) {
			fprintf(fp, "%d ", p_lc[i][j].lc);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\n");
	for (int i = 0; i < s_loc.size(); i++) {							// TODO test
		fprintf(fp, "send%d is at [%d][%d]\n", s_loc[i].label, s_loc[i].row, s_loc[i].col);	
	}
	fprintf(fp, "\n");
	for (int i = 0; i < r_loc.size(); i++) {							// TODO test
		fprintf(fp, "recv%d is at [%d][%d]\n", r_loc[i].label, r_loc[i].row, r_loc[i].col);	
	}
	fclose(fp);
}

void print_map(const std::vector<std::vector<event>> &p_lc, FILE* fp) {		//TODO test
//	FILE* fp = fopen(outfile, "w");

	for (int i = 0; i < num_p; i++) {
		fprintf(fp, "p[%d] : ", i);
		for (int j = 0; j < p_size; j++) {
			if (p_lc[i][j].type == 1) {
				fprintf(fp, "  s%d", p_lc[i][j].label);
			} else if (p_lc[i][j].type == 2) {
				fprintf(fp, "  r%d", p_lc[i][j].label);
			} else if (p_lc[i][j].type == 3) {
				fprintf(fp, "   i");
			} else { 
				fprintf(fp, " Nil");
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

