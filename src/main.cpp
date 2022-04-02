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

//board
char Board[64] = {' ',' ',' ','Q',' ',' ',' ',' ',
  								' ',' ',' ','p',' ',' ',' ',' ',
  								' ','Q',' ',' ',' ','Q',' ',' ',
  								' ',' ','p',' ','p',' ',' ',' ',
  								'Q','p',' ','k',' ','p','Q',' ',
  								' ',' ','p','p','p',' ',' ',' ',
  								' ',' ',' ',' ',' ','Q',' ',' ',
  								'Q',' ',' ','Q',' ',' ',' ',' '};

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
	uint64_t occ = occupancies[side ^ 1];
	uint64_t attacks = get_rook_attacks(rookSq, occ);
  blockers &= attacks;
  return attacks ^ get_rook_attacks(rookSq, occ ^ blockers);
}

inline uint64_t xrayBishopAttacks(int bishopSq, int side) {
  uint64_t blockers = occupancies[BOTH] & bishop_masks[bishopSq];
	uint64_t occ = occupancies[side ^ 1];
	uint64_t attacks = get_bishop_attacks(bishopSq, occ);
  blockers &= attacks;
  return attacks ^ get_bishop_attacks(bishopSq, occ ^ blockers);
}

//test purposes
 uint64_t inBetween(int sq1, int sq2) {
   const  uint64_t m1   = -1;
   const  uint64_t a2a7 = 0x0001010101010100;
   const  uint64_t b2g7 = 0x0040201008040200;
   const  uint64_t h1b7 = 0x0002040810204080; /* Thanks Dustin, g2b7 did not work for c1-a3 */
    uint64_t btwn, line, rank, file;

   btwn  = (m1 << sq1) ^ (m1 << sq2);
   file  =   (sq2 & 7) - (sq1   & 7);
   rank  =  ((sq2 | 7) -  sq1) >> 3 ;
   line  =      (   (file  &  7) - 1) & a2a7; /* a2a7 if same file */
   line += 2 * ((   (rank  &  7) - 1) >> 58); /* b1g1 if same rank */
   line += (((rank - file) & 15) - 1) & b2g7; /* b2g7 if same diagonal */
   line += (((rank + file) & 15) - 1) & h1b7; /* h1b7 if same antidiag */
   line *= btwn & -btwn; /* mul acts like shift by smaller square */
   return line & btwn;   /* return the bits on that line in-between */
}

uint64_t absolute_pins(int side,int squareOfKing){
	uint64_t pinned, pinner;
	pinned = 0ULL;
	pinner = xrayRookAttacks(squareOfKing, side ^ 1) & (Bitboards[R + side * 6] | Bitboards[Q + side * 6]);
	while ( pinner ) {
    int sq  = get_ls1b_index(pinner);
    pinned |= inBetween(sq, squareOfKing) & occupancies[BOTH];
    pinner &= pinner - 1;
	}
	pinner = xrayBishopAttacks(squareOfKing, side) ^ 1 & (Bitboards[B + side * 6] | Bitboards[Q + side * 6]);
  while ( pinner ) {
    int sq  = get_ls1b_index(pinner);
    pinned |= inBetween(sq, squareOfKing) & occupancies[BOTH];
    pinner &= pinner - 1;
	}
	return pinned & occupancies[side ^ 1];
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
	update_occupancies();
	Print.Board();
	board.history.push_back(encode_moves(d2,d4,p,0,0,1,0,0));
	Print.Test_Board(board.en_passant(BLACK));
	Print.Test_Board(xrayRookAttacks(a6, WHITE));
	Print.Test_Board(xrayRookAttacks(h6, WHITE));
	Print.Test_Board(inBetween(a1, a6));
	Print.Test_Board(absolute_pins(WHITE, d4));}