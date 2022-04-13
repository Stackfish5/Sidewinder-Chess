#include <iostream>
#include <cmath>
#include "attack_var.h"
#include "occupancies.h"

uint64_t mask_Pawn_Attacks(int side, int square){
    uint64_t attacks = 0ULL;
    uint64_t bitboard = 0ULL;
    // set piece on board
    set_bit(bitboard, square);
    // white pawns
    if (!side){
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);}
    // black pawns
    else{
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);}
    // return attack map
    return attacks;}



//For bit-shifting stuff later on
const int bishop_relevant_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 9, 9, 7, 5, 5, 
    5, 5, 7, 7, 7, 7, 5, 5, 
    5, 5, 5, 5, 5, 5, 5, 5, 
    6, 5, 5, 5, 5, 5, 5, 6};

const int rook_relevant_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    11, 10, 10, 10, 10, 10, 10, 11, 
    12, 11, 11, 11, 11, 11, 11, 12};

// get bishop attacks
static inline uint64_t get_bishop_attacks(int square, uint64_t occupancy){
    occupancy &= bishop_masks[square];
    occupancy *= BMagic[square];
    occupancy >>= 64 - bishop_relevant_bits[square];
    // return bishop attacks
    return Bishop_Attacks[square][occupancy];}

// get rook attacks
static inline uint64_t get_rook_attacks(int square, uint64_t occupancy){
    occupancy &= rook_masks[square];
    occupancy *= RMagic[square];
    occupancy >>= 64 - rook_relevant_bits[square];
    // return rook attacks
    return Rook_Attacks[square][occupancy];}
//End of borrowed code

static inline uint64_t get_queen_attacks(int square, uint64_t occupancy){
    return get_rook_attacks(square, occupancy) | get_bishop_attacks(square, occupancy);}

//Knight Attacks
uint64_t Knights(uint64_t Knights){
	uint64_t l1 = (Knights >> 1) & (0x7f7f7f7f7f7f7f7f);
  uint64_t l2 = (Knights >> 2) & (0x3f3f3f3f3f3f3f3f);
  uint64_t r1 = (Knights << 1) & (0xfefefefefefefefe);
  uint64_t r2 = (Knights << 2) & (0xfcfcfcfcfcfcfcfc);
  uint64_t h1 = l1 | r1;
  uint64_t h2 = l2 | r2;
  uint64_t result = ((h1<<16) | (h1>>16) | (h2<<8) | (h2>>8));
	return result;}