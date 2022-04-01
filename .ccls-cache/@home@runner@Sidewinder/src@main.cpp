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

const uint64_t Rook_Masks[64] = {
0x1010101010101fe,
0x2020202020202fd,
0x4040404040404fb,
0x8080808080808f7,
0x10101010101010ef,
0x20202020202020df,
0x40404040404040bf,
0x808080808080807f,
0x10101010101fe01,
0x20202020202fd02,
0x40404040404fb04,
0x80808080808f708,
0x101010101010ef10,
0x202020202020df20,
0x404040404040bf40,
0x8080808080807f80,
0x101010101fe0101,
0x202020202fd0202,
0x404040404fb0404,
0x808080808f70808,
0x1010101010ef1010,
0x2020202020df2020,
0x4040404040bf4040,
0x80808080807f8080,
0x1010101fe010101,
0x2020202fd020202,
0x4040404fb040404,
0x8080808f7080808,
0x10101010ef101010,
0x20202020df202020,
0x40404040bf404040,
0x808080807f808080,
0x10101fe01010101,
0x20202fd02020202,
0x40404fb04040404,
0x80808f708080808,
0x101010ef10101010,
0x202020df20202020,
0x404040bf40404040,
0x8080807f80808080,
0x101fe0101010101,
0x202fd0202020202,
0x404fb0404040404,
0x808f70808080808,
0x1010ef1010101010,
0x2020df2020202020,
0x4040bf4040404040,
0x80807f8080808080,
0x1fe010101010101,
0x2fd020202020202,
0x4fb040404040404,
0x8f7080808080808,
0x10ef101010101010,
0x20df202020202020,
0x40bf404040404040,
0x807f808080808080,
0xfe01010101010101,
0xfd02020202020202,
0xfb04040404040404,
0xf708080808080808,
0xef10101010101010,
0xdf20202020202020,
0xbf40404040404040,
0x7f80808080808080};

const uint64_t Bishop_Masks[64] = {
0x8040201008040200,
0x80402010080500,
0x804020110a00,
0x8041221400,
0x182442800,
0x10204885000,
0x102040810a000,
0x102040810204000,
0x4020100804020002,
0x8040201008050005,
0x804020110a000a,
0x804122140014,
0x18244280028,
0x1020488500050,
0x102040810a000a0,
0x204081020400040,
0x2010080402000204,
0x4020100805000508,
0x804020110a000a11,
0x80412214001422,
0x1824428002844,
0x102048850005088,
0x2040810a000a010,
0x408102040004020,
0x1008040200020408,
0x2010080500050810,
0x4020110a000a1120,
0x8041221400142241,
0x182442800284482,
0x204885000508804,
0x40810a000a01008,
0x810204000402010,
0x804020002040810,
0x1008050005081020,
0x20110a000a112040,
0x4122140014224180,
0x8244280028448201,
0x488500050880402,
0x810a000a0100804,
0x1020400040201008,
0x402000204081020,
0x805000508102040,
0x110a000a11204080,
0x2214001422418000,
0x4428002844820100,
0x8850005088040201,
0x10a000a010080402,
0x2040004020100804,
0x200020408102040,
0x500050810204080,
0xa000a1120408000,
0x1400142241800000,
0x2800284482010000,
0x5000508804020100,
0xa000a01008040201,
0x4000402010080402,
0x2040810204080,
0x5081020408000,
0xa112040800000,
0x14224180000000,
0x28448201000000,
0x50880402010000,
0xa0100804020100,
0x40201008040201
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
	pinned = 0;
	pinner = xrayRookAttacks(squareOfKing, side ^ 1) & (Bitboards[3 + side * 6] | Bitboards[4 + side * 6]);
	while ( pinner ) {
    int sq  = get_ls1b_index(pinner);
    pinned |= inBetween(sq, squareOfKing) & occupancies[side];
    pinner &= pinner - 1;
	}
	pinner = xrayBishopAttacks(squareOfKing, side) ^ 1 & (Bitboards[2 + side * 6] | Bitboards[4 + side * 6]);
  while ( pinner ) {
    int sq  = get_ls1b_index(pinner);
    pinned |= inBetween(sq, squareOfKing) & occupancies[side];
    pinner &= pinner - 1;
	}
	return pinned;
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
	Print.Board();
	board.history.push_back(encode_moves(c7,c5,p,0,0,1,0,0));
	Print.Test_Board(board.en_passant(WHITE));
	std::cout<<board.is_square_attacked(c6, WHITE);
	Print.Test_Board(board.attack_map(WHITE));
	std::cout<<"\n"<<board.is_square_attacked(40, WHITE)<<"\n";
	Print.Test_Board(board.square_attackers(c6,WHITE));
	std::cout<<get_ls1b_index(90);
	Print.Test_Board(board.absolute_pins(h6, WHITE));
	std::cout<<board.castle_kingside(WHITE);
	Print.Test_Board(xrayRookAttacks(h6, WHITE));
	Print.Test_Board(xrayBishopAttacks(c8, WHITE));
	Print.Test_Board(inBetween(f1, a6));
	std::cout<<get_ls1b_index(9999000000);
	Print.Test_Board(absolute_pins(WHITE, a6));}