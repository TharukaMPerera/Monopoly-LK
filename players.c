#include "structures.h"

void player_initializer(player players[]){

players[1]= (player){
    .location = 0,
    .name = "Aggressive Investor", 
    .player_id = AGGRESSIVE_INVESTOR,
    .player_round_counter = 0,
    .cash = 30000,
    .dice_value = 0,
    .last_roll = 0,
    .is_order_assigned = inactive,
    .num_properties = 0,
    .loans_owned = (loan_details){inactive,0.0,0.0,0},
    .total_mortgage_value = 0,
    .policies = (insurance){inactive,0,0,0.0,0.0},
    .buildings = (building){no_buildings,0.0},
    .position = 0,
    .in_jail = inactive,
    .in_jail_counter = 0,
    .bankrupt = inactive
};


players[2]= (player){
    .location = 0,
    .name = "Conservative Banker", 
    .player_id = CONSERVATIVE_BANKER,
    .player_round_counter = 0,
    .cash = 30000,
    .dice_value = 0,
    .last_roll = 0,
    .is_order_assigned = inactive,
    .num_properties = 0,
    .loans_owned = (loan_details){inactive,0.0,0.0,0},
    .total_mortgage_value = 0,
    .policies = (insurance){inactive,0,0,0.0,0.0},
    .buildings = (building){no_buildings,0.0},
    .position = 0,
    .in_jail = inactive,
    .in_jail_counter = 0,
    .bankrupt = inactive
};

players[3]= (player){
    .location = 0,
    .name = "Risk Taker", 
    .player_id = RISK_TAKER,
    .player_round_counter = 0,
    .cash = 30000,
    .dice_value = 0,
    .last_roll = 0,
    .is_order_assigned = inactive,
    .num_properties = 0,
    .loans_owned = (loan_details){inactive,0.0,0.0,0},
    .total_mortgage_value = 0,
    .policies = (insurance){inactive,0,0,0.0,0.0},
    .buildings = (building){no_buildings,0.0},
    .position = 0,
    .in_jail = inactive,
    .in_jail_counter = 0,
    .bankrupt = inactive
};


players[4]= (player){
    .location = 0,
    .name = "Opportunistic Trader", 
    .player_id = OPPORTUNISTIC_TRADER,
    .player_round_counter = 0,
    .cash = 30000,
    .dice_value = 0,
    .last_roll = 0,
    .is_order_assigned = inactive,
    .num_properties = 0,
    .loans_owned = (loan_details){inactive,0.0,0.0,0},
    .total_mortgage_value = 0,
    .policies = (insurance){inactive,0,0,0.0,0.0},
    .buildings = (building){no_buildings,0.0},
    .position = 0,
    .in_jail = inactive,
    .in_jail_counter = 0,
    .bankrupt = inactive
};

}
