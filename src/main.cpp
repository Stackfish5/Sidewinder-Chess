#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>  

#include "Zobrist.h"
#include "movegen.h"

using std::to_string;
using std::string;
using std::uint64_t;
using std::uint32_t;



//encode moves
inline uint32_t encode_moves(int sourcesq, int targetsq, int piece, int promote, int capture, int double_push, int enpassant, int castle){
	uint32_t return_var = sourcesq | (targetsq << 6) | (piece << 12) | (promote << 16) | (capture << 20) | (double_push << 21) | (enpassant << 22) | (castle << 23);
	return return_var;}

class Move{
 void push( string move /*UCI string format but with 5th character for certain 
reasons including promotion*/){
  /*stuff goes here for making move*/
 }
 void pop(){
  /*Not completed*/
 }
};

inline uint64_t xrayRookAttacks(int rookSq, int side) {
  uint64_t blockers = occupancies[BOTH] & rook_masks[rookSq];
	uint64_t occ = occupancies[side];
	uint64_t attacks = get_rook_attacks(rookSq, occ);
  blockers &= attacks;
  return attacks ^ get_rook_attacks(rookSq, occ ^ blockers);
}

inline uint64_t xrayBishopAttacks(int bishopSq, int side) {
  uint64_t blockers = occupancies[BOTH] & bishop_masks[bishopSq];
	uint64_t occ = occupancies[side];
	uint64_t attacks = get_bishop_attacks(bishopSq, occ);
  blockers &= attacks;
  return attacks ^ get_bishop_attacks(bishopSq, occ ^ blockers);
}

//Absolute pinned pieces
//make it so side is the one with the pinned pieces to king
uint64_t absolute_pins(int side, int squareOfKing){
	uint64_t pinned, pinner;
	pinned = 0ULL;
	side ^= 1;
	pinner = xrayRookAttacks(squareOfKing, side ^ 1) & (Bitboards[R + side * 6] | Bitboards[Q + side * 6]);
	while ( pinner ) {
    int sq  = get_ls1b_index(pinner);
    pinned |= Rect_Lookup[sq] [squareOfKing] & occupancies[side ^ 1];
    pinner &= pinner - 1;
	}
	pinner = xrayBishopAttacks(squareOfKing, side ^ 1) & (Bitboards[B + side * 6] | Bitboards[Q + side * 6]);
  while ( pinner ) {
    int sq  = get_ls1b_index(pinner);
    pinned |= Rect_Lookup[sq] [squareOfKing] & occupancies[side ^ 1];
    pinner &= pinner - 1;
	}
	return pinned;
}

//for moves with pins, first get 
//pinner (use x ray attacks to get)
//then while pinner bitboard is true
//then find msb to get a rectagular
//lookup from index to king (also
//include msb square) Then find
//pinned piece. Identify it, use its
//pseudo-legal moves and use & to 
//find moves avaliable, also remove
//that bit from bitboard of pinners.



//create table where two inputs into table to return direction
//Also when calculating it just test looping through all mask directions from
//sq1 to sq2 as bitboard. if mask & bitboard == bitboard, set that direction in
//there. Also, if none of the directions are found, return -1
//really expensive but who cares
//Lookup returns relative direction from sq1 to sq2

int pin_direction [64] [64];
	
int direction(int sq1, int sq2){
	Legal_Moves pin;
	if(sq1 == sq2) {
		return -1; //error, not applicable
	}
	for(int direction = 0; direction < 8; direction++){
		//finds ray to test on
		uint64_t test_mask = pin.masks[direction][sq1];
		//find square 2 as bitboard
		uint64_t squares_bitboard = (1ULL << sq2);
		//if 
		if(test_mask & squares_bitboard){
			return direction;
		}
	}
	return -1; //if there isn't a ray direction
}

int main() {
	//Initialize stuff (prep)
	ChessBoard Print;
	Legal_Moves board;
	Initialize_Everything();
	WP = Print.Initialize(Board,'P');
	WN = Print.Initialize(Board,'N');
	WB = Print.Initialize(Board,'B');
	WR = Print.Initialize(Board,'R');
	WQ = Print.Initialize(Board,'Q');
	WK = Print.Initialize(Board,'K');
	BP = Print.Initialize(Board,'p');
	BN = Print.Initialize(Board,'n');
	BB = Print.Initialize(Board,'b');
	BR = Print.Initialize(Board,'r');
	BQ = Print.Initialize(Board,'q');
	BK = Print.Initialize(Board,'k');
	update_occupancies();

	//pin directions
	for(int sq1 = 0; sq1 < 64; sq1++){
		for(int sq2 = 0; sq2 < 64; sq2++){
			pin_direction[sq1][sq2] = direction(sq1, sq2);
		}
	}
	
	Print.Board();
	board.history.push_back(encode_moves(d7, d5, p, 0, 0, 1, 0, 0));
	Print.Test_Board(xrayBishopAttacks(d4, 1) & (Bitboards[B] | Bitboards[Q]));
	Print.Test_Board(absolute_pins(BLACK, d4));
	Print.Test_Board(xrayRookAttacks(d4, BLACK));
	std::cout<<pin_direction[a1] [d4];}