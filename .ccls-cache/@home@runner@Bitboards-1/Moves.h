//Pseudolegal moves (as of 2/15/22)
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <string>
#include "Initialize.h"

//Heavy work in progress. Get the part where it appends the UCI string to the vector correct, make sure they all represent correct squares
char Board[64] = {'r','n','b','q','k','b','n','r',
  								'p','p','p','p','p','p','p','p',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ',' ',
  								'P','P','P','P','P','P','P','P',
  								'R','N','B','Q','K','B','N','R'};
using std::to_string;
//constants
const uint64_t FILE_A=0x0101010101010101;
const uint64_t FILE_H=0x8080808080808080;
const uint64_t RANK_8=0xFF00000000000000;
const uint64_t RANK_1=0x00000000000000FF;
const uint64_t RANK_4=0xff000000;
uint64_t PreviousBP;
uint64_t CurrentBP;
//end of constants
//pieces
std::uint64_t P=Initialize_Bits(Board,'P');
std::uint64_t N=Initialize_Bits(Board,'N');
std::uint64_t B=Initialize_Bits(Board,'B');
std::uint64_t R=Initialize_Bits(Board,'R');
std::uint64_t Q=Initialize_Bits(Board,'Q');
std::uint64_t K=Initialize_Bits(Board,'K');
std::uint64_t p=Initialize_Bits(Board,'p');
std::uint64_t n=Initialize_Bits(Board,'n');
std::uint64_t b=Initialize_Bits(Board,'b');
std::uint64_t r=Initialize_Bits(Board,'r');
std::uint64_t q=Initialize_Bits(Board,'q');
std::uint64_t k=Initialize_Bits(Board,'k');
//end of pieces
std::string int_to_str(int input){
	std::string to_str=to_string(floor(input));
	to_str.erase ( to_str.find_last_not_of('0') + 1, std::string::npos );
	to_str.erase ( to_str.find_last_not_of('.') + 1, std::string::npos );
  return to_str;}
uint64_t posibleMovesW(std::string history,uint64_t WP,uint64_t WN,uint64_t WB,uint64_t WR,uint64_t WQ,uint64_t WK,uint64_t BP,uint64_t BN,uint64_t BB,uint64_t BR,uint64_t BQ,uint64_t BK) {
		uint64_t NOT_WHITE_PIECES=~(WP|WN|WB|WR|WQ|WK|BK);
		uint64_t BLACK_PIECES=BP|BN|BB|BR|BQ;
		uint64_t EMPTY=~(WP|WN|WB|WR|WQ|WK|BP|BN|BB|BR|BQ|BK);
		return 0;
}
//(i/8) is the y-value, i%8 is x-value


//possible pawn moves (pseudolegal)	
std::vector <std::string> possiblePW(std::uint64_t WP, uint64_t Original_BP, uint64_t New_BP,uint64_t BLACK_PIECES,uint64_t EMPTY){
 uint64_t test1=Original_BP^(New_BP&Original_BP); //Original location of potentially moved pawn
 uint64_t test2=Original_BP^(New_BP&New_BP); //New position of potentially moved pawn
 bool en_passant;//en passant variable
 if((((test2>>16)==test1)&&(((test2&~FILE_H)>>1)==test1))||((test2&~FILE_A)<<1==test1)){
	 en_passant=true;
 } else {
	 en_passant=false;}
 std::string into_list="";
 std::vector <std::string> list;
 //capture to right
 std::uint64_t PAWN_MOVES=(WP>>9)&BLACK_PIECES&~RANK_8&~FILE_A;
 for(int i=0;i<64;i++){ 
into_list=int_to_str(i%8)+int_to_str(i/8)+int_to_str(i%8+1)+int_to_str(i%8+1);
	 if (((PAWN_MOVES>>i)&1)==1)
	 {list.push_back(into_list);}}	
 //capture to left
 PAWN_MOVES=(WP>>7)&BLACK_PIECES&~RANK_8&~FILE_H;	
   for(int i=0;i<64;i++){
	  into_list=int_to_str(i%8)+int_to_str(i/8)+int_to_str(i%8-1)+int_to_str(i%8+1);
	 if (((PAWN_MOVES>>i)&1)==1){list.push_back(into_list);}}
  //move forward one square	
 PAWN_MOVES=(WP>>8)&EMPTY&~RANK_8;
 for(int i=0;i<64;i++){
  into_list+int_to_str(i%8)+int_to_str(i/8)+int_to_str(i%8)+int_to_str(i/8+1);
	if (((PAWN_MOVES>>i)&1)==1){list.push_back(into_list);}}
 //move forward by two	
 PAWN_MOVES=(WP>>16)&EMPTY&(EMPTY>>8)&RANK_4; 
 for(int i=0;i<64;i++){
	into_list+int_to_str(i/8+1)+int_to_str(i%8-1)+int_to_str(i/8)+int_to_str(i%8);
	if (((PAWN_MOVES>>i)&1)==1){list.push_back(into_list);}}
 //pawn promotion by capture right	
 PAWN_MOVES=(WP>>9)&BLACK_PIECES&RANK_8&~FILE_A;
 for(int i=0;i<64;i++){
into_list=int_to_str(i%8)+int_to_str(i/8)+int_to_str(i%8+1)+int_to_str(i/8+1);
	if (((PAWN_MOVES>>i)&1)==1){list.push_back(into_list+"q");list.push_back(into_list+"r");list.push_back(into_list+"b");list.push_back(into_list+"n");}}
 //pawn promotion by capture left	
 PAWN_MOVES=(WP>>7)&BLACK_PIECES&RANK_8&~FILE_H;
 for(int i=0;i<64;i++){
into_list+int_to_str(i%8)+int_to_str(i/8)+int_to_str(i%8-1)+int_to_str(i/8+1);
	if (((PAWN_MOVES>>i)&1)==1){list.push_back(into_list+"q");list.push_back(into_list+"r");list.push_back(into_list+"b");list.push_back(into_list+"n");}}
 //pawn promotion by moving forward one square
 PAWN_MOVES=(WP>>8)&EMPTY&RANK_8;
 for(int i=0;i<64;i++){
into_list=int_to_str(i%8)+int_to_str(i%8)+int_to_str(i%8+1)+int_to_str(i/8+1);
	if (((PAWN_MOVES>>i)&1)==1){list.push_back(into_list+"q");list.push_back(into_list+"r");list.push_back(into_list+"b");list.push_back(into_list+"n");}}

 return list;}

std::vector <std::string> PossibleKnightMove(uint64_t WN, uint64_t BLACK_PIECES, uint64_t EMPTY){
	//Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna
}