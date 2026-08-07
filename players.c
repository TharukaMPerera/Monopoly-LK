#include "structures.h"

void player_initializer(){
player players[5];
players[1]= (player){0,"Aggressive Investor", 30000,0,0,0,0,0,0,AGGRESSIVE_INVESTOR, 0};
players[2]= (player){0,"Conservative Banker", 30000,0,0,0,0,0,0,CONSERVATIVE_BANKER, 0};
players[3]= (player){0,"Risk Taker", 30000,0,0,0,0,0,0,RISK_TAKER, 0};
players[4]= (player){0,"Opportunistic Trader", 30000,0,0,0,0,0,0,OPPORTUNISTIC_TRADER, 0};

}


