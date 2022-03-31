#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <cmath>
#include "Zobrist.h"
#include "Evaluation.h"
using std::to_string;
using std::string;
using std::uint64_t;
using std::uint32_t;

#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)
#define get_bit(bitboard, square) (bitboard & (1ULL << square))

//board
char Board[64] = {' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ','K',' ','n',' ','q',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' '};

//In which a1=0, h1=7, a8=56, and h8=63
enum enumSquare {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8};

//converting all the stuff
unsigned long long convert(int i){
	i=63-i;
	unsigned long long intermediate=(unsigned long long)floor(i/8+1)*8-i%8-1;
	return intermediate;}

//Reverse the LERBEF format to LERF for proper use (Dislaying purposes, not rotated biboards)
uint64_t Reverse (uint64_t x) {
   const uint64_t k1 = 0x5555555555555555;
   const uint64_t k2 = 0x3333333333333333;
   const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
   x = ((x >> 1) & k1) | ((x & k1) << 1);
   x = ((x >> 2) & k2) | ((x & k2) << 2);
   x = ((x >> 4) & k4) | ((x & k4) << 4);
   return x;}

//Bitboard variables initialized
uint64_t WP;
uint64_t WN;
uint64_t WB;
uint64_t WR;
uint64_t WQ;
uint64_t WK;
uint64_t BP;
uint64_t BN;
uint64_t BB;
uint64_t BR;
uint64_t BQ;
uint64_t BK;

class ChessBoard {
	public:
	//draw the board
	void Board(){
        string chessboard[8][8]={};
        WP=Reverse(WP);
				WN=Reverse(WN);
				WB=Reverse(WB);
				WR=Reverse(WR);
				WQ=Reverse(WQ);
				WK=Reverse(WK);
				BP=Reverse(BP);
				BN=Reverse(BN);
				BB=Reverse(BB);
				BR=Reverse(BR);
				BQ=Reverse(BQ);
				BK=Reverse(BK);
				for (int i=0;i<64;i++) {
        	if (((BP>>i)&1)==1) {chessboard[i/8][i%8]="\u2659";}
        	if (((BN>>i)&1)==1) {chessboard[i/8][i%8]="\u2658";}
        	if (((BB>>i)&1)==1) {chessboard[i/8][i%8]="\u2657";}
       	  if (((BR>>i)&1)==1) {chessboard[i/8][i%8]="\u2656";}
        	if (((BQ>>i)&1)==1) {chessboard[i/8][i%8]="\u2655";}
        	if (((BK>>i)&1)==1) {chessboard[i/8][i%8]="\u2654";}
        	if (((WP>>i)&1)==1) {chessboard[i/8][i%8]="\u265F";}
        	if (((WN>>i)&1)==1) {chessboard[i/8][i%8]="\u265E";}
        	if (((WB>>i)&1)==1) {chessboard[i/8][i%8]="\u265D";}
        	if (((WR>>i)&1)==1) {chessboard[i/8][i%8]="\u265C";}
        	if (((WQ>>i)&1)==1) {chessboard[i/8][i%8]="\u265B";}
        	if (((WK>>i)&1)==1) {chessboard[i/8][i%8]="\u265A";}}
					for(int i=64; i>0;i--){
						if(i%8==0 && i!=64){
							std::cout<<"\n"<<floor(i/8)<<" ";
						}else if (i%8==0){
							std::cout<<"\n"<<floor(i/8);
						} if(i/8==8){std::cout<<" ";}
				  	if(chessboard[i/8][i%8-1]!=""){
							std::cout<<chessboard[i/8][i%8-1];
					  }else{
							std::cout<<".";}std::cout<<" ";}
				std::cout<<"\n  a b c d e f g h\n";}

	//For experimental purposes in testing (really useful)
	void Test_Board(uint64_t mask){
		string chessboard[8][8]={};
      for (int i=0;i<64;i++) {
				uint64_t Bit_Mask = Reverse(mask);
       	if (((Bit_Mask>>i)&1)==1) {chessboard[i/8][i%8]="1";
				}else {chessboard[i/8][i%8]=".";}}
			for(int i=64; i>0;i--){
				if(i%8==0 && i!=64){
					std::cout<<"\n"<<floor(i/8)<<" ";
				}else if (i%8==0){
					std::cout<<"\n"<<floor(i/8);
				} if(i/8==8){std::cout<<" ";}
			 	if(chessboard[i/8][i%8-1]!=""){
					std::cout<<chessboard[i/8][i%8-1];
			  }else{
					std::cout<<"0";}std::cout<<" ";}
			std::cout<<"\n  a b c d e f g h\n";}
};
						
//initialize bitboard with inputs
uint64_t Initialize_Bits(char chessboard[64],char type){
	uint64_t Bit=0;
	for(int i=0;i<64;i++){
		Bit^=Reverse(1ull<<(63-i))*(chessboard[i]==type);}
	return Bit;}

//Integer to string conversion
string int_to_str(int input){
	string to_str=to_string(floor(input));
	to_str.erase ( to_str.find_last_not_of('0') + 1, string::npos );
	to_str.erase ( to_str.find_last_not_of('.') + 1, string::npos );
  return to_str;}

//constants
const uint64_t FILE_A=0x1010101010101010;
const uint64_t FILE_H=0x8080808080808080;
const uint64_t RANK_8=0xFF00000000000000;
const uint64_t RANK_1=0x00000000000000FF;
const uint64_t RANK_4=0xFF000000;
const uint64_t RANK_5=0xFF00000000;

//Count trailing zeros
int trailing_zeros(int n) {
	int count = 0;
	while (n > 0 && (n & 1) == 0) {
		count = count + 1;
		n = n >> 1;
		}return count;}

//Scan bits, FLAG is for promotions, en passant and castling
string Scan_Bits(uint64_t input, int offset, string FLAG){
 string output="";
 string into_list;
 int x;
 for(int i=0;i<64;i++){ 
	x=i;
	if (((input>>x)&1)==1){	into_list = int_to_str((x - offset) % 8 + 1) + int_to_str((x - offset) / 8 + 1) + int_to_str(x % 8 + 1) + int_to_str(x / 8 + 1) + FLAG;
		output.append(into_list+"\n");}}
return output;}	

//Pawn Attacks (White)
//Pre-calculated (Side and square)
uint64_t Pawn_Attacks [2] [64];

const uint64_t not_a_file = 0xfefefefefefefefe;
const uint64_t not_h_file = 0x7f7f7f7f7f7f7f7f;

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
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);    }
    // return attack map
    return attacks;}

//On the fly
string WP_Attacks(uint64_t White_Pawns, uint64_t Black_Pieces, uint64_t Empty){
	string return_string;
	uint64_t PAWN_MOVES;
	//Capture to the right
	PAWN_MOVES=(White_Pawns<<9)&Black_Pieces&~RANK_8&~FILE_A;
	return_string += Scan_Bits(PAWN_MOVES,9,"0");
	//Capture to the left
	PAWN_MOVES=(White_Pawns<<7)&Black_Pieces&~RANK_8&~FILE_H;
	return_string += Scan_Bits(PAWN_MOVES,7,"0");
	//Move forward one square
	PAWN_MOVES=(White_Pawns<<8)&Empty&~RANK_8;
	return_string += Scan_Bits(PAWN_MOVES,8,"0");
	//Move forward two squares
	PAWN_MOVES=(White_Pawns<<16)&Empty&(Empty<<8)&RANK_4;
	return_string += Scan_Bits(PAWN_MOVES,16,"0");
	//Pawn promotion by capturing right
	PAWN_MOVES=(White_Pawns<<9)&Black_Pieces&RANK_8&~FILE_A;
	return_string += Scan_Bits(PAWN_MOVES,9,"q") + Scan_Bits(PAWN_MOVES,9,"r") + Scan_Bits(PAWN_MOVES,9,"b") + Scan_Bits(PAWN_MOVES,9,"n");
	//Pawn promotion by capturing left
	PAWN_MOVES=(White_Pawns<<7)&Black_Pieces&RANK_8&~FILE_H;
	return_string += Scan_Bits(PAWN_MOVES,7,"q") + Scan_Bits(PAWN_MOVES,7,"r") + Scan_Bits(PAWN_MOVES,7,"b") + Scan_Bits(PAWN_MOVES,7,"n");
	//Pawn promotion by moving forward
	PAWN_MOVES=(White_Pawns<<8)&Empty&RANK_8;
	return_string += Scan_Bits(PAWN_MOVES,8,"q") + Scan_Bits(PAWN_MOVES,8,"r") + Scan_Bits(PAWN_MOVES,8,"b") + Scan_Bits(PAWN_MOVES,8,"n");
	//En Passant (not going to worry about it right now)
	
	return return_string;}
//Implemented from Maksim Korzh's Bit Board Chess
//Rook Magic Numbers
const uint64_t RMagic[64] = {
  0x8a80104000800020ULL,
  0x140002000100040ULL,
  0x2801880a0017001ULL,
  0x100081001000420ULL,
  0x200020010080420ULL,
  0x3001c0002010008ULL,
  0x8480008002000100ULL,
  0x2080088004402900ULL,
  0x800098204000ULL,
  0x2024401000200040ULL,
  0x100802000801000ULL,
  0x120800800801000ULL,
  0x208808088000400ULL,
  0x2802200800400ULL,
  0x2200800100020080ULL,
  0x801000060821100ULL,
  0x80044006422000ULL,
  0x100808020004000ULL,
  0x12108a0010204200ULL,
  0x140848010000802ULL,
  0x481828014002800ULL,
  0x8094004002004100ULL,
  0x4010040010010802ULL,
  0x20008806104ULL,
  0x100400080208000ULL,
  0x2040002120081000ULL,
  0x21200680100081ULL,
  0x20100080080080ULL,
  0x2000a00200410ULL,
  0x20080800400ULL,
  0x80088400100102ULL,
  0x80004600042881ULL,
  0x4040008040800020ULL,
  0x440003000200801ULL,
  0x4200011004500ULL,
  0x188020010100100ULL,
  0x14800401802800ULL,
  0x2080040080800200ULL,
  0x124080204001001ULL,
  0x200046502000484ULL,
  0x480400080088020ULL,
  0x1000422010034000ULL,
  0x30200100110040ULL,
  0x100021010009ULL,
  0x2002080100110004ULL,
  0x202008004008002ULL,
  0x20020004010100ULL,
  0x2048440040820001ULL,
  0x101002200408200ULL,
  0x40802000401080ULL,
  0x4008142004410100ULL,
  0x2060820c0120200ULL,
  0x1001004080100ULL,
  0x20c020080040080ULL,
  0x2935610830022400ULL,
  0x44440041009200ULL,
  0x280001040802101ULL,
  0x2100190040002085ULL,
  0x80c0084100102001ULL,
  0x4024081001000421ULL,
  0x20030a0244872ULL,
  0x12001008414402ULL,
  0x2006104900a0804ULL,
  0x1004081002402ULL};

//Bishop Magic Numbers
const uint64_t BMagic[64] = {
  0x40040844404084ULL,
  0x2004208a004208ULL,
  0x10190041080202ULL,
  0x108060845042010ULL,
  0x581104180800210ULL,
  0x2112080446200010ULL,
  0x1080820820060210ULL,
  0x3c0808410220200ULL,
  0x4050404440404ULL,
  0x21001420088ULL,
  0x24d0080801082102ULL,
  0x1020a0a020400ULL,
  0x40308200402ULL,
  0x4011002100800ULL,
  0x401484104104005ULL,
  0x801010402020200ULL,
  0x400210c3880100ULL,
  0x404022024108200ULL,
  0x810018200204102ULL,
  0x4002801a02003ULL,
  0x85040820080400ULL,
  0x810102c808880400ULL,
  0xe900410884800ULL,
  0x8002020480840102ULL,
  0x220200865090201ULL,
  0x2010100a02021202ULL,
  0x152048408022401ULL,
  0x20080002081110ULL,
  0x4001001021004000ULL,
  0x800040400a011002ULL,
  0xe4004081011002ULL,
  0x1c004001012080ULL,
  0x8004200962a00220ULL,
  0x8422100208500202ULL,
  0x2000402200300c08ULL,
  0x8646020080080080ULL,
  0x80020a0200100808ULL,
  0x2010004880111000ULL,
  0x623000a080011400ULL,
  0x42008c0340209202ULL,
  0x209188240001000ULL,
  0x400408a884001800ULL,
  0x110400a6080400ULL,
  0x1840060a44020800ULL,
  0x90080104000041ULL,
  0x201011000808101ULL,
  0x1a2208080504f080ULL,
  0x8012020600211212ULL,
  0x500861011240000ULL,
  0x180806108200800ULL,
  0x4000020e01040044ULL,
  0x300000261044000aULL,
  0x802241102020002ULL,
  0x20906061210001ULL,
  0x5a84841004010310ULL,
  0x4010801011c04ULL,
  0xa010109502200ULL,
  0x4a02012000ULL,
  0x500201010098b028ULL,
  0x8040002811040900ULL,
  0x28000010020204ULL,
  0x6000020202d0240ULL,
  0x8918844842082200ULL,
  0x4010011029020020ULL};

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

static inline int count_bits(uint64_t bitboard){
    // bit counter
    int count = 0;
    // consecutively reset least significant 1st bit
    while (bitboard){
        // increment count
        count++;
        // reset least significant 1st bit
        bitboard &= bitboard - 1;
    }// return bit count
    return count;
}

static inline int get_ls1b_index(uint64_t bitboard){
    // make sure bitboard is not 0
    if (bitboard){
        // count trailing bits before LS1B
        return count_bits((bitboard & -bitboard) - 1);
    }//otherwise
    else
        // return illegal index
        return -1;}

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

//Look-up tables and some preperation
uint64_t bishop_masks[64];
uint64_t rook_masks	 [64];

uint64_t Rook_Attacks [64]  [4096];
uint64_t Bishop_Attacks [64] [512];

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

//Bishop Attacks
string Bishop_Moves(uint64_t BISHOP, uint64_t OCCUPIED, uint64_t Friendly){
	string return_string;
	// Find position of Bishops
	for(int x=0; x<64-trailing_zeros(BISHOP);x++){
		x=convert(x);
		uint64_t BISHOP_MOVES=0;
	}
	return return_string;
}

//Knight Attacks
uint64_t Knight_Attacks(uint64_t Knights, uint64_t Friendly){
	string return_string;
	uint64_t Knight_Moves;
	uint64_t l1 = (Knights >> 1) & (0x7f7f7f7f7f7f7f7f);
  uint64_t l2 = (Knights >> 2) & (0x3f3f3f3f3f3f3f3f);
  uint64_t r1 = (Knights << 1) & (0xfefefefefefefefe);
  uint64_t r2 = (Knights << 2) & (0xfcfcfcfcfcfcfcfc);
  uint64_t h1 = l1 | r1;
  uint64_t h2 = l2 | r2;
  uint64_t To_Analysis = ((h1<<16) | (h1>>16) | (h2<<8) | (h2>>8)) & ~Friendly;
	return To_Analysis;}

//King Attacks (created pre-generated attack table)
const uint64_t King_Mask[64] = {0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 
0x7050, 0xe0a0, 0xc040, 0x30203, 0x70507, 0xe0a0e, 0x1c141c, 
0x382838, 0x705070, 0xe0a0e0, 0xc040c0, 0x3020300, 0x7050700, 
0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000, 
0xc040c000, 0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 
0x3828380000, 0x7050700000, 0xe0a0e00000, 0xc040c00000, 
0x30203000000, 0x70507000000, 0xe0a0e000000, 0x1c141c000000, 
0x382838000000, 0x705070000000, 0xe0a0e0000000, 0xc040c0000000, 
0x3020300000000, 0x7050700000000, 0xe0a0e00000000, 0x1c141c00000000, 
0x38283800000000, 0x70507000000000, 0xe0a0e000000000, 
0xc040c000000000, 0x302030000000000, 0x705070000000000, 
0xe0a0e0000000000, 0x1c141c0000000000, 0x3828380000000000, 
0x7050700000000000, 0xe0a0e00000000000, 0xc040c00000000000, 
0x203000000000000, 0x507000000000000, 0xa0e000000000000, 
0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 
0xa0e0000000000000, 0x40c0000000000000};

//Scan king move board
string Scan_Bit_King(int Origin, uint64_t Moves){
	//
	return "Lorem Ipsum";
}

string King_Attacks(uint64_t King_Board, uint64_t Friendly){
	uint64_t KING_MOVES = King_Mask[(int)log2(King_Board)];
	KING_MOVES &= ~Friendly;
	//get it to scan the bits (Not completed)
	return "Lorem Ipsum";
}

//Rook Attacks
string Rook_Moves(uint64_t Rook_Board,int Lorem_Ipsum/*filler for now*/){
	for(int square=0; square<64; square++){return "o";}
}

//Collecting all move generator parts into one or SMTH (pseudolegal for now)
string PossibleMoves(uint64_t WP,uint64_t WN,uint64_t WB,uint64_t WR,uint64_t WQ,uint64_t WK,uint64_t BP,uint64_t BN,uint64_t BB,uint64_t BR,uint64_t BQ,uint64_t BK) {
	uint64_t NOT_TAKEABLE=~(WP|WN|WB|WR|WQ|WK|BK);
	uint64_t BLACK_PIECES=BP|BN|BB|BR|BQ;
	uint64_t Empty=~(WP|WN|WB|WR|WQ|WK|BP|BN|BB|BR|BQ|BK);
	string output=WP_Attacks(WP,BLACK_PIECES,Empty);
	return output;}

enum Piece_Types {P, N, B, R, Q, K, p, n, b, r, q, k};
enum colors{WHITE, BLACK, BOTH};
uint64_t Bitboards [12];

//make moves and undo moves
//Castling rights
int Castle_White = 2;
int Castle_Black = 2;

//Bitboards after move
uint64_t White_Pawns;
uint64_t Black_Pawns;
uint64_t White_Rooks;
uint64_t Black_Rooks;
uint64_t White_Kings;
uint64_t Black_Kings;

//Previous bitboards before move
uint64_t pWhite_Pawns;
uint64_t pBlack_Pawns;
uint64_t pWhite_Rooks;
uint64_t pBlack_Rooks;
uint64_t pWhite_Kings;
uint64_t pBlack_Kings;

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


class Board_States{
	private:
		//Screams internally
		uint64_t W_K_Castle = 0x70;
		uint64_t W_Q_Castle = 0x1c;
		uint64_t B_K_Castle = 0x7000000000000000;
		uint64_t B_Q_Castle = 0x1c00000000000000;
		uint64_t W_R = 0x81;
		uint64_t B_R = 0x8100000000000000;
	public:
	//Attack map function. 'side' is the aggressor
	//Insert branchless programming
	uint64_t attack_map(int side){
		uint64_t mapped_attacks = 0ULL;
		uint64_t attacked_bitboard;
		//treated queens as a seperate entity
		for(int square=0; square<64; square++){
			attacked_bitboard=1ULL << square;			
			mapped_attacks |= (get_bit(Bitboards[side * 6], square) > 0) * Pawn_Attacks[side][square] + (get_bit(Bitboards[side * 6], square) <= 0) * 0; /*Pawns*/
			mapped_attacks |= (get_bit(Bitboards[1 + side * 6], square) > 0) * Knight_Attacks(attacked_bitboard, 0) + (get_bit(Bitboards[1 + side * 6], square) <= 0) * 0; /*Knights*/
			mapped_attacks |= (get_bit(Bitboards[2 + side * 6], square) > 0) * get_bishop_attacks(square, occupancies[BOTH]) + (get_bit(Bitboards[2 + side * 6], square) <= 0) * 0; /*Bishops*/
			mapped_attacks |= (get_bit(Bitboards[3 + side * 6], square) > 0)* get_rook_attacks(square, occupancies[BOTH]) + (get_bit(Bitboards[3 + side * 6], square) <= 0) * 0; /*Rooks*/
			mapped_attacks |= (get_bit(Bitboards[4 + side * 6], square) > 0) * get_queen_attacks(square, occupancies[BOTH]) + (get_bit(Bitboards[4 + side * 6], square) <= 0) * 0; /*Queens*/
			mapped_attacks |= (get_bit(Bitboards[5 + side * 6], square) > 0) * King_Mask[square] + (get_bit(Bitboards[5 + side * 6], square) <= 0) * 0;}
		return mapped_attacks;}

	/*The absolute pins to return map of pins on kings
	https://chess.stackexchange.com/questions/25137/chess-engine-generate-a-bitboard-of-pinned-pieces*/
	//Definitely not done
	uint64_t absolute_pins_vertical(int rooksq, int side){
		uint64_t vertical_attackers = occupancies[BOTH];
		if(vertical_attackers == 0){
    	return 0;}
		uint64_t bb_pinners = get_queen_attacks(0, 0) & occupancies[side];
		return 0;}
	//castling 
	bool can_castle(int side){
		//if king has moved, automatically return zero
		//if it sees that rook positions are different from suposed starting position, prune that possibility away
		//If there is any piece between the castling map, also remove possibility
		//If there attacks from other side on the squares, remove particular 
		uint64_t attacks = attack_map(side ^ 1);
		
	}
	
};

//encode moves
inline uint32_t encode_moves(int sourcesq, int targetsq, int piece, int promote, int capture, int double_push, int enpassant, int castle){
	uint32_t return_var = sourcesq | (targetsq << 6) | (piece << 12) | (promote << 16) | (capture << 20) | (double_push << 21) | (enpassant << 22) | (castle << 23);
	return return_var;}

//Board move history (Don't know what to refer to it as)
std::vector <int> foo;

class Move{
 void push( string move /*UCI string format but with 5th character for certain 
reasons including promotion*/){
  /*stuff goes here for making move*/
 }
 void pop(){
  /*Not completed*/
 }
};

inline void Initialize_Everything(){
	init_sliders_attacks(1);
	init_sliders_attacks(0);
	for (int square = 0; square < 64; square++){
    //Pawns
    Pawn_Attacks[WHITE][square] = mask_Pawn_Attacks(WHITE, square);
    Pawn_Attacks[BLACK][square] = mask_Pawn_Attacks(BLACK, square);}}

//Copy paste into shell (g++ -o3 main.cpp)
int main() {
	ChessBoard Print;
	Board_States board;
	//Initialize stuff
	Initialize_Everything();
	WP=Initialize_Bits(Board,'P');
	WN=Initialize_Bits(Board,'N');
	WB=Initialize_Bits(Board,'B');
	WR=Initialize_Bits(Board,'R');
	WQ=Initialize_Bits(Board,'Q');
	WK=Initialize_Bits(Board,'K');
	BP=Initialize_Bits(Board,'p');
	BN=Initialize_Bits(Board,'n');
	BB=Initialize_Bits(Board,'b');
	BR=Initialize_Bits(Board,'r');
	BQ=Initialize_Bits(Board,'q');
	BK=Initialize_Bits(Board,'k');
	update_occupancies();
	//testing pawn moves
	string test1=PossibleMoves(65280, 66, 24, 129, 16, 8, 262144, 0, 0, 0, 0, 0);
	std::cout<<"\n"<<test1;
	Print.Board();
	Print.Test_Board(rook_masks[d4]);
	Print.Test_Board(board.attack_map(BLACK));
	int a = 6;
	int *b = &a;
	std::cout<<*b;}