#include <iostream>
#include <cmath>
#include <vector>
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

uint64_t xrayRookAttacks(uint64_t occ, int rookSq) {
   uint64_t attacks = get_rook_attacks(rookSq, occ);
   uint64_t blockers = occ & attacks;
   return attacks ^ get_rook_attacks(occ ^ blockers, rookSq);
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
	Print.Test_Board(board.absolute_pins(40, WHITE));
	std::cout<<board.castle_kingside(WHITE);}