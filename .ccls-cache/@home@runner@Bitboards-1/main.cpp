#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <cmath>
#include "Piece_Square_Tables.h"
using std::to_string;
using std::string;

//board
char Board[64] = {'r','n','b','q','k','b','n','r',
  								'p','p','p','p','p','p','p','p',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ','p',' ',' ',
  								'P','P','P','P','P','P','P','P',
  								'R','N','B','Q','K','B','N','R'};

//In which a1=7, h1=0, a8=63, and h8=56
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

//draw the board
void draw_array(uint64_t WP,uint64_t WN,uint64_t WB,uint64_t WR,uint64_t WQ,uint64_t WK,uint64_t BP,uint64_t BN,uint64_t BB,uint64_t BR,uint64_t BQ,uint64_t BK){
        string chessboard[8][8]={};
        for (int i=0;i<64;i++) {
				uint64_t P=Reverse(WP);
				uint64_t N=Reverse(WN);
				uint64_t B=Reverse(WB);
				uint64_t R=Reverse(WR);
				uint64_t Q=Reverse(WQ);
				uint64_t K=Reverse(WK);
				uint64_t p=Reverse(BP);
				uint64_t n=Reverse(BN);
				uint64_t b=Reverse(BB);
				uint64_t r=Reverse(BR);
				uint64_t q=Reverse(BQ);
				uint64_t k=Reverse(BK);
        if (((p>>i)&1)==1) {chessboard[i/8][i%8]="\u2659";}
        if (((n>>i)&1)==1) {chessboard[i/8][i%8]="\u2658";}
        if (((b>>i)&1)==1) {chessboard[i/8][i%8]="\u2657";}
        if (((r>>i)&1)==1) {chessboard[i/8][i%8]="\u2656";}
        if (((q>>i)&1)==1) {chessboard[i/8][i%8]="\u2655";}
        if (((k>>i)&1)==1) {chessboard[i/8][i%8]="\u2654";}
        if (((P>>i)&1)==1) {chessboard[i/8][i%8]="\u265F";}
        if (((N>>i)&1)==1) {chessboard[i/8][i%8]="\u265E";}
        if (((B>>i)&1)==1) {chessboard[i/8][i%8]="\u265D";}
        if (((R>>i)&1)==1) {chessboard[i/8][i%8]="\u265C";}
        if (((Q>>i)&1)==1) {chessboard[i/8][i%8]="\u265B";}
        if (((K>>i)&1)==1) {chessboard[i/8][i%8]="\u265A";}}
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

//initialize bitboard with inputs
uint64_t Initialize_Bits(char chessboard[64],char type){
	uint64_t Bit=0;
	for(int i=0;i<64;i++){
		Bit^=(1ull<<convert(i))*(chessboard[i]==type);}
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
//Reverse the x1 value
string Scan_Bits(uint64_t input, int offset, string FLAG){
 string output="";
 string into_list;
 int x;
 for(int i=0;i<64;i++){ 
	x=convert(i);
	if (((input>>x)&1)==1){	into_list = int_to_str((x - offset) % 8 + 1/*x1 have to mirror this value*/) + int_to_str((x - offset) / 8 + 1/*y1*/) + int_to_str(x % 8 + 1/*x2*/) + int_to_str(x / 8 + 1/*y2*/) + FLAG;
		output.append(into_list+"\n");}}
return output;}	

//Move History
string History;

//Pawn Attacks (White)
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
	//This is buggy
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
	//Set a few variables necessary
	
	return return_string;}

//Magic Bitboard Move Generation (retry time, let's not botch this)
//Rook Magic Numbers
const uint64_t RMagic[64] = {
  0x2080020500400f0ULL,
  0x28444000400010ULL,
  0x20000a1004100014ULL,
  0x20010c090202006ULL,
  0x8408008200810004ULL,
  0x1746000808002ULL,
  0x2200098000808201ULL,
  0x12c0002080200041ULL,
  0x104000208e480804ULL,
  0x8084014008281008ULL,
  0x4200810910500410ULL,
  0x100014481c20400cULL,
  0x4014a4040020808ULL,
  0x401002001010a4ULL,
  0x202000500010001ULL,
  0x8112808005810081ULL,
  0x40902108802020ULL,
  0x42002101008101ULL,
  0x459442200810c202ULL,
  0x81001103309808ULL,
  0x8110000080102ULL,
  0x8812806008080404ULL,
  0x104020000800101ULL,
  0x40a1048000028201ULL,
  0x4100ba0000004081ULL,
  0x44803a4003400109ULL,
  0xa010a00000030443ULL,
  0x91021a000100409ULL,
  0x4201e8040880a012ULL,
  0x22a000440201802ULL,
  0x30890a72000204ULL,
  0x10411402a0c482ULL,
  0x40004841102088ULL,
  0x40230000100040ULL,
  0x40100010000a0488ULL,
  0x1410100200050844ULL,
  0x100090808508411ULL,
  0x1410040024001142ULL,
  0x8840018001214002ULL,
  0x410201000098001ULL,
  0x8400802120088848ULL,
  0x2060080000021004ULL,
  0x82101002000d0022ULL,
  0x1001101001008241ULL,
  0x9040411808040102ULL,
  0x600800480009042ULL,
  0x1a020000040205ULL,
  0x4200404040505199ULL,
  0x2020081040080080ULL,
  0x40a3002000544108ULL,
  0x4501100800148402ULL,
  0x81440280100224ULL,
  0x88008000000804ULL,
  0x8084060000002812ULL,
  0x1840201000108312ULL,
  0x5080202000000141ULL,
  0x1042a180880281ULL,
  0x900802900c01040ULL,
  0x8205104104120ULL,
  0x9004220000440aULL,
  0x8029510200708ULL,
  0x8008440100404241ULL,
  0x2420001111000bdULL,
  0x4000882304000041ULL};

//Bishop Magic Numbers
const uint64_t BMagic[64] = {
  0x100420000431024ULL,
  0x280800101073404ULL,
  0x42000a00840802ULL,
  0xca800c0410c2ULL,
  0x81004290941c20ULL,
  0x400200450020250ULL,
  0x444a019204022084ULL,
  0x88610802202109aULL,
  0x11210a0800086008ULL,
  0x400a08c08802801ULL,
  0x1301a0500111c808ULL,
  0x1280100480180404ULL,
  0x720009020028445ULL,
  0x91880a9000010a01ULL,
  0x31200940150802b2ULL,
  0x5119080c20000602ULL,
  0x242400a002448023ULL,
  0x4819006001200008ULL,
  0x222c10400020090ULL,
  0x302008420409004ULL,
  0x504200070009045ULL,
  0x210071240c02046ULL,
  0x1182219000022611ULL,
  0x400c50000005801ULL,
  0x4004010000113100ULL,
  0x2008121604819400ULL,
  0xc4a4010000290101ULL,
  0x404a000888004802ULL,
  0x8820c004105010ULL,
  0x28280100908300ULL,
  0x4c013189c0320a80ULL,
  0x42008080042080ULL,
  0x90803000c080840ULL,
  0x2180001028220ULL,
  0x1084002a040036ULL,
  0x212009200401ULL,
  0x128110040c84a84ULL,
  0x81488020022802ULL,
  0x8c0014100181ULL,
  0x2222013020082ULL,
  0xa00100002382c03ULL,
  0x1000280001005c02ULL,
  0x84801010000114cULL,
  0x480410048000084ULL,
  0x21204420080020aULL,
  0x2020010000424a10ULL,
  0x240041021d500141ULL,
  0x420844000280214ULL,
  0x29084a280042108ULL,
  0x84102a8080a20a49ULL,
  0x104204908010212ULL,
  0x40a20280081860c1ULL,
  0x3044000200121004ULL,
  0x1001008807081122ULL,
  0x50066c000210811ULL,
  0xe3001240f8a106ULL,
  0x940c0204030020d4ULL,
  0x619204000210826aULL,
  0x2010438002b00a2ULL,
  0x884042004005802ULL,
  0xa90240000006404ULL,
  0x500d082244010008ULL,
  0x28190d00040014e0ULL,
  0x825201600c082444ULL};

//Occupancy Rook
uint64_t MaskRook[64] = {0x101010101017e, 0x202020202027c, 
0x404040404047a, 0x8080808080876, 0x1010101010106e, 
0x2020202020205e, 0x4040404040403e, 0x8080808080807e, 
0x1010101017e00, 0x2020202027c00, 0x4040404047a00, 
0x8080808087600, 0x10101010106e00, 0x20202020205e00, 
0x40404040403e00, 0x80808080807e00, 0x10101017e0100, 
0x20202027c0200, 0x40404047a0400, 0x8080808760800, 
0x101010106e1000, 0x202020205e2000, 0x404040403e4000, 
0x808080807e8000, 0x101017e010100, 0x202027c020200, 
0x404047a040400, 0x8080876080800, 0x1010106e101000, 
0x2020205e202000, 0x4040403e404000, 0x8080807e808000, 
0x1017e01010100, 0x2027c02020200, 0x4047a04040400, 
0x8087608080800, 0x10106e10101000, 0x20205e20202000, 
0x40403e40404000, 0x80807e80808000, 0x17e0101010100, 
0x27c0202020200, 0x47a0404040400, 0x8760808080800, 
0x106e1010101000, 0x205e2020202000, 0x403e4040404000, 
0x807e8080808000, 0x7e010101010100, 0x7c020202020200, 
0x7a040404040400, 0x76080808080800, 0x6e101010101000, 
0x5e202020202000, 0x3e404040404000, 0x7e808080808000, 
0x7e01010101010100, 0x7c02020202020200, 
0x7a04040404040400, 0x7608080808080800, 
0x6e10101010101000, 0x5e20202020202000, 
0x3e40404040404000, 0x7e80808080808000};

//Occupancy Bishop
uint64_t MaskBishop[64] = {0x40201008040200,  
0x402010080400,  0x4020100a00,  0x40221400,  0x2442800,  
0x204085000,  0x20408102000,  0x2040810204000,  
0x20100804020000,  0x40201008040000,  0x4020100a0000,  
0x4022140000,  0x244280000,  0x20408500000,  
0x2040810200000,  0x4081020400000,  0x10080402000200,  
0x20100804000400,  0x4020100a000a00,  0x402214001400,  
0x24428002800,  0x2040850005000,  0x4081020002000,  
0x8102040004000,  0x8040200020400,  0x10080400040800,  
0x20100a000a1000,  0x40221400142200,  0x2442800284400,  
0x4085000500800,  0x8102000201000,  0x10204000402000,  
0x4020002040800,  0x8040004081000,  0x100a000a102000,  
0x22140014224000,  0x44280028440200,  0x8500050080400,  
0x10200020100800,  0x20400040201000,  0x2000204081000,  
0x4000408102000,  0xa000a10204000,  0x14001422400000,  
0x28002844020000,  0x50005008040200,  0x20002010080400,  
0x40004020100800,  0x20408102000,  0x40810204000,  
0xa1020400000,  0x142240000000,  0x284402000000,  
0x500804020000,  0x201008040200,  0x402010080400,  
0x2040810204000,  0x4081020400000,  0xa102040000000,  
0x14224000000000,  0x28440200000000,  0x50080402000000,  
0x20100804020000,  0x40201008040200};

/*Blocker Boards courtesey of 
https://stackoverflow.com/questions/30680559/how-to-find-magic-bitboards#comment49437050_30680559*/
uint64_t gen_blockerboard (int index, uint64_t blockermask) {
	/* Start with a blockerboard identical to the mask. */
	uint64_t blockerboard = blockermask;
	/* Loop through the blockermask to find the indices of all set bits. */
  int8_t bitindex = 0;
  for (int8_t i=0; i<64; i++) {
  	/* Check if the i'th bit is set in the mask (and thus a potential blocker). */
    if ( blockermask & (1ULL<<i) ) {
      /* Clear the i'th bit in the blockerboard if it's clear in the index at bitindex. */
      if ( !(index & (1<<bitindex)) ) {
        blockerboard &= ~(1ULL<<i); //Clear the bit.
  } /* Increment the bit index in the 0-4096 index, so each bit in index will correspond * to each set bit in blockermask. */
  bitindex++;
        }}return blockerboard;}

//Bishop Attacks
string Bishop_Attacks(uint64_t BISHOP, uint64_t OCCUPIED, uint64_t Friendly){
	string return_string;
	// Find position of Bishops
	for(int x=0; x<64-trailing_zeros(BISHOP);x++){
		x=convert(x);
		uint64_t BISHOP_MOVES=0;
	}
	return return_string;
}

/*Knight Attacks
return the thing as a string like pawns (or else)*/
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

//King Attacks (created pre-generated lookup table)
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

string King_Attacks(uint64_t King_Board, uint64_t Friendly){
	uint64_t KING_MOVES = King_Mask[(int)log2(King_Board)];
	//get it to scan the bits
}

//Rook Attacks
string Rook_Attacks(uint64_t Rook_Board,int Lorem_Ipsum/*filler for now*/){return 0;}

//Collecting all move generator parts into one or SMTH (pseudolegal for now)
string PossibleMoves(uint64_t WP,uint64_t WN,uint64_t WB,uint64_t WR,uint64_t WQ,uint64_t WK,uint64_t BP,uint64_t BN,uint64_t BB,uint64_t BR,uint64_t BQ,uint64_t BK) {
	uint64_t NOT_TAKEABLE=~(WP|WN|WB|WR|WQ|WK|BK);
	uint64_t BLACK_PIECES=BP|BN|BB|BR|BQ;
	uint64_t Empty=~(WP|WN|WB|WR|WQ|WK|BP|BN|BB|BR|BQ|BK);
	string output=WP_Attacks(WP,BLACK_PIECES,Empty);
	return output;}

//Initialize bitboards
uint64_t WP=Initialize_Bits(Board,'P');
	uint64_t WN=Initialize_Bits(Board,'N');
	uint64_t WB=Initialize_Bits(Board,'B');
	uint64_t WR=Initialize_Bits(Board,'R');
	uint64_t WQ=Initialize_Bits(Board,'Q');
	uint64_t WK=Initialize_Bits(Board,'K');
	uint64_t BP=Initialize_Bits(Board,'p');
	uint64_t BN=Initialize_Bits(Board,'n');
	uint64_t BB=Initialize_Bits(Board,'b');
	uint64_t BR=Initialize_Bits(Board,'r');
	uint64_t BQ=Initialize_Bits(Board,'q');
	uint64_t BK=Initialize_Bits(Board,'k');

//make moves and undo moves
class Move{
 uint64_t push( string move /*UCI string format but with 5th character for certain 
reasons including promotion*/){
	 return 1; /*stuff goes here for making move*/
 }
 uint64_t pop(){
	 return 1;  /*Not completed*/
 }
};


int main() {
	//Initialize board and stuff
	std::cout<<h8;
  //substring test thing?
	string test="Stockfish";
	std::cout<<"\n"<<test.substr(0,9);
	//testing pawn moves
	string test1=PossibleMoves(65280, 66, 24, 129, 16, 8, 262144, 0, 0, 0, 0, 0);
	draw_array(WP, WN, WB, WR, WQ, WK, BP, BN, BB, BR, BQ, BK);
	std::cout<<"\n"<<test1;}