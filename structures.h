#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>

int owned_properties[40]; 
typedef enum{
    inactive,
    active
}activity;


typedef enum
{
    NONE,
    BANK,
    PLAYER_1,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4
} Player;

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
}player_id;

typedef enum{
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
    int purchase_price;
    int mortgage_value;
    int base_rental;
    int house_cost;
    int current_owner;
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
    int cash;
    int *owned_properties[40];   
    int num_properties;
    loan_details loans_owned;
    int total_mortgage_value;
    insurance policies[40];      
    building buildings[40];      
    int position;                
    activity in_jail;            
    int bankrupt;
}player;


void roll();
void player_initializer();
int roll_dice();
int double_roll(int dice_01, int dice_02);
void first_go(int *order);
void buy(int player_id, int property_id,int *owned_properties[40], player players[4], property board[40]);
void loan(int player_id, int player_request, player players[4]);
void auction(int property_id , player players[5] , property board[40], int player_id);

#endif