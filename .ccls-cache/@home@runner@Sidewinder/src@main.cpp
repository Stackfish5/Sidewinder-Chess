#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>  

#include "Zobrist.h"
#include "movegen.h"
#include "timer.h"

using std::to_string;
using std::string;
using std::uint64_t;
using std::uint32_t;

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
//really expensive but who cares?
//Lookup returns relative direction from sq1 to sq2



//to resolve pins, use the pin_direction to find direction relative to the
//two squares through getting king square and pinned piece square. If the 
//result isn't -1, then find masks[direction] to get pin masks. Now get
//pinned piece type, generate pseudo-legal moves and & them with the mask
//to return result, legal moves for pinned piece!


int main() {
	//Initialize stuff (prep)
	ChessBoard Print;
	Legal_Moves board;
	Timer time;
	double_pawn_push pawn;
	Pins pin;

	Initialize_Everything();
	update_occupancies();
	Print.Board();

	//setting up en passant possibility
	board.history.push_back(encode_moves(f7, f5, 0, 0, 0, 1, 0, 0));

	moves move_list[1];
	
  // init move count
	time.reset();
	board.generate_moves(move_list, WHITE);
	std::cout<<time.elapsed()<<"\n";
	board.print_move_list(move_list);
	std::cout<<"\n"<<"done!"<<"\n";
	std::cout<<"original pawn board:";
	Print.Test_Board(Bitboards[p]);
	pop_bit(Bitboards[(WHITE ^ 1) * 6], (f6 + ((WHITE == WHITE) ? -8 : 8)));
	update_occupancies();
	Print.Test_Board(board.square_attackers(e4, BLACK));
	Print.Test_Board(Bitboards[p]);
	std::cout<<board.legal_en_passant(WHITE, e5, h8);
	uint64_t bob = 0x2000000000;
	set_bit(Bitboards[P], f6);
	Print.Test_Board(bob);}


//random links:
//http://talkchess.com/forum3/viewtopic.php?f=7&t=54872&hilit=tuning+sicilian&start=20#p604340
//http://www.talkchess.com/forum3/viewtopic.php?f=2&t=76446
//https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/