#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void print_board(int board[]){
	// print board
	printf("+---------+---------+---------+\n");
	for (int line=0; line<9; line++) {
		putchar('|');
		for (int col=0; col<9; col++) {
			int c= board[line*9+col];
			if (0==c) {
				printf(" . ");
			} else {
				printf(" %d ",c);
			}
			if (0==(col+1)%3) {
				putchar('|');
			}
		}
		printf("\n");
		if (0==(line+1)%3) {
			printf("+---------+---------+---------+\n");
		}
	}
}

void print_numbers(long matches){
	printf("Match: 0x%03lx (%ld)",matches,matches);
	printf("Possible numbers: ");
	for (int i=0; i<9; i++) {
		long mask = 1 << i;
		if (mask == (mask & matches)) {
			printf("%d ",i+1);
		}
	}
	printf("\n");
}

int convert_bitwise(long matches){
	for (int i=0; i<9; i++) {
		long mask = 1 << i;
		if (mask == (mask & matches)) {
			return i+1;
		}
	}
	return 0;
}

long poss_matches(int* board, int position) {
	// only look for matches for empty positions
	if (board[position]!=0) {
		return 0;
	}
	// where on the board are we?
	int row = floor((double)position / 9.0);
	int row_offset = row % 3;
	int column = position % 9;
	int column_offset = column % 3;
	int box = floor((double)column / 3.0);
	
	// generate possible matches
	long mask = 0x1FF;
	// first we go along the row
	for (int i=0; i<9; i++) {
		int pos = i+(row*9);
		if (board[pos] != 0) {
			long eliminate_number = 1 << (board[pos]-1);
			mask &= ~eliminate_number;
		}
	}
	// then down the column
	for (int i=0; i<9; i++) {
		int pos = (i*9)+column;
		if (board[pos] !=0) {
			long eliminate_number = 1 << (board[pos]-1);
			mask &= ~eliminate_number;
		}
	}
	// then we look around the box
	for (int rowc=0; rowc<3; rowc++) {
		for (int colc=0; colc<3; colc++) {
			int pos=(row-row_offset+rowc)*9+(column-column_offset+colc);
			if (board[pos]!=0) {
			long eliminate_number = 1 << (board[pos]-1);
			mask &= ~eliminate_number;
			}
		}
	}
	return mask;
}

int simple_iterate(int board[]){
	int found_match = 0;
	for (int i=0; i<81; i++) {
		long match = poss_matches(board,i);
		// http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
		// the first half (match &&) evaluates to true if match is not zero (the second half only works for positive numbers)
		// the second half is a very clever trick. If you subtract 1 from a power of 2 (say, 01000) then all the lower bits flip (00111)
		// and the first number AND the second number is 0 because no bits are in common. With NOT, it evaluates to TRUE.
		// If the number is not a power of 2 (say, 01010) then subtracting one will leave some bits in common (01010 & 01001)
		// so it'll evaluate to TRUE (or FALSE when used in combination with NOT).
		// three hurrays to all non-zero values evaluating to TRUE.
		int power_of_two = match && !(match & (match - 1));
		if (power_of_two) {
			board[i]=convert_bitwise(match);
			found_match=1;
		}
	}		
	return found_match;
}

int check_solution(int board[]){
	for (int i=0; i<81; i++){
		if (0==board[i]){
			return 0;
		}
	}
	return 1;
}

int recursive_iterate(int board[]){
	for (int i=0; i<81; i++) {
		if (0==board[i]) {
			long remaining_matches = poss_matches(board,i);
			if (!remaining_matches) {
				// no match possible for current board
				return 0;
			}
			while (remaining_matches) {
				int first_match = convert_bitwise(remaining_matches);
				remaining_matches = remaining_matches ^ (1 << (first_match-1));
				board[i]=first_match;
				if (check_solution(board)) {
					// board is solved
					printf("Board is solved!\n");
					return 99;
				} else {
					int solution = recursive_iterate(board);
					if (99==solution) {
						// board was solved further down the line
						return solution;
					}
					if (0==solution) {
						// we encountered a dead end -- rolling back the latest change
						board[i]=0;
					}
				}
			}
			return 0;
		}
	}
	return 0;
}	

int main (int argc, char* argv[]) {
/*	int board[81] = { 
					0,0,6,1,5,7,4,0,8,
					7,2,8,3,4,0,5,9,0,
					1,5,4,9,0,0,0,0,3,
					8,7,0,2,0,1,0,4,5,
					0,1,0,0,0,5,6,0,7,
					0,6,5,0,7,0,0,0,2,
					6,3,1,7,2,0,0,0,4,
					5,4,0,6,3,0,1,7,9,
					9,8,7,0,0,4,0,3,0 
					}; */
					
	int board[81] = {
					4,2,0,0,7,8,0,3,0,
					0,0,0,0,0,0,0,4,0,
					1,3,8,9,0,0,0,0,0,
					3,9,0,0,4,5,0,0,7,
					0,0,0,0,9,0,0,0,0,
					7,0,0,3,1,0,0,5,9,
					0,0,0,0,0,3,8,6,4,
					0,1,0,0,0,0,0,0,0,
					0,4,0,7,5,0,0,9,2
					};
					
	print_board(board);
	int iterate = 1;
	while(simple_iterate(board)) {
		printf("Iteration %d\n",iterate++);
	}
	print_board(board);
	printf("Starting recursive iterate...\n");
	int result = recursive_iterate(board);
	print_board(board);
}
