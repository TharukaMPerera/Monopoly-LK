#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

/* ============================================================
 * CONSTANTS
 * ============================================================ */
#define MAX_PLAYERS      4
#define BOARD_SIZE       40
#define MAX_ROUNDS       500
#define STARTING_CASH    30000
#define GO_SALARY        2000
#define JAIL_BAIL        300
#define MAX_JAIL_TURNS   3
#define LOAN_DURATION    20
#define INSURANCE_DURATION 20
#define MAX_PROPERTIES   28
#define MAX_RAILWAYS     4
#define MAX_UTILITIES    2
#define MAX_GROUPS       8
#define MAX_HOUSES       4
#define MAX_CARDS        20
#define AUCTION_INCREMENT 250

/* ============================================================
 * ENUMERATIONS
 * ============================================================ */
typedef enum {
    SQUARE_GO,
    SQUARE_PROPERTY,
    SQUARE_EVENT,
    SQUARE_TAX,
    SQUARE_RAILWAY,
    SQUARE_UTILITY,
    SQUARE_SPECIAL,
    SQUARE_INSURANCE,
    SQUARE_BANK
} SquareType;

typedef enum {
    GROUP_BROWN,
    GROUP_LIGHT_BLUE,
    GROUP_PINK,
    GROUP_ORANGE,
    GROUP_RED,
    GROUP_YELLOW,
    GROUP_GREEN,
    GROUP_DARK_BLUE
} PropertyGroup;

typedef enum {
    PLAYER_AGGRESSIVE,
    PLAYER_CONSERVATIVE,
    PLAYER_RISK_TAKER,
    PLAYER_OPPORTUNISTIC
} PlayerStrategy;

typedef enum {
    INSURANCE_NONE,
    INSURANCE_BASIC,
    INSURANCE_COMPREHENSIVE,
    INSURANCE_BUSINESS
} InsuranceType;

typedef enum {
    DISASTER_FIRE,
    DISASTER_FLOOD,
    DISASTER_RIOT,
    DISASTER_BUILDING_COLLAPSE,
    DISASTER_ELECTRICAL_FAILURE
} DisasterType;

typedef enum {
    ECON_TOURISM_BOOM,
    ECON_FUEL_CRISIS,
    ECON_HEAVY_MONSOON,
    ECON_RECESSION,
    ECON_STOCK_BOOM,
    ECON_HOUSING_PROGRAMME,
    ECON_FOREIGN_INVESTMENT,
    ECON_POLITICAL_UNREST
} EconomicEventType;

typedef enum {
    REG_SOUTHERN_TOURISM,
    REG_PORT_CITY,
    REG_IT_GROWTH,
    REG_NORTHERN_DEV,
    REG_TEA_EXPORT,
    REG_AIRPORT,
    REG_UNIVERSITY,
    REG_BEACH_POLLUTION,
    REG_FLOOD_DAMAGE,
    REG_TRANSPORT_STRIKE,
    REG_ELECTRICITY_TARIFF,
    REG_WATER_SHORTAGE
} RegionalCardType;

typedef enum {
    GOV_INCREASE_TAX,
    GOV_REDUCE_INTEREST,
    GOV_HOUSING_SUBSIDY,
    GOV_LUXURY_TAX,
    GOV_RAILWAY_MODERN,
    GOV_ELECTRICITY_REVISION,
    GOV_INSURANCE_REG,
    GOV_ANTI_SPECULATION
} GovRegulationType;

typedef enum {
    LOAN_ACTIVE,
    LOAN_REPAID,
    LOAN_DEFAULTED,
    LOAN_NONE
} LoanStatus;

/* ============================================================
 * STRUCTURES
 * ============================================================ */

/* A loan held by a player */
typedef struct {
    int amount;
    int interest_rate;      /* percentage */
    int accumulated_interest;
    int duration_remaining; /* rounds remaining */
    LoanStatus status;
    /* For tracking which properties are collateral */
    int collateral_indices[20];
    int collateral_count;
} Loan;

/* Insurance policy on a property */
typedef struct {
    InsuranceType type;
    int premium;
    int rounds_remaining;
    bool active;
} InsurancePolicy;

/* A property on the board */
typedef struct {
    int index;              /* board position 0-39 */
    char name[64];
    PropertyGroup group;
    int purchase_price;
    int base_purchase_price;
    int mortgage_value;
    int base_mortgage_value;
    int base_rent;
    int current_rent;
    int house_cost;
    int base_house_cost;
    int hotel_cost;
    int base_hotel_cost;
    int owner;              /* -1 = bank, 0-3 = player index */
    bool mortgaged;
    int num_houses;         /* 0-4 */
    bool has_hotel;
    int age;                /* rounds owned */
    int depreciation_pct;
    bool renovated;
    InsurancePolicy insurance;
    int damage_turns;       /* turns until repaired after disaster */
    bool loan_locked;       /* collateral for a loan */
    /* Market state */
    bool in_boom;
    bool in_decline;
    int boom_rounds_remaining;
    int decline_rounds_remaining;
    /* Building condition */
    int building_condition;  /* 0-100 */
    int maintenance_skipped; /* consecutive rounds without maintenance */
    bool structural_damage;
} Property;

/* Railway stations */
typedef struct {
    int index;
    char name[64];
    int purchase_price;
    int mortgage_value;
    int base_mortgage_value;
    int owner;              /* -1 = bank */
    bool mortgaged;
    bool loan_locked;
} Railway;

/* Utility companies */
typedef struct {
    int index;
    char name[64];
    int purchase_price;
    int mortgage_value;
    int base_mortgage_value;
    int owner;              /* -1 = bank */
    bool mortgaged;
    bool loan_locked;
} Utility;

/* National event card */
typedef struct {
    char description[64];
    int effect_type;        /* 0-19 index into the card effects */
    int duration;
    int magnitude;          /* percentage modifier */
} NationalCard;

/* A square on the board */
typedef struct {
    int index;
    char name[64];
    SquareType type;
    union {
        int property_index;  /* -1 if not a property */
        int railway_index;   /* -1 if not a railway */
        int utility_index;   /* -1 if not a utility */
    } ref;
} BoardSquare;

/* Active modifiers on a player (from cards, events, etc.) */
typedef struct {
    bool double_hotel_rent;
    int double_hotel_rent_rounds;
    bool double_railway_rent;
    int double_railway_rent_rounds;
    bool half_utility_income;
    int half_utility_income_rounds;
    bool construction_suspended;
    int construction_suspended_rounds;
    int property_closed_index;  /* -1 if none */
    int property_closed_rounds;
    bool festival_season;
    int festival_rounds;
    bool insurance_discount;
    int insurance_discount_rounds;
    int extra_rent_pct;     /* additional rent percentage */
    int extra_rent_rounds;
} PlayerModifiers;

/* A player in the game */
typedef struct {
    int id;
    char strategy_name[32];
    PlayerStrategy strategy;
    int cash;
    int position;           /* 0-39 board index */
    bool in_jail;
    int jail_turns;
    bool bankrupt;
    Loan active_loan;
    int net_worth;
    /* Owned assets tracked via property/railway/utility arrays */
    int properties_owned[28];  /* indices of owned properties, -1 terminated */
    int railways_owned[4];     /* indices of owned railways, -1 terminated */
    int utilities_owned[2];    /* indices of owned utilities, -1 terminated */
    int prop_count;
    int rail_count;
    int util_count;
    PlayerModifiers modifiers;
    /* Strategy state tracking */
    bool has_monopoly[8];
} Player;

/* Game state */
typedef struct {
    Player players[MAX_PLAYERS];
    Property properties[28];
    Railway railways[4];
    Utility utilities[2];
    BoardSquare board[BOARD_SIZE];
    NationalCard national_cards[MAX_CARDS];
    int card_deck_index;    /* current top of deck */
    int current_round;
    int current_player;     /* whose turn it is */
    int turn_order[MAX_PLAYERS];
    bool game_over;
    int winner;
    /* Economic state */
    double inflation_rate;  /* as decimal, e.g. 0.05 for 5% */
    int inflation_rounds_remaining;
    int base_interest_rate;  /* percentage */
    bool tourism_boom;
    bool fuel_crisis;
    bool heavy_monsoon;
    bool economic_recession;
    bool stock_market_boom;
    bool housing_programme;
    bool foreign_investment;
    bool political_unrest;
    /* Active event tracking */
    EconomicEventType current_economic_event;
    int economic_event_rounds_remaining;
    GovRegulationType current_gov_regulation;
    int gov_regulation_rounds_remaining;
    RegionalCardType current_regional_card;
    int regional_card_rounds_remaining;
    /* Market boom/decline tracking per group */
    int group_last_boom_round[8];
    int group_last_decline_round[8];
    int market_review_countdown;
    int disaster_countdown;
    int inflation_countdown;
    int economic_event_countdown;
    int gov_regulation_countdown;
    int regional_card_countdown;
    /* Property depreciation tracking */
    int prop_age_countdown;
    /* Gov regulation state */
    int income_tax_amount;
    int base_income_tax;
    double interest_rate_modifier;
    double construction_cost_modifier;
    double railway_rent_modifier;
    double utility_rent_modifier;
    double insurance_premium_modifier;
    bool anti_speculation_active;
    bool luxury_tax_active;
} GameState;

/* ============================================================
 * FUNCTION DECLARATIONS
 * ============================================================ */

/* main.c */
int main(void);

/* game.c */
void init_game(GameState *gs);
void run_simulation(GameState *gs);
void determine_turn_order(GameState *gs);
int roll_dice(void);
void player_turn(GameState *gs, int player_idx);
void end_round(GameState *gs);
int calculate_net_worth(GameState *gs, int player_idx);
void declare_winner(GameState *gs);
void print_round_summary(GameState *gs);
void print_market_conditions(GameState *gs);

/* board.c */
void init_board(GameState *gs);
void init_properties(GameState *gs);
void init_railways(GameState *gs);
void init_utilities(GameState *gs);
void init_national_cards(GameState *gs);
void move_player(GameState *gs, int player_idx, int steps);
void resolve_landing(GameState *gs, int player_idx);
void handle_property_landing(GameState *gs, int player_idx, int prop_idx);
void handle_railway_landing(GameState *gs, int player_idx, int rail_idx);
void handle_utility_landing(GameState *gs, int player_idx, int util_idx);
void handle_tax(GameState *gs, int player_idx);
void handle_event_square(GameState *gs, int player_idx);
void handle_insurance_square(GameState *gs, int player_idx, int square_idx);
void handle_bank_square(GameState *gs, int player_idx);
void handle_go_to_jail(GameState *gs, int player_idx);
int get_property_group_size(PropertyGroup group);
bool player_has_monopoly(GameState *gs, int player_idx, PropertyGroup group);

/* players.c */
bool should_buy_property(GameState *gs, int player_idx, int prop_idx);
int get_auction_bid_limit(GameState *gs, int player_idx, int prop_idx);
bool should_develop(GameState *gs, int player_idx);
void execute_development(GameState *gs, int player_idx);
bool should_buy_insurance(GameState *gs, int player_idx, int prop_idx, InsuranceType *out_type);
bool should_obtain_loan(GameState *gs, int player_idx, int *out_amount);
bool should_repay_loan(GameState *gs, int player_idx, int *out_amount);
bool should_renovate(GameState *gs, int player_idx, int prop_idx);
bool should_sell_property(GameState *gs, int player_idx);
void execute_player_decisions(GameState *gs, int player_idx);
int player_bid_amount(GameState *gs, int player_idx, int current_bid, int prop_idx);

/* finance.c */
void grant_loan(GameState *gs, int player_idx, int amount);
bool repay_loan(GameState *gs, int player_idx, int amount);
void repay_loan_full(GameState *gs, int player_idx);
void extend_loan(GameState *gs, int player_idx);
void increase_loan(GameState *gs, int player_idx);
void process_loan_default(GameState *gs, int player_idx);
void accumulate_interest(GameState *gs);
void purchase_insurance(GameState *gs, int player_idx, int prop_idx, InsuranceType type);
void process_insurance_expiry(GameState *gs);
void apply_inflation(GameState *gs, double rate);
void apply_property_depreciation(GameState *gs);
void apply_building_depreciation(GameState *gs);
void perform_maintenance(GameState *gs, int player_idx);
void apply_building_condition(GameState *gs);
void process_bankruptcy(GameState *gs, int player_idx);
void check_bankruptcy(GameState *gs);
void renovate_property(GameState *gs, int player_idx, int prop_idx);
int calculate_rent(GameState *gs, int prop_idx, int dice_value);
int calculate_railway_rent(GameState *gs, int rail_idx);
int calculate_utility_rent(GameState *gs, int util_idx, int dice_value);
int get_property_market_value(GameState *gs, int prop_idx);
int get_max_loan_amount(GameState *gs, int player_idx);
bool can_afford(GameState *gs, int player_idx, int amount);
void deduct_cash(GameState *gs, int player_idx, int amount);
void add_cash(GameState *gs, int player_idx, int amount);
void auction_property(GameState *gs, int prop_idx);

/* events.c */
void check_economic_events(GameState *gs);
void check_gov_regulations(GameState *gs);
void check_regional_cards(GameState *gs);
void check_disasters(GameState *gs);
void check_market_review(GameState *gs);
void apply_economic_event(GameState *gs, EconomicEventType evt);
void apply_gov_regulation(GameState *gs, GovRegulationType reg);
void apply_regional_card(GameState *gs, RegionalCardType card);
void trigger_disaster(GameState *gs);
void review_property_market(GameState *gs);
void draw_national_card(GameState *gs, int player_idx);
void clear_economic_effects(GameState *gs);
void update_all_prices(GameState *gs);

#endif /* TYPES_H */
