/*****************************************

	Copyright 2015 Niclas Mika

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


******************************************/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

void randomize() {
	srand(time(NULL));
}


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

int convert_bitwise(long matches){
	for (int i=0; i<9; i++) {
		long mask = 1 << i;
		if (mask == (mask & matches)) {
			return i+1;
		}
	}
	return 0;
}

int random_convert_bitwise(long matches){
	int count = 0;
	int m[10]= { 0 };
	for (int i=0; i<9; i++) {
		long mask = 1 << i;
		if (mask == (mask & matches)) {
			m[count]=i+1;
			count++;
		}
	}
	if (count==0) {
		return 0;
	} else {
		int match;
		int randint=0;
		do {
			randint = rand() % 9;
		} while (m[randint]==0);
		match=m[randint];
		return match;
	}
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
				int first_match = random_convert_bitwise(remaining_matches);
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

	int board[81] = { 0 };
	
	randomize();
	for (int i=1; i<10; i++) {
		int val;
		do {
			val = (rand() % 9);
		} while (board[val]!=0);
		board[val]=i;
	}
			
	print_board(board);
	int result = recursive_iterate(board);
	print_board(board);
}
