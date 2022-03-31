#include <iostream>
#include <cmath>
#include <vector>

#include "attacks.h"

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
            Rook_Attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);}}}}

//Directions for pins
enum directions {N_, NE, E_, SE, S_, SW, W_, NW};

enum Piece_Types {P, N, B, R, Q, K, p, n, b, r, q, k};
enum colors{WHITE, BLACK, BOTH};
uint64_t Bitboards [12];

//Occupancies
uint64_t occupancies[3];
void update_occupancies(){
	//Update Bitboard array (really ugly code)
	Bitboards [0]=WP;
	Bitboards [1]=WN;
	Bitboards [2]=WB;
	Bitboards [3]=WR;
	Bitboards [4]=WQ;
	Bitboards [5]=WK;
	Bitboards [6]=BP;
	Bitboards [7]=BN;
	Bitboards [8]=BB;
	Bitboards [9]=BR;
	Bitboards[10]=BQ;
	Bitboards[11]=BK;
	uint64_t to_occupancy1 = 0ULL;
	uint64_t to_occupancy2 = 0ULL;
	uint64_t to_occupancy3 = 0ULL;
	for(int i=6; i>0; i--){
		to_occupancy1 |= Bitboards[i];
		to_occupancy2 |= Bitboards[i+6];
		to_occupancy3 |= to_occupancy1 | to_occupancy2;}
	occupancies[0] = to_occupancy1;
	occupancies[1] = to_occupancy2;
	occupancies[2] = to_occupancy3;}

//Castling rights
int Castle_White = 2;
int Castle_Black = 2;

class Board_States{
	private:
		const uint64_t rook_positions [4] = {0x1, 0x80, 0x100000000000000, 0x8000000000000000};

		const uint64_t castle_constants [4] = {0x70, 0x1c, 0x7000000000000000, 0x1c00000000000000};

		const uint64_t castle_king_constants [2] = {0x81, 0x8100000000000000};

		const uint64_t en_passant_ranks [2] = {0xff00000000, 0xff000000};

	public:
	//Piece Masks for pins
	static uint64_t masks;

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

	//Pieces that attack a square (A work in progress)
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
	//Insert branchless programming
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

	/*The absolute pins to return map of pins on kings
	https://chess.stackexchange.com/questions/25137/chess-engine-generate-a-bitboard-of-pinned-pieces*/
	//Definitely not done
	uint64_t absolute_pins(int rooksq, int side){
		uint64_t vertical_attackers = Bitboards[R + side * 6] | Bitboards[B + side * 6] | Bitboards[Q + side * 6];
		if(vertical_attackers == 0){
    	return 0;}
		uint64_t bb_pinners = get_queen_attacks(0, 0) & occupancies[side];
		return 0;}
	//castling 
	int can_castle(int side){
		//0=No Castle, 1=King side castling, 2=Queen castling, 3=All castling methods avaliable
		//reset the temporary variables
		int Castle_White_Temp = 0;
	  int Castle_Black_Temp = 0;
		//enemy attacks
		uint64_t attacks = attack_map(side ^ 1);
		//if king has moved, automatically return zero
		if(Bitboards[K + side * 6] != 0x10){
			//Set permanent castling rights to zero for the rest of the game
			(side == WHITE) ? Castle_White = 0 : Castle_Black = 0;
			return 0;}
		//if King is attacked
		if(Bitboards[K + side * 6] & attacks){
			return 0;
		}
		//if it sees that rook positions are different from suposed starting position, remove that possibility
		//check to make sure no rooks have been moved or captured
		if (side == WHITE){
			Castle_White -= ((Bitboards[R] & rook_positions[0]) != 1) + ((Bitboards[R] & rook_positions[1]) != 128);}
		else{
			Castle_Black -= ((Bitboards[r] & rook_positions[2]) != 0x100000000000000) + ((Bitboards[r] & rook_positions[3]) != 0x8000000000000000);}
		//The two conditions below are temporary, if that issue doesn't exist anymore, then we should give back these rights
		//If there is any piece on the castling map, also remove possibility
		//Integrate this with the previous if else statement
		if (side == WHITE){
			Castle_White_Temp = ((castle_constants[0] & occupancies[BOTH]) > 0) + ((castle_constants[0] & occupancies[BOTH]) <= 0) * -1;
		} else {
			Castle_Black_Temp = ((castle_constants[2] & occupancies[BOTH]) > 0) + ((castle_constants[3] & occupancies[BOTH]) > 0);
		}
		//If there attacks from other side on the squares, remove particular castle right temporarily
		return 0;
	}

	//En Passant. 'side' is the side looking for en passant
	uint64_t en_passant(int side){
		int target_square;
		uint64_t en_passant_attacks;
		//if there is a pawn on the possible en passant rank
		if((Bitboards[(side ^ 1) * 6] & en_passant_ranks[side])<1){return 0;}
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
			return 0;}
	return en_passant_attacks;
	}
};