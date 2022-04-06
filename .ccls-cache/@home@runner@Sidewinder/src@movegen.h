#include <iostream>
#include <cmath>
#include <vector>

#include "attacks.h"

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

	//rectangular lookup table for pins
	for(int sq1=0; sq1<64; sq1++){
		for(int sq2=0; sq2<64; sq2++){
			Rect_Lookup [sq1] [sq2] = inBetween(sq1, sq2);
		}
	}}

enum Piece_Types {P, N, B, R, Q, K, p, n, b, r, q, k};

enum colors{WHITE, BLACK, BOTH};

uint64_t Bitboards [13];

//Occupancies
uint64_t occupancies[3];
void update_occupancies(){
	//Update Bitboard array (really ugly code)
	Bitboards [0] = WP;
	Bitboards [1] = WN;
	Bitboards [2] = WB;
	Bitboards [3] = WR;
	Bitboards [4] = WQ;
	Bitboards [5] = WK;
	Bitboards [6] = BP;
	Bitboards [7] = BN;
	Bitboards [8] = BB;
	Bitboards [9] = BR;
	Bitboards[10] = BQ;
	Bitboards[11] = BK;
	Bitboards[12] = ~uint64_t(0);
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

class Legal_Moves{
	private:
	const uint64_t rook_positions [4] = {0x1, 0x80, 0x100000000000000, 0x8000000000000000};

	const uint64_t castle_constants [4] = {0x70, 0x1c, 0x7000000000000000, 0x1c00000000000000};

	const uint64_t castle_king_constants [2] = {0x10, 0x1000000000000000};

	const uint64_t en_passant_ranks [2] = {0xff00000000, 0xff000000};

	public:
	const uint64_t masks [8] [64] /*[direction] [square]*/ = {
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
	0x0, 0x0, 0x0}/*North West*/
	};


	//Board move history 
	std::vector <int> history;

	//square attacked function
	bool is_square_attacked(int square, int Side){
		uint64_t attacked_bitboard = 1 << square;
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
		uint64_t attackers = 0;
		uint64_t attacked_bitboard = 1 << square;
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

	
	//castling 
	//can castle kingside?
	bool castle_kingside(int side){
		//If king moved, set castle white or black to zero
		if(Bitboards[K + side * 6] != castle_king_constants[side]){
			(side == WHITE) ? Castle_White_Kingside = 0 : Castle_Black_Kingside = 0;
			return 0;
		}
		//If that rook has moved, return 0;
		if((Bitboards[3 + side * 6] & rook_positions[side]) == 0){
			(side == WHITE) ? Castle_White_Kingside = 0 : Castle_Black_Kingside = 0;
			return 0;
		}
		//If there is a piece between the king and rook, return 0
		if(castle_constants[side] & occupancies[BOTH]){
			return 0;
		}
		//If King is in check or piece attacking castle squares, return 0
		uint64_t attacks = attack_map(side ^ 1);
		if((attacks & castle_king_constants[side]) || (attacks & castle_constants[side])){
			return 0;
		}
		return 1;
	}

	//En Passant. 'side' is the side looking for en passant
	uint64_t en_passant(int side){
		int target_square;
		uint64_t en_passant_attacks;
		//if there is a pawn on the possible en passant rank
		if((Bitboards[(side ^ 1) * 6] & en_passant_ranks[side])<1){
			return 0;
		}
		//most recent move
		int most_recent_move = history.at(history.size() - 1);
		//make sure it is a double pawn push
		if(most_recent_move & 0x200000){
			//attempt to find en passant attacks
			//target square
			target_square = (most_recent_move & 0xfc0) >> 6;
			//also shift target square to the square original side must attack to be considered en passant
			target_square += (side == WHITE) * 8;
			target_square -= (side == BLACK) * 8;
			//gets pawns with posssible en_passant attack
			en_passant_attacks = Pawn_Attacks[side ^ 1] [target_square] & Bitboards[side * 6];
		} else {
			return 0;
		}
	return (en_passant_attacks > 0) * (1ULL << target_square);
	}

	//Generate all legal moves for position
	//All legal move generation except for en passant
	void legal_moves(){
		return 0;
	}
};

inline void Initialize_Everything(){
	init_sliders_attacks(1);
	init_sliders_attacks(0);
	uint64_t bitboard;
	for (int square = 0; square < 64; square++){
    //Pawns
    Pawn_Attacks[WHITE][square] = mask_Pawn_Attacks(WHITE, square);
    Pawn_Attacks[BLACK][square] = mask_Pawn_Attacks(BLACK, square);
		//Knights
		bitboard = 1ULL << square;
		Knight_Attacks [square] = Knights(bitboard);}}