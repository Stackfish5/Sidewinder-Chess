#include <iostream>
#include <cmath>


using std::string;

//board
char Board[64] = {' ',' ',' ',' ',' ','Q',' ',' ',
  								' ',' ',' ','Q',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ','Q',' ',
  								'Q',' ',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ',' ',' ',' ','Q',
  								' ','Q',' ',' ',' ',' ',' ',' ',
  								' ',' ',' ',' ','Q',' ',' ',' ',
  								' ',' ','Q',' ',' ',' ',' ','K'};

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

//Reverse the LERBEF format to LERF for proper use (Dislaying purposes, not rotated biboards)
uint64_t Reverse (uint64_t x) {
   const uint64_t k1 = 0x5555555555555555;
   const uint64_t k2 = 0x3333333333333333;
   const uint64_t k4 = 0x0f0f0f0f0f0f0f0f;
   x = ((x >> 1) & k1) | ((x & k1) << 1);
   x = ((x >> 2) & k2) | ((x & k2) << 2);
   x = ((x >> 4) & k4) | ((x & k4) << 4);
   return x;}

//declare bitboard list
uint64_t Bitboards [12];

class ChessBoard {
	public:
	//draw the board
	void Board(){
        string chessboard[8][8]={};
				for (int i=0;i<64;i++) {
        	if (((Reverse(Bitboards[6])>>i)&1)==1) {chessboard[i/8][i%8]="\u2659";}
        	if (((Reverse(Bitboards[7])>>i)&1)==1) {chessboard[i/8][i%8]="\u2658";}
        	if (((Reverse(Bitboards[8])>>i)&1)==1) {chessboard[i/8][i%8]="\u2657";}
       	  if (((Reverse(Bitboards[9])>>i)&1)==1) {chessboard[i/8][i%8]="\u2656";}
        	if (((Reverse(Bitboards[10])>>i)&1)==1) {chessboard[i/8][i%8]="\u2655";}
        	if (((Reverse(Bitboards[11])>>i)&1)==1) {chessboard[i/8][i%8]="\u2654";}
        	if (((Reverse(Bitboards[0])>>i)&1)==1) {chessboard[i/8][i%8]="\u265F";}
        	if (((Reverse(Bitboards[1])>>i)&1)==1) {chessboard[i/8][i%8]="\u265E";}
        	if (((Reverse(Bitboards[2])>>i)&1)==1) {chessboard[i/8][i%8]="\u265D";}
        	if (((Reverse(Bitboards[3])>>i)&1)==1) {chessboard[i/8][i%8]="\u265C";}
        	if (((Reverse(Bitboards[4])>>i)&1)==1) {chessboard[i/8][i%8]="\u265B";}
        	if (((Reverse(Bitboards[5])>>i)&1)==1) {chessboard[i/8][i%8]="\u265A";}
					}
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

	//initialize bitboard with inputs
	uint64_t Initialize(char chessboard[64],char type){
		uint64_t Bit=0;
		for(int i=0;i<64;i++){
			Bit^=(1ull<<(63-i))*(chessboard[i]==type);}
		return Reverse(Bit);}
};