#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>
#include <fstream>
using std::ifstream;

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

unsigned long long convert(int i){
	i=63-i;
	unsigned long long intermediate=(unsigned long long)floor(i/8+1)*8-i%8-1;
	return intermediate;}

//this isn't good
void Parse_Fen(std::string FEN){
	int counter = 0;
	FEN.erase(remove(FEN.begin(), FEN.end(), '/'), FEN.end());
	char reader[1024];
	strcpy(reader, FEN.c_str());
	//Board setup
	//square number counter
	int sq_counter = 0;
	//Null bitboards
	Bitboards[P] = 0;
	Bitboards[N] = 0;
	Bitboards[B] = 0;
	Bitboards[R] = 0;
	Bitboards[Q] = 0;
	Bitboards[K] = 0;
	Bitboards[p] = 0;
	Bitboards[n] = 0;
	Bitboards[b] = 0;
	Bitboards[r] = 0;
	Bitboards[q] = 0;
	Bitboards[k] = 0;
	while(FEN.substr(counter) != " "){
		//if character isn't a number:
		switch(reader[counter]){
			//White Pawn
			case 'P': 
				Bitboards[P] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Black Pawn
			case 'p': 
				Bitboards[p] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//White Knight
			case 'N': 
				Bitboards[N] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Black Knight
			case 'n': 
				Bitboards[n] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//White Bishop
			case 'B': 
				Bitboards[B] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Black Bishop
			case 'b': 
				Bitboards[b] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//White Rook
			case 'R': 
				Bitboards[R] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Black Rook
			case 'r': 
				Bitboards[r] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//White Queen
			case 'Q': 
				Bitboards[Q] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Black Rook
			case 'q': 
				Bitboards[q] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//White King
			case 'K': 
				Bitboards[K] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Black King
			case 'k': 
				Bitboards[k] |= 1ULL << convert(sq_counter);
				counter ++;
				sq_counter ++;
				break;
			//Empty squares
			case '1': counter ++; sq_counter += 1; break;
			case '2': counter ++; sq_counter += 2; break;
			case '3': counter ++; sq_counter += 3; break;
			case '4': counter ++; sq_counter += 4; break;
			case '5': counter ++; sq_counter += 5; break;
			case '6': counter ++; sq_counter += 6; break;
			case '7': counter ++; sq_counter += 7; break;
			case '8': counter ++; sq_counter += 8; break;

		}		

		//
	}
}


int main() {
	//Initialize stuff (prep)
	ChessBoard Print;
	Legal_Moves board;
	Timer time;
	double_pawn_push pawn;
	Pins pin;
	Zobrist hash;

	Initialize_Everything();
	

	std::cout<<"hi";
	Parse_Fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	update_occupancies();
	Print.Board();

	//setting up en passant possibility
	board.history.push_back(encode_moves(f7, f5, 0, 0, 0, 1, 0, 0));

	moves move_list[1];
	
  // init move count
	time.reset();
	board.generate_moves(move_list, WHITE);
	std::cout<<std::fixed<<time.elapsed()<<"\n";
	board.print_move_list(move_list);
	std::cout<<"\n"<<"done!"<<"\n";
	hash.Initialize_Hash(Bitboards, WHITE, 1, 1, 1, 1, board.en_passant(WHITE));
	std::cout<<std::hex<<hash.Key<<"\n";
	std::string FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	std::cout<<FEN;
	FEN.erase(remove(FEN.begin(), FEN.end(), '/'), FEN.end());
	
}


//random links:
//http://talkchess.com/forum3/viewtopic.php?f=7&t=54872&hilit=tuning+sicilian&start=20#p604340
//http://www.talkchess.com/forum3/viewtopic.php?f=2&t=76446
//https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/