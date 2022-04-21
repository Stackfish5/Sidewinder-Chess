#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string.h>

#include "attacks.h"
#include "Hash.h"

//Rectangular lookup tables
uint64_t Rect_Lookup [64] [64]; //[sq1] [sq2]

//Initialization purposes
uint64_t inBetween(int sq1, int sq2) {
   const  uint64_t m1   = -1;
   const  uint64_t a2a7 = 0x0001010101010100;
   const  uint64_t b2g7 = 0x0040201008040200;
   const  uint64_t h1b7 = 0x0002040810204080;
    uint64_t btwn, line, rank, file;

   btwn  = (m1 << sq1) ^ (m1 << sq2);
   file  =   (sq2 & 7) - (sq1   & 7);
   rank  =  ((sq2 | 7) -  sq1) >> 3 ;
   line  =      (   (file  &  7) - 1) & a2a7;
   line += 2 * ((   (rank  &  7) - 1) >> 58);
   line += (((rank - file) & 15) - 1) & b2g7;
   line += (((rank + file) & 15) - 1) & h1b7;
   line *= btwn & -btwn;
   return line & btwn;
}

// init slider piece's attack tables
void init_sliders_attacks(int bishop){
    // loop over 64 board squares
    for (int square = 0; square < 64; square++){
        // init bishop & rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);
        uint64_t attack_mask = bishop ? bishop_masks[square] : rook_masks[square];
        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);
        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);
        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++){
          if (bishop){
            // init current occupancy variation
            uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
            // init magic index
            int magic_index = (occupancy * BMagic[square]) >> (64 - bishop_relevant_bits[square]);
          	// init bishop attacks
            Bishop_Attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }
          else{
            // init current occupancy variation
            uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
            // init magic index
            int magic_index = (occupancy * RMagic[square]) >> (64 - rook_relevant_bits[square]);
            // init bishop attacks
            Rook_Attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);}}}
}

enum Piece_Types {P, N, B, R, Q, K, p, n, b, r, q, k};

enum colors{WHITE, BLACK, BOTH};



//Occupancies
uint64_t occupancies[3];
void update_occupancies(){
	uint64_t to_occupancy1 = 0ULL;
	uint64_t to_occupancy2 = 0ULL;
	uint64_t to_occupancy3 = 0ULL;
	for(int i=0; i<6; i++){
		to_occupancy1 |= Bitboards[i];
		to_occupancy2 |= Bitboards[i + 6];
		to_occupancy3 |= to_occupancy1 | to_occupancy2;}
	occupancies[0] = to_occupancy1;
	occupancies[1] = to_occupancy2;
	occupancies[2] = to_occupancy3;}

//Castling rights
int Castle_White_Kingside = 1;
int Castle_White_Queenside = 1;

int Castle_Black_Kingside = 1;
int Castle_Black_Queenside = 1;

//promoting pieces
char promoted_pieces[] = {
    [Q] = 'q',
    [R] = 'r',
    [B] = 'b',
    [N] = 'n',
    [q] = 'q',
    [r] = 'r',
    [b] = 'b',
    [n] = 'n'
};

//pretty printing stuff (idk where to put it)
string coordinates [64] = {
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
  "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
  "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
  "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
  "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
  "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
  "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
  "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

//encode moves
inline uint32_t encode_moves(int sourcesq, int targetsq, int piece, int promote, int capture, int double_push, int enpassant, int castle){
	uint32_t return_var = sourcesq | (targetsq << 6) | (piece << 12) | (promote << 16) | (capture << 20) | (double_push << 21) | (enpassant << 22) | (castle << 23);
	return return_var;}

// move list structure
typedef struct {
    // moves
    int moves[256];
    
    // move count
    int count;
} moves;

class Pins{
	public:
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
	
	//ray masks for pins (index 9 is buffer if there aren't any pins)
	const uint64_t masks [9] [64] /*[direction] [square]*/ = {
		{0x101010101010100, 0x202020202020200, 0x404040404040400, 
	0x808080808080800, 0x1010101010101000, 0x2020202020202000, 
	0x4040404040404000, 0x8080808080808000, 0x101010101010000, 
	0x202020202020000, 0x404040404040000, 0x808080808080000, 
	0x1010101010100000, 0x2020202020200000, 0x4040404040400000, 
	0x8080808080800000, 0x101010101000000, 0x202020202000000, 
	0x404040404000000, 0x808080808000000, 0x1010101010000000, 
	0x2020202020000000, 0x4040404040000000, 0x8080808080000000, 
	0x101010100000000, 0x202020200000000, 0x404040400000000, 
	0x808080800000000, 0x1010101000000000, 0x2020202000000000, 
	0x4040404000000000, 0x8080808000000000, 0x101010000000000, 
	0x202020000000000, 0x404040000000000, 0x808080000000000, 
	0x1010100000000000, 0x2020200000000000, 0x4040400000000000, 
	0x8080800000000000, 0x101000000000000, 0x202000000000000, 
	0x404000000000000, 0x808000000000000, 0x1010000000000000, 
	0x2020000000000000, 0x4040000000000000, 0x8080000000000000, 
	0x100000000000000, 0x200000000000000, 0x400000000000000, 
	0x800000000000000, 0x1000000000000000, 0x2000000000000000, 
	0x4000000000000000, 0x8000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 
	0x0, 0x0, 0x0}/*North*/,

		{0x8040201008040200, 0x80402010080400, 0x804020100800, 
	0x8040201000, 0x80402000, 0x804000, 0x8000, 0x0, 
	0x4020100804020000, 0x8040201008040000, 0x80402010080000, 
	0x804020100000, 0x8040200000, 0x80400000, 0x800000, 0x0, 
	0x2010080402000000, 0x4020100804000000, 0x8040201008000000, 
	0x80402010000000, 0x804020000000, 0x8040000000, 0x80000000, 0x0, 
	0x1008040200000000, 0x2010080400000000, 0x4020100800000000, 
	0x8040201000000000, 0x80402000000000, 0x804000000000, 0x8000000000, 
	0x0, 0x804020000000000, 0x1008040000000000, 0x2010080000000000, 
	0x4020100000000000, 0x8040200000000000, 0x80400000000000, 
	0x800000000000, 0x0, 0x402000000000000, 0x804000000000000, 
	0x1008000000000000, 0x2010000000000000, 0x4020000000000000, 
	0x8040000000000000, 0x80000000000000, 0x0, 0x200000000000000, 
	0x400000000000000, 0x800000000000000, 0x1000000000000000, 
	0x2000000000000000, 0x4000000000000000, 0x8000000000000000, 0x0, 
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}/*North East*/,

		{0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x0, 0xfe00, 0xfc00, 
	0xf800, 0xf000, 0xe000, 0xc000, 0x8000, 0x0, 0xfe0000, 0xfc0000, 
	0xf80000, 0xf00000, 0xe00000, 0xc00000, 0x800000, 0x0, 0xfe000000, 
	0xfc000000, 0xf8000000, 0xf0000000, 0xe0000000, 0xc0000000, 
	0x80000000, 0x0, 0xfe00000000, 0xfc00000000, 0xf800000000, 
	0xf000000000, 0xe000000000, 0xc000000000, 0x8000000000, 0x0, 
	0xfe0000000000, 0xfc0000000000, 0xf80000000000, 0xf00000000000, 
	0xe00000000000, 0xc00000000000, 0x800000000000, 0x0, 
	0xfe000000000000, 0xfc000000000000, 0xf8000000000000, 
	0xf0000000000000, 0xe0000000000000, 0xc0000000000000, 
	0x80000000000000, 0x0, 0xfe00000000000000, 0xfc00000000000000, 
	0xf800000000000000, 0xf000000000000000, 0xe000000000000000, 
	0xc000000000000000, 0x8000000000000000, 0x0}/*East*/,

		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x4, 0x8, 0x10, 
	0x20, 0x40, 0x80, 0x0, 0x204, 0x408, 0x810, 0x1020, 0x2040, 0x4080, 
	0x8000, 0x0, 0x20408, 0x40810, 0x81020, 0x102040, 0x204080, 
	0x408000, 0x800000, 0x0, 0x2040810, 0x4081020, 0x8102040, 
	0x10204080, 0x20408000, 0x40800000, 0x80000000, 0x0, 0x204081020, 
	0x408102040, 0x810204080, 0x1020408000, 0x2040800000, 0x4080000000, 
	0x8000000000, 0x0, 0x20408102040, 0x40810204080, 0x81020408000, 
	0x102040800000, 0x204080000000, 0x408000000000, 0x800000000000, 
	0x0, 0x2040810204080, 0x4081020408000, 0x8102040800000, 
	0x10204080000000, 0x20408000000000, 0x40800000000000, 
	0x80000000000000, 0x0}/*South East*/,

		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x4, 0x8, 
	0x10, 0x20, 0x40, 0x80, 0x101, 0x202, 0x404, 0x808, 0x1010, 0x2020, 
	0x4040, 0x8080, 0x10101, 0x20202, 0x40404, 0x80808, 0x101010, 
	0x202020, 0x404040, 0x808080, 0x1010101, 0x2020202, 0x4040404, 
	0x8080808, 0x10101010, 0x20202020, 0x40404040, 0x80808080, 
	0x101010101, 0x202020202, 0x404040404, 0x808080808, 0x1010101010, 
	0x2020202020, 0x4040404040, 0x8080808080, 0x10101010101, 
	0x20202020202, 0x40404040404, 0x80808080808, 0x101010101010, 
	0x202020202020, 0x404040404040, 0x808080808080, 0x1010101010101, 
	0x2020202020202, 0x4040404040404, 0x8080808080808, 
	0x10101010101010, 0x20202020202020, 0x40404040404040, 
	0x80808080808080}/*South*/,

		{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x2, 0x4, 0x8, 
	0x10, 0x20, 0x40, 0x0, 0x100, 0x201, 0x402, 0x804, 0x1008, 0x2010, 
	0x4020, 0x0, 0x10000, 0x20100, 0x40201, 0x80402, 0x100804, 
	0x201008, 0x402010, 0x0, 0x1000000, 0x2010000, 0x4020100, 
	0x8040201, 0x10080402, 0x20100804, 0x40201008, 0x0, 0x100000000, 
	0x201000000, 0x402010000, 0x804020100, 0x1008040201, 0x2010080402, 
	0x4020100804, 0x0, 0x10000000000, 0x20100000000, 0x40201000000, 
	0x80402010000, 0x100804020100, 0x201008040201, 0x402010080402, 0x0, 
	0x1000000000000, 0x2010000000000, 0x4020100000000, 0x8040201000000, 
	0x10080402010000, 0x20100804020100, 0x40201008040201}/*South West*/,

		{0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0x0, 0x100, 0x300, 
	0x700, 0xf00, 0x1f00, 0x3f00, 0x7f00, 0x0, 0x10000, 0x30000, 
	0x70000, 0xf0000, 0x1f0000, 0x3f0000, 0x7f0000, 0x0, 0x1000000, 
	0x3000000, 0x7000000, 0xf000000, 0x1f000000, 0x3f000000, 
	0x7f000000, 0x0, 0x100000000, 0x300000000, 0x700000000, 
	0xf00000000, 0x1f00000000, 0x3f00000000, 0x7f00000000, 0x0, 
	0x10000000000, 0x30000000000, 0x70000000000, 0xf0000000000, 
	0x1f0000000000, 0x3f0000000000, 0x7f0000000000, 0x0, 
	0x1000000000000, 0x3000000000000, 0x7000000000000, 0xf000000000000, 
	0x1f000000000000, 0x3f000000000000, 0x7f000000000000, 0x0, 
	0x100000000000000, 0x300000000000000, 0x700000000000000, 
	0xf00000000000000, 0x1f00000000000000, 0x3f00000000000000, 
	0x7f00000000000000}/*West*/,

		{0x0, 0x100, 0x10200, 0x1020400, 0x102040800, 0x10204081000, 
	0x1020408102000, 0x102040810204000, 0x0, 0x10000, 0x1020000, 
	0x102040000, 0x10204080000, 0x1020408100000, 0x102040810200000, 
	0x204081020400000, 0x0, 0x1000000, 0x102000000, 0x10204000000, 
	0x1020408000000, 0x102040810000000, 0x204081020000000, 
	0x408102040000000, 0x0, 0x100000000, 0x10200000000, 
	0x1020400000000, 0x102040800000000, 0x204081000000000, 
	0x408102000000000, 0x810204000000000, 0x0, 0x10000000000, 
	0x1020000000000, 0x102040000000000, 0x204080000000000, 
	0x408100000000000, 0x810200000000000, 0x1020400000000000, 0x0, 
	0x1000000000000, 0x102000000000000, 0x204000000000000, 
	0x408000000000000, 0x810000000000000, 0x1020000000000000, 
	0x2040000000000000, 0x0, 0x100000000000000, 0x200000000000000, 
	0x400000000000000, 0x800000000000000, 0x1000000000000000, 
	0x2000000000000000, 0x4000000000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 
	0x0, 0x0, 0x0}/*North West*/,

		{0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 
0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff} /*For errors*/
	};

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
};

//direction of pins
int pin_direction [64] [64];
	
int direction(int sq1, int sq2){
	Pins pin;
	if(sq1 == sq2) {
		return 8; //error, not applicable
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
	return 8; //if there isn't a ray direction
}

class double_pawn_push{
	public:
	uint64_t double_pawn_push_on_the_fly(int square, bool side){
		const uint64_t Rank_4 = 0xff000000;
		const uint64_t Rank_5 = 0xff00000000;
		uint64_t Pawn = 1ULL << square;
		uint64_t Empty = ~occupancies[BOTH];
		uint64_t Pawn_Double_Push = (side == WHITE) ? (Pawn << 16) & Empty & (Empty << 8) & Rank_4 : (Pawn >> 16) & Empty & (Empty >> 8) & Rank_5;
		return Pawn_Double_Push;
	}
};

class Legal_Moves{
	private:
	const uint64_t rook_positions [4] = {0x1, 0x80, 0x100000000000000, 0x8000000000000000};

	const uint64_t castle_constants [4] = {0x60, 0xc, 0x6000000000000000, 0xc00000000000000};

	const uint64_t castle_king_constants [2] = {0x10, 0x1000000000000000};

	const uint64_t en_passant_ranks [2] = {0xff00000000, 0xff000000};

	//promotion ranks
	const uint64_t promotion_ranks [2] = {0xff00000000000000, 0xff};

	public:
	// add move to the move list
	static inline void add_move(moves *move_list, int move){
    // store move
    move_list->moves[move_list->count] = move;
    
    // increment move count
    move_list->count++;
	}

	void print_move_list(moves *move_list){
    // loop over moves within a move list
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        // init move
        int move = move_list->moves[move_count];
        
        std::cout << coordinates[get_move_source(move)];
				std::cout << coordinates[get_move_target(move)];
				std::cout << promoted_pieces[get_move_promoted(move)] << "\n";
        
    }
	// print total number of moves
	int total_moves = move_list->count;
  std::cout << "\nTotal number of moves: ";
	std::cout <<move_list->count;
}

	//Board move history 
	std::vector <int> history;

	//square attacked function (side is aggressor)
	bool is_square_attacked(int square, int Side){
		//Pawns
		uint64_t pawns = Bitboards[Side * 6];
		if (Pawn_Attacks[Side ^ 1][square] & pawns) return true;
		//Knights
		uint64_t knights = Bitboards[1 + Side * 6];
		if (Knight_Attacks[square] & knights) return true;
		//Bishops and Queens
		uint64_t bishopqueens = Bitboards[2 + Side * 6] | Bitboards[4 + Side * 6];
		if (get_bishop_attacks(square, occupancies[BOTH]) & bishopqueens) return true;
		//Rooks and Queens
		uint64_t rookqueens = Bitboards[3 + Side * 6] | Bitboards[4 + Side * 6];
		if (get_rook_attacks(square, occupancies[BOTH]) & rookqueens) return true;
		//Kings
		uint64_t kings = Bitboards[5 + Side * 6];
		if (King_Mask[square] & kings) return true;
		return false;}

	//Pieces that attack a square
	uint64_t square_attackers(int square, int Side){
		uint64_t attackers = 0ULL;
		//Pawns
		uint64_t pawns = Bitboards[Side * 6];
		attackers |= Pawn_Attacks[Side ^ 1][square] & pawns;
		//Knights
		uint64_t knights = Bitboards[1 + Side * 6];
		attackers |= Knight_Attacks[square] & knights;
		//Bishops and Queens
		uint64_t bishopqueens = Bitboards[2 + Side * 6] | Bitboards[4 + Side * 6];
		attackers |= get_bishop_attacks(square, occupancies[BOTH]) & bishopqueens;
		//Rooks and Queens
		uint64_t rookqueens = Bitboards[3 + Side * 6] | Bitboards[4 + Side * 6];
		attackers |= get_rook_attacks(square, occupancies[BOTH]) & rookqueens;
		//Kings
		uint64_t kings = Bitboards[5 + Side * 6];
		attackers |= King_Mask[square] & kings;
		return attackers;}

	//Attack map function. 'side' is the aggressor
	uint64_t attack_map(int side){
		uint64_t mapped_attacks = 0ULL;
		//treated queens as a seperate entity
		for(int square=0; square<64; square++){
			mapped_attacks |= (get_bit(Bitboards[side * 6], square) > 0) * Pawn_Attacks[side][square] + (get_bit(Bitboards[side * 6], square) <= 0) * 0; /*Pawns*/
			mapped_attacks |= (get_bit(Bitboards[1 + side * 6], square) > 0) * Knight_Attacks[square] + (get_bit(Bitboards[1 + side * 6], square) <= 0) * 0; /*Knights*/
			mapped_attacks |= (get_bit(Bitboards[2 + side * 6], square) > 0) * get_bishop_attacks(square, occupancies[BOTH]) + (get_bit(Bitboards[2 + side * 6], square) <= 0) * 0; /*Bishops*/
			mapped_attacks |= (get_bit(Bitboards[3 + side * 6], square) > 0)* get_rook_attacks(square, occupancies[BOTH]) + (get_bit(Bitboards[3 + side * 6], square) <= 0) * 0; /*Rooks*/
			mapped_attacks |= (get_bit(Bitboards[4 + side * 6], square) > 0) * get_queen_attacks(square, occupancies[BOTH]) + (get_bit(Bitboards[4 + side * 6], square) <= 0) * 0; /*Queens*/
			mapped_attacks |= (get_bit(Bitboards[5 + side * 6], square) > 0) * King_Mask[square] + (get_bit(Bitboards[5 + side * 6], square) <= 0) * 0;}
		return mapped_attacks;}

	//Almost same thing, difference is that this doesn't treat king as a blocker
	uint64_t king_danger_squares(int side){
		uint64_t mapped_attacks = 0ULL;
		//remove opponent king
		uint64_t blockers = occupancies[BOTH] ^ Bitboards[K + ((side ^ 1) * 6)];
		//treated queens as a seperate entity
		for(int square = 0; square < 64; square ++){
			mapped_attacks |= (get_bit(Bitboards[side * 6], square) > 0) * Pawn_Attacks[side][square] + (get_bit(Bitboards[side * 6], square) <= 0) * 0; /*Pawns*/
			mapped_attacks |= (get_bit(Bitboards[1 + side * 6], square) > 0) * Knight_Attacks[square] + (get_bit(Bitboards[1 + side * 6], square) <= 0) * 0; /*Knights*/
			mapped_attacks |= (get_bit(Bitboards[2 + side * 6], square) > 0) * get_bishop_attacks(square, blockers) + (get_bit(Bitboards[2 + side * 6], square) <= 0) * 0; /*Bishops*/
			mapped_attacks |= (get_bit(Bitboards[3 + side * 6], square) > 0)* get_rook_attacks(square, blockers) + (get_bit(Bitboards[3 + side * 6], square) <= 0) * 0; /*Rooks*/
			mapped_attacks |= (get_bit(Bitboards[4 + side * 6], square) > 0) * get_queen_attacks(square, blockers) + (get_bit(Bitboards[4 + side * 6], square) <= 0) * 0; /*Queens*/
			mapped_attacks |= (get_bit(Bitboards[5 + side * 6], square) > 0) * King_Mask[square] + (get_bit(Bitboards[5 + side * 6], square) <= 0) * 0;}
		return mapped_attacks;}

	//castling 
	//can castle kingside?
	bool castle_kingside(int side){
		//If king moved, set castle white or black to zero
		//If that rook has moved, return 0;
		if((Bitboards[K + side * 6] != castle_king_constants[side]) || ((Bitboards[R + side * 6] & rook_positions[1 + side * 2])) == 0){
			(side == WHITE) ? Castle_White_Kingside = 0 : Castle_Black_Kingside = 0;
			return 0;
		}
		
		//If there is a piece between the king and rook, return 0
		//If King is in check or piece attacking castle squares, return 0
		uint64_t attacks = attack_map(side ^ 1);
		if((attacks & castle_king_constants[side]) || (attacks & castle_constants[side * 2]) || (castle_constants[side] & occupancies[BOTH])){
			return 0;
		}
		return 1;
	}

	//can castle queenside?
	//literally duplicate of previous function for now.
	bool castle_queenside(int side){
		//If king moved, set castle white or black to zero
		//If that rook has moved, return 0;
		if((Bitboards[K + side * 6] != castle_king_constants[side]) || ((Bitboards[R + side * 6] & rook_positions[1 + side * 2])) == 0){
			(side == WHITE) ? Castle_White_Queenside = 0 : Castle_Black_Queenside = 0;
			return 0;
		}
		
		//If there is a piece between the king and rook, return 0
		//If King is in check or piece attacking castle squares, return 0
		//make it 100% for queenside
		uint64_t attacks = attack_map(side ^ 1);
		if((attacks & castle_king_constants[side]) || (attacks & castle_constants[1 + side * 2]) || (castle_constants[1 + side * 2] & occupancies[BOTH])){
			return 0;
		}
		return 1;
	}

	//En Passant. 'side' is the side looking for en passant
	uint64_t en_passant(int side){
		int targetsq;
		uint64_t en_passant_attacks;
		//if there is a pawn on the possible en passant rank
		if((Bitboards[(side ^ 1) * 6] & en_passant_ranks[side]) < 1){
			return 0;
		}
		//most recent move
		int most_recent_move = history.at(history.size() - 1);
		//make sure it is a double pawn push
		if(most_recent_move & 0x200000){
			//attempt to find en passant attacks
			//target square
			targetsq = (most_recent_move & 0xfc0) >> 6;
			//also shift target square to the square that the original side must attack to be considered en passant
			targetsq += (side == WHITE) * 8;
			targetsq -= (side == BLACK) * 8;
			//gets pawns with posssible en_passant attack
			en_passant_attacks = Pawn_Attacks[side ^ 1] [targetsq] & Bitboards[side * 6];
		} else {
			return 0;
		}
	return (en_passant_attacks > 0) * (1ULL << targetsq);
	}

	bool legal_en_passant(int side, int source, int target){
		//make copies of the affected bitboards by move
		uint64_t WP = Bitboards[P];
		uint64_t BP = Bitboards[p];
		//make moves (shift and replace source sqare pawn to target square for side)
		pop_bit(Bitboards[side * 6], source);
		set_bit(Bitboards[side * 6], target);
		//remove opposing pawn that has been targeted
		(side == WHITE) ? pop_bit(Bitboards[(side ^ 1) * 6], (target - 8)) : pop_bit(Bitboards[(side ^ 1) * 6], (target + 8));
		update_occupancies();
		if(is_square_attacked(get_ls1b_index(Bitboards[K + side * 6]), side ^ 1)){
			//reset the bitboards
			Bitboards[P] = WP;
			Bitboards[p] = BP;
			update_occupancies();
			return false;
		} else {
			//reset the bitboards
			Bitboards[P] = WP;
			Bitboards[p] = BP;
			update_occupancies();
			return true;	
		}
	}

	//Generate all legal moves for position
	//All legal move generation except for en passant
	//use Leorik as reference for generation method: 
	//https://github.com/lithander/Leorik/blob/master/Leorik.Perft/PerftMoveGen.cs
	void generate_moves(moves *move_list, bool side){
		//initialize objects
		double_pawn_push pawn;
		Pins pin;
		
		//Set move counter to zero
		move_list -> count = 0;
		
		//source and target squares
		int sourcesq, targetsq;

		//side constant
		const int stm = side * 6;
		
		//double check boolean
		bool double_check;

		//king square
		int kingsq = get_ls1b_index(Bitboards[K + side * 6]);

		std::vector <int> en_passant_targets;
		
		//pinned pieces
		uint64_t pinned = pin.absolute_pins(side, kingsq);		
		
		//The only possible replies to a double check are 	
		//king moves, as both checking pieces cannot be 		
		//captured or blocked at once.

		uint64_t king_attacked_by = this -> square_attackers(kingsq, side ^ 1);

		//capture move boolean
		bool capture;

		//pinmask
		uint64_t pinmask;

		//targets
		uint64_t targets;

		//checkmask
		uint64_t checkmask = ~uint64_t(0);
		
		switch(count_bits(king_attacked_by)){
			case 0: double_check = false; break;
			case 1: double_check = false; checkmask = Rect_Lookup[kingsq][get_ls1b_index(king_attacked_by)] | king_attacked_by; break;
			case 2: double_check = true; break;
		}
		if(double_check == false){
			//go through correct 'side' bitboards
			//King moves
			for(uint64_t King = Bitboards[K + stm]; King != 0; King &= King - 1){
				//get source square
				sourcesq = get_ls1b_index(King);
				//get targets for attacks
				targets = King_Mask[sourcesq] & ~occupancies[side] & ~king_danger_squares(side ^ 1);
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					add_move(move_list, encode_moves(sourcesq, targetsq, K + stm, 0, capture, 0, 0, 0));
				}
			}
			//castling moves
			if(castle_kingside(side)){
				add_move(move_list, encode_moves(e1 + side * 56, g1 + side * 56, K + stm, 0, 0, 0, 0, 1));
			}
			if(castle_queenside(side)){
				add_move(move_list, encode_moves(e1 + side * 56, c1 + side * 56, K + stm, 0, 0, 0, 0, 1));
			}
			
			//Queen moves
			for(uint64_t Queen = Bitboards[Q + stm]; Queen != 0; Queen &= Queen - 1){
				//get source square
				sourcesq = get_ls1b_index(Queen);
				//get pinmask
				pinmask = pin.masks[pin_direction[kingsq][(get_bit(Queen, sourcesq) & pinned) ? sourcesq : kingsq]][kingsq];
				//get targets for attacks
				targets = (get_queen_attacks(sourcesq, occupancies[BOTH]) & pinmask) & ~occupancies[side] & checkmask;
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					add_move(move_list, encode_moves(sourcesq, targetsq, Q + stm, 0, capture, 0, 0, 0));
				}
			}
			
			//Rook moves
			for(uint64_t Rook = Bitboards[R + stm]; Rook != 0; Rook &= Rook - 1){
				//get source square
				sourcesq = get_ls1b_index(Rook);
				//get pinmask
				pinmask = pin.masks[pin_direction[kingsq][(get_bit(Rook, sourcesq) & pinned) ? sourcesq : kingsq]][kingsq];
				//get targets for attacks
				targets = (get_rook_attacks(sourcesq, occupancies[BOTH]) & pinmask) & ~occupancies[side] & checkmask;
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					add_move(move_list, encode_moves(sourcesq, targetsq, R + stm, 0, capture, 0, 0, 0));
				}
			}
			
			//Bishop moves
			for(uint64_t Bishop = Bitboards[B + stm]; Bishop != 0; Bishop &= Bishop - 1){
				//get source square
				sourcesq = get_ls1b_index(Bishop);
				//get pinmask
				pinmask = pin.masks[pin_direction[kingsq][(get_bit(Bishop, sourcesq) & pinned) ? sourcesq : kingsq]][kingsq];
				//get targets for attacks
				targets = (get_bishop_attacks(sourcesq, occupancies[BOTH]) & pinmask) & ~occupancies[side] & checkmask;
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					add_move(move_list, encode_moves(sourcesq, targetsq, B + stm, 0, capture, 0, 0, 0));
				}
			}
			
			//Knight moves
			for(uint64_t Knight = Bitboards[N + stm]; Knight != 0; Knight &= Knight - 1){
				//get source square
				sourcesq = get_ls1b_index(Knight);
				//get pinmask
				pinmask = pin.masks[pin_direction[kingsq][(get_bit(Knight, sourcesq) & pinned) ? sourcesq : kingsq]][kingsq];
				//get targets for attacks
				targets = Knight_Attacks[sourcesq] & pinmask & ~occupancies[side] & checkmask;
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					add_move(move_list, encode_moves(sourcesq, targetsq, N + stm, 0, capture, 0, 0, 0));
				}
			}

			//Pawn moves
			for(uint64_t Pawn = Bitboards[stm]; Pawn != 0; Pawn &= Pawn -1){
				//get source square
				sourcesq = get_ls1b_index(Pawn);
				//get pinmask
				pinmask = pin.masks[pin_direction[kingsq][(get_bit(Pawn, sourcesq) & pinned) ? sourcesq : kingsq]][kingsq];
				targets = ((Pawn_Attacks[side][sourcesq] & occupancies[side ^ 1]) | (pawn_single_push[side][sourcesq] & ~occupancies[BOTH]) | pawn.double_pawn_push_on_the_fly(sourcesq, side)) & pinmask & checkmask;
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					if(get_bit(targets, targetsq) & promotion_ranks[side]){
						add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, N + stm, capture, 0, 0, 0));
						add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, B + stm, capture, 0, 0, 0));
						add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, R + stm, capture, 0, 0, 0));
						add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, Q + stm, capture, 0, 0, 0));
					} else{
						//quiet pawn push
						add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, 0, capture, 0, 0, 0));
					}
				}
			}	

			//en passant
			//get target square index
			targetsq = get_ls1b_index(en_passant(side));
			//get potential target pawns
			targets = Pawn_Attacks[side ^ 1][targetsq] & Bitboards[side * 6];
			if(count_bits(targets) == 2){
				sourcesq = get_ls1b_index(targets);
				if(legal_en_passant(side, sourcesq, targetsq)){
					add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, 0, 1, 0, 1, 0));
				}
				sourcesq = get_ms1b_index(targets);
				if(legal_en_passant(side, sourcesq, targetsq)){
					add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, 0, 1, 0, 1, 0));
				}
			} else if(count_bits(targets) == 1){
				sourcesq = get_ls1b_index(targets);
				if(legal_en_passant(side, sourcesq, targetsq)){
					add_move(move_list, encode_moves(sourcesq, targetsq, P + stm, 0, 1, 0, 1, 0));
				}
			}
			
		} else {
			//Only return King moves
			//King moves
			for(uint64_t King = Bitboards[K + stm]; King != 0; King &= King - 1){
				//get source square
				sourcesq = get_ls1b_index(King);
				//get targets for attacks
				targets = King_Mask[sourcesq] & ~occupancies[side] & ~king_danger_squares(side ^ 1);
				for(; targets != 0; targets &= targets - 1){
					targetsq = get_ls1b_index(targets);
					capture = ((get_bit(targets, targetsq) & occupancies[side ^ 1]) != 0);
					add_move(move_list, encode_moves(sourcesq, targetsq, K + stm, 0, capture, 0, 0, 0));
				}
			}
		}
	}

	//50 move rule
	int fifty_move_rule = 0;

	//add clear comments in it
	int push_move(int move, int move_flag){
		Zobrist Hash;
		if(!move_flag){
			int source_square = get_move_source(move);
      int target_square = get_move_target(move);
      int piece = get_move_piece(move);
      int promoted_piece = get_move_promoted(move);
      int capture = get_move_capture(move);
      int double_push = get_move_double(move);
      int enpassant = get_move_enpassant(move);
      int castling = get_move_castling(move);
			//find piece side 0 is White pawn, 6 is Black pawn
			int stm = ((int)floor(piece / 6) ^ 1) * 6;
      //move the piece
			pop_bit(Bitboards[piece], source_square);
			set_bit(Bitboards[piece], target_square);
			//update zobrist key
			Hash.Key ^= Polyglot_Random_U64[piece * 64 + source_square];
			Hash.Key ^= Polyglot_Random_U64[piece * 64 + target_square];
			//reset 50 move counter if it is a pawn move
			if(piece == P || piece == p){
				fifty_move_rule = 0;
			}
			//increase 50 move counter
			fifty_move_rule++;
			if(capture){
				
				//scan for correct bitboards
				for (int bb_piece = stm; bb_piece <= K + stm; bb_piece++){
					if(get_bit(Bitboards[bb_piece], target_square)){
						// remove it from corresponding bitboard
            pop_bit(Bitboards[bb_piece], target_square);
            // remove the piece from hash key
            Hash.Key ^= Polyglot_Random_U64[bb_piece * 64 + target_square];
            break;
					}
				}
			}
			if(promoted_piece){
				//reset 50 move counter
				fifty_move_rule = 0;
				//pop pawn bitboard
				pop_bit(Bitboards[piece], target_square);
				Hash.Key ^= Polyglot_Random_U64[piece * 64 + target_square];
				//update piece onto promoted bitboard
				set_bit(Bitboards[promoted_piece], target_square);
				Hash.Key ^= Polyglot_Random_U64[promoted_piece * 64 + target_square];
			}
			if(enpassant){
				int removal_square = (stm/6 == WHITE) ? 
														target_square - 8:
														target_square + 8;
				//remove victim pawn
				pop_bit(Bitboards[((stm/6) ^ 1) * 6], removal_square);
				Hash.Key ^= Polyglot_Random_U64[((stm/6) ^ 1) * 6 * 64 + removal_square];
			}
			uint64_t en_passant_attack = en_passant(stm / 6);
			if(en_passant_attack){
				Hash.Key ^= Polyglot_Random_U64[772 + get_ls1b_index(en_passant_attack) & 7];
			}
			if(double_push){
				en_passant_attack = en_passant((stm / 6) ^ 1);
				if(en_passant_attack){
					Hash.Key ^= Polyglot_Random_U64[772 + get_ls1b_index(en_passant_attack) & 7];
				}
			}
			if(castling){
				if(target_square == (g1 + (stm == 6) * 56)){
					pop_bit(Bitboards[R + stm], h1 + (stm == 6) * 56);
          set_bit(Bitboards[R + stm], f1 + (stm == 6) * 56);
					Hash.Key ^= Polyglot_Random_U64[(R + stm) * 64 + (h1 + (stm == 6) * 56)];
					Hash.Key ^= Polyglot_Random_U64[(R + stm) * 64 + (f1 + (stm == 6) * 56)];
					//reset castling rights
					Castle_White_Kingside = 0;
					Castle_White_Queenside = 0;
					Castle_Black_Kingside = 0;
					Castle_Black_Queenside = 0;
				}

				if(target_square == (c1 + (stm == 6) * 56)){
					pop_bit(Bitboards[R + stm], a1 + (stm == 6) * 56);
          set_bit(Bitboards[R + stm], d1 + (stm == 6) * 56);
					Hash.Key ^= Polyglot_Random_U64[(R + stm) * 64 + (a1 + (stm == 6) * 56)];
					Hash.Key ^= Polyglot_Random_U64[(R + stm) * 64 + (d1 + (stm == 6) * 56)];
					//reset castling rights
					Castle_White_Kingside = 0;
					Castle_White_Queenside = 0;
					Castle_Black_Kingside = 0;
					Castle_Black_Queenside = 0;
				}
			}
			//update occupancies
			update_occupancies();
			moves move_list[1];
			
		}
	}
};

inline void Initialize_Everything(){
	ChessBoard Init;
	//initialize Bitboard array (really ugly code)
	Bitboards [P] = Init.Initialize(Board,'P');
	Bitboards [N] = Init.Initialize(Board,'N');
	Bitboards [B] = Init.Initialize(Board,'B');
	Bitboards [R] = Init.Initialize(Board,'R');
	Bitboards [Q] = Init.Initialize(Board,'Q');
	Bitboards [K] = Init.Initialize(Board,'K');
	Bitboards [p] = Init.Initialize(Board,'p');
	Bitboards [n] = Init.Initialize(Board,'n');
	Bitboards [b] = Init.Initialize(Board,'b');
	Bitboards [r] = Init.Initialize(Board,'r');
	Bitboards [q] = Init.Initialize(Board,'q');
	Bitboards [k] = Init.Initialize(Board,'k');
	init_sliders_attacks(1);
	init_sliders_attacks(0);
	uint64_t bitboard;
	for (int square = 0; square < 64; square++){
    //Pawns
    Pawn_Attacks[WHITE][square] = mask_Pawn_Attacks(WHITE, square);
    Pawn_Attacks[BLACK][square] = mask_Pawn_Attacks(BLACK, square);
		//Knights
		bitboard = 1ULL << square;
		Knight_Attacks [square] = Knights(bitboard);
		}
	//pin directions
	for(int sq1 = 0; sq1 < 64; sq1++){
		for(int sq2 = 0; sq2 < 64; sq2++){
			pin_direction[sq1][sq2] = direction(sq1, sq2);
		}
	}
	//populate pawn push tables (gets target square)
	for(int square = 0; square < 64; square++){
		pawn_single_push [0] [square] = (square > 7 && square < 56) ? 1ULL << (square + 8) : 0;
		pawn_single_push [1] [square] = (square > 7 && square < 56) ? 1ULL << (square - 8) : 0;
	}
	//rectangular lookup table for pins
	for(int sq1=0; sq1<64; sq1++){
		for(int sq2=0; sq2<64; sq2++){
			Rect_Lookup [sq1] [sq2] = inBetween(sq1, sq2);
		}
	}
}