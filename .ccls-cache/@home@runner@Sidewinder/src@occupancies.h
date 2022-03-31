#include <iostream>
#include <cmath>
#include "functions.h"

// generate bishop attacks on the fly
uint64_t bishop_attacks_on_the_fly(int square, uint64_t block){
    // result attacks bitboard
    uint64_t attacks = 0ULL;
    // init ranks & files
    int r, f;
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    // generate bishop atacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }
    // return attack map
    return attacks;
}

// generate rook attacks on the fly
uint64_t rook_attacks_on_the_fly(int square, uint64_t block){
    // result attacks bitboard
    uint64_t attacks = 0ULL;
    // init ranks & files
    int r, f;
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    // generate rook attacks
    for (r = tr + 1; r <= 7; r++){
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    for (r = tr - 1; r >= 0; r--){
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }
    for (f = tf + 1; f <= 7; f++){
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    for (f = tf - 1; f >= 0; f--){
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    // return attack map
    return attacks;
}

uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask){
    // occupancy map
    uint64_t occupancy = 0ULL;
    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++){
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);
        // pop LS1B in attack map
        pop_bit(attack_mask, square);
        // make sure occupancy is on board
        if (index & (1 << count)){
            // populate occupancy map
            occupancy |= (1ULL << square);}
    }// return occupancy map
    return occupancy;}



//Look-up tables and some preperation
uint64_t bishop_masks[64];
uint64_t rook_masks	 [64];

// mask bishop attacks
uint64_t mask_bishop_attacks(int square){
    // result attacks bitboard
    uint64_t attacks = 0ULL;
    // init ranks & files
    int r, f;
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    // mask relevant bishop occupancy bits
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));
    // return attack map
    return attacks;}

// mask rook attacks
uint64_t mask_rook_attacks(int square){
    // result attacks bitboard
    uint64_t attacks = 0ULL;
    // init ranks & files
    int r, f;
    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;
    // mask relevant rook occupancy bits
    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));
    // return attack map
    return attacks;}