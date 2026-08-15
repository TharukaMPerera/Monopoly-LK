#include "structures.h"



void main_game_loop(player players[], int *round_counter){
  while(1){
    print_stats(players,round_counter);

    
    if( 1 /*there should be the winning condition*/){

    break;
    }
}
}

void initial_game_loop()
{
    srand(time(NULL));
    int round_counter = 0;
    int owned_properites [40];
    player players[5];
    property board[40];
    Monopoly_board(board);
    game_start_print();
    player_initializer(players);
    player_order(players);
    main_game_loop(players , &round_counter);


}

