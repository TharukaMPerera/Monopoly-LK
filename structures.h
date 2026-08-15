#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


 
typedef enum{
    inactive,
    active
}activity;


typedef enum
{   
    none,
    brown,
    light_blue,
    pink,
    orange,
    red,
    yellow,
    green,
    dark_blue
} color_group;

typedef struct 
{
    activity active;             
    double amount;
    double interest_rate;
    int loan_duration;          
} loan_details;


typedef enum
{
    start,
    land,
    event,
    tax,
    special,
    utility,
    railway,
    insurance_companies,
    bank

}property_type;

typedef enum {
    BANKER,
    AGGRESSIVE_INVESTOR,
    CONSERVATIVE_BANKER,
    RISK_TAKER,
    OPPORTUNISTIC_TRADER 
}Player_id;

typedef enum{
    no_buildings,
    house,
    hotel
}building_type;

typedef struct{
    building_type type;
    double condition;      
}building;


typedef struct{
    activity active;             
    int type;
    int rounds_remaining;
    double premium;
    double compensation;
}insurance;

typedef struct   // Property struct
{
    int index;
    color_group color;
    property_type type;
    char* name;
    int original_price;
    int purchase_price;
    int mortgage_value;
    int base_rental;
    int house_cost;
    Player_id current_owner;
    int hotel_cost;
    activity mortgage_status;
    activity insurance_status;
    int no_of_houses;
    int no_of_hotels;
    int age;
}property;

typedef struct//player struct
{
    int location;
    char* name;
    Player_id player_id;
    int cash;
    int dice_value;
    activity is_order_assigned;
    int *owned_properties[40];   
    int num_properties;
    loan_details loans_owned;
    int total_mortgage_value;
    insurance policies[40];      
    building buildings[40];      
    int position;                
    int in_jail;            
    int bankrupt;
}player;



//functions

void game_start_print();
void initial_game_loop();
void main_game_loop(player players[], int *round_counter);
void Monopoly_board(property board[]);
void print_stats(player players[], int *round_counter);
void player_initializer(player players[]);
int roll_dice();
int double_roll(int dice_01, int dice_02);
void player_order(player players[]);
void buy_function(int player_id, int property_id,int owned_properties[], player players[], property board[]);
void loan_function(int player_id, int player_request, player players[], property board[]);
void auction_function(int property_id , player players[] , property board[]);
void inflation(property board[], double inflation_rate);
#endif