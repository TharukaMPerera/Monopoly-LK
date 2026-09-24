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

typedef enum{
    FIRE,
    FLOOD,
    RIOT,
    BUILDING_COLLAPSE,
    ELECTRICAL_FALIURE
}Disasters;

typedef enum{
    TOURISM_BOOM,
    FUEL_CRISIS,
    HEAVY_MONSOON,
    ECONOMIC_RECESSION,
    STOCK_MARKET_BOOM,
    GOVERNMENT_HOUSING_PROGRAM,
    FOREIGN_INVESTMENT,
    POLITICAL_UNREST
}Economic_events;

typedef enum{
    TOURISM_HYPE,
    FUEL_SHORTAGE,
    HEAVY_FLOODS,
    POLITICAL_RALLY,
    STOCK_MARKET_RISE,
    ECONOMIC_DOWNTURN,
    HOUSING_SUBSIDY,
    INTEREST_RATE_CUT,
    INTEREST_RATE_INCREASE,
    TAX_AMNESTY,
    POWER_FAILURE,
    FOREIGN_FUNDING,
    PORT_EXPANSION,
    FESTIVAL_SEASON,
    LABOUR_STRIKE,
    INSURANCE_DISCOUNT,
    PROPERTY_REVALUATION,
    CURRENCY_DEPRECIATION,
    GOVERNMENT_GRANT,
    NATIONAL_DISASTER
}National_event_cards;

typedef enum{
    INCREASE_PROPERTY_TAX,
    REDUCE_LOAN_INTEREST,
    HOUSING_SUBSIDY_REGULATION,
    LUXURY_PROPERTY_TAX,
    RAILWAY_MODERNIZATION,
    ELECTRICITY_TARIFF_REVISION,
    INSURANCE_REGULATION,
    ANTI_SPECULATION_ACT
}Government_regulations;

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

typedef enum{
    NO_INSURANCE,
    BASIC_PROPERTY_INSURANCE,
    COMPREHENSIVE_INSURANCE,
    BUSINESS_INTERRUPTION_INSURANCE
}policy_type;

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
    int player_round_counter;
    int cash;
    int dice_value;
    int last_roll;
    activity is_order_assigned;
    int *owned_properties[40];   
    int num_properties;
    loan_details loans_owned;
    int total_mortgage_value;
    insurance policies[40];      
    building buildings[40];      
    int position;                
    activity in_jail; 
    int in_jail_counter;       
    activity bankrupt;
}player;

/*typedef struct{
    int TOURISM_BOOM_ROUND_COUNTER;
    int 
}*/


//functions

void game_start_print();
void initial_game_loop();
void main_game_loop(player players[], int *round_counter, property board[],int owned_properties[]);
void Monopoly_board(property board[]);
void print_stats(player players[], int *round_counter);
void player_initializer(player players[]);
int roll_dice();
int double_roll(int dice_01, int dice_02);
void player_order(player players[]);
void buy_function(int owned_properties[], player players[], property board[]);
void rent_function(player players[], property board[]);
void loan_function(int player_id, int player_request, player players[], property board[]);
void auction_function(int property_id , player players[] , property board[] , int owned_properties[] , int market_decline_group);
void inflation(property board[], double *current_inflation_rate);
void round_counter_function(player players[], int * round_counter);
void player_moving_function(player players[],property board[],int player_id);
void bankruptcy(player players[], activity *game_status, property board[], int owned_properties[]);
void go_to_jail(player players[], property board[]);
void escape_jail(player players[]);
void build_function(player players[], property board[]);
void depreciation_function(player players[], property board[]);
void renovation_function(player players[], property board[]);
void national_event_function(player players[], property board[], int drawer);
void update_national_event_counters(void);
void special_disaster(void);
void economic_events(Economic_events event_name, property board[]);
void government_regulation(Government_regulations regulation_name, property board[], double *income_tax_rate);
void dynamic_property_market(property board[], int *market_decline_group);
void insurance_function(player players[], property board[]);
void update_insurance_policies(player players[], property board[]);
void disasters(player players[], property board[]);
int total_property_assets(property board[], int owner_id);
void loan_interest_accumulation(player players[], property board[]);
void loan_payoff_function(int player_id, player players[]);


#endif