#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/* Property data: group, name, purchase, house_cost, hotel_cost, mortgage */
typedef struct {
    PropertyGroup group;
    char *name;
    int price;
    int house_cost;
    int hotel_cost;
    int mortgage;
} PropertyData;

static PropertyData prop_data[28] = {
    { GROUP_BROWN,       "Pettah",          1500,  500,  2000,  750 },
    { GROUP_BROWN,       "Maradana",        1500,  500,  2000,  750 },
    { GROUP_LIGHT_BLUE,  "Bambalapitiya",   2500,  750,  3000, 1250 },
    { GROUP_LIGHT_BLUE,  "Wellawatte",      2500,  750,  3000, 1250 },
    { GROUP_LIGHT_BLUE,  "Mount Lavinia",   2500,  750,  3000, 1250 },
    { GROUP_PINK,        "Nugegoda",        3500, 1000,  4000, 1750 },
    { GROUP_PINK,        "Maharagama",      3500, 1000,  4000, 1750 },
    { GROUP_PINK,        "Kottawa",         3500, 1000,  4000, 1750 },
    { GROUP_ORANGE,      "Negombo",         4500, 1250,  5000, 2250 },
    { GROUP_ORANGE,      "Katunayake",      4500, 1250,  5000, 2250 },
    { GROUP_ORANGE,      "Ja-Ela",          4500, 1250,  5000, 2250 },
    { GROUP_RED,         "Kandy City",      5500, 1500,  6000, 2750 },
    { GROUP_RED,         "Peradeniya",      5500, 1500,  6000, 2750 },
    { GROUP_RED,         "Katugastota",     5500, 1500,  6000, 2750 },
    { GROUP_YELLOW,      "Galle Fort",      6500, 2000,  8000, 3250 },
    { GROUP_YELLOW,      "Unawatuna",       6500, 2000,  8000, 3250 },
    { GROUP_YELLOW,      "Hikkaduwa",       6500, 2000,  8000, 3250 },
    { GROUP_GREEN,       "Jaffna Town",     8000, 2500, 10000, 4000 },
    { GROUP_GREEN,       "Nallur",          8000, 2500, 10000, 4000 },
    { GROUP_GREEN,       "Trincomalee",     8000, 2500, 10000, 4000 },
    { GROUP_DARK_BLUE,   "Nuwara Eliya",   10000, 3000, 12000, 5000 },
    { GROUP_DARK_BLUE,   "Galle Face",     10000, 3000, 12000, 5000 },
    /* Pad to 28 - actually there are only 22 residential properties in the board layout,
       but the assignment mentions up to 28 properties including the board. Let's count:
       Brown: 2, Light Blue: 3, Pink: 3, Orange: 3, Red: 3, Yellow: 3, Green: 3, Dark Blue: 2 = 22 */
};

/* Board layout: index, name, type, ref_index */
typedef struct {
    int index;
    char *name;
    SquareType type;
    int ref_index; /* property_idx / railway_idx / utility_idx, -1 if none */
} BoardLayout;

static BoardLayout layout[40] = {
    {  0, "GO",                                    SQUARE_GO,        -1 },
    {  1, "Pettah",                                SQUARE_PROPERTY,   0 },
    {  2, "Community Development Fund",            SQUARE_EVENT,     -1 },
    {  3, "Maradana",                              SQUARE_PROPERTY,   1 },
    {  4, "Income Tax",                            SQUARE_TAX,       -1 },
    {  5, "Colombo Fort Railway Station",          SQUARE_RAILWAY,    0 },
    {  6, "Bambalapitiya",                         SQUARE_PROPERTY,   2 },
    {  7, "National Event Card",                   SQUARE_EVENT,     -1 },
    {  8, "Wellawatte",                            SQUARE_PROPERTY,   3 },
    {  9, "Mount Lavinia",                         SQUARE_PROPERTY,   4 },
    { 10, "Jail / Just Visiting",                  SQUARE_SPECIAL,   -1 },
    { 11, "Nugegoda",                              SQUARE_PROPERTY,   5 },
    { 12, "Ceylon Electricity Board",              SQUARE_UTILITY,    0 },
    { 13, "Maharagama",                            SQUARE_PROPERTY,   6 },
    { 14, "Kottawa",                               SQUARE_PROPERTY,   7 },
    { 15, "Kandy Railway Station",                 SQUARE_RAILWAY,    1 },
    { 16, "Negombo",                               SQUARE_PROPERTY,   8 },
    { 17, "Sri Lanka Insurance",                   SQUARE_INSURANCE, -1 },
    { 18, "Katunayake",                            SQUARE_PROPERTY,   9 },
    { 19, "Ja-Ela",                                SQUARE_PROPERTY,  10 },
    { 20, "Free Parking",                          SQUARE_SPECIAL,   -1 },
    { 21, "Kandy City",                            SQUARE_PROPERTY,  11 },
    { 22, "National Event Card",                   SQUARE_EVENT,     -1 },
    { 23, "Peradeniya",                            SQUARE_PROPERTY,  12 },
    { 24, "Katugastota",                           SQUARE_PROPERTY,  13 },
    { 25, "Galle Railway Station",                 SQUARE_RAILWAY,    2 },
    { 26, "Galle Fort",                            SQUARE_PROPERTY,  14 },
    { 27, "Unawatuna",                             SQUARE_PROPERTY,  15 },
    { 28, "National Water Supply and Drainage Board", SQUARE_UTILITY, 1 },
    { 29, "Hikkaduwa",                             SQUARE_PROPERTY,  16 },
    { 30, "Go To Jail",                            SQUARE_SPECIAL,   -1 },
    { 31, "Jaffna Town",                           SQUARE_PROPERTY,  17 },
    { 32, "Nallur",                                SQUARE_PROPERTY,  18 },
    { 33, "Ceylinco Insurance",                    SQUARE_INSURANCE, -1 },
    { 34, "Trincomalee",                           SQUARE_PROPERTY,  19 },
    { 35, "Jaffna Railway Station",                SQUARE_RAILWAY,    3 },
    { 36, "National Event Card",                   SQUARE_EVENT,     -1 },
    { 37, "Nuwara Eliya",                          SQUARE_PROPERTY,  20 },
    { 38, "Bank of Ceylon",                        SQUARE_BANK,      -1 },
    { 39, "Galle Face",                            SQUARE_PROPERTY,  21 },
};

void init_board(GameState *gs) {
    int i;

    /* Initialize board squares */
    for (i = 0; i < BOARD_SIZE; i++) {
        gs->board[i].index = layout[i].index;
        strncpy(gs->board[i].name, layout[i].name, 63);
        gs->board[i].name[63] = '\0';
        gs->board[i].type = layout[i].type;
        if (layout[i].type == SQUARE_PROPERTY)
            gs->board[i].ref.property_index = layout[i].ref_index;
        else if (layout[i].type == SQUARE_RAILWAY)
            gs->board[i].ref.railway_index = layout[i].ref_index;
        else if (layout[i].type == SQUARE_UTILITY)
            gs->board[i].ref.utility_index = layout[i].ref_index;
        else {
            gs->board[i].ref.property_index = -1;
        }
    }
}

void init_properties(GameState *gs) {
    int i;
    for (i = 0; i < 22; i++) {
        Property *p = &gs->properties[i];
        p->index = layout[i].index;
        strncpy(p->name, prop_data[i].name, 63);
        p->name[63] = '\0';
        p->group = prop_data[i].group;
        p->purchase_price = prop_data[i].price;
        p->base_purchase_price = prop_data[i].price;
        p->mortgage_value = prop_data[i].mortgage;
        p->base_mortgage_value = prop_data[i].mortgage;
        p->base_rent = prop_data[i].price / 10; /* base rent = 10% of purchase price */
        p->current_rent = p->base_rent;
        p->house_cost = prop_data[i].house_cost;
        p->base_house_cost = prop_data[i].house_cost;
        p->hotel_cost = prop_data[i].hotel_cost;
        p->base_hotel_cost = prop_data[i].hotel_cost;
        p->owner = -1;
        p->mortgaged = false;
        p->num_houses = 0;
        p->has_hotel = false;
        p->age = 0;
        p->depreciation_pct = 0;
        p->renovated = false;
        p->insurance.type = INSURANCE_NONE;
        p->insurance.active = false;
        p->insurance.rounds_remaining = 0;
        p->insurance.premium = 0;
        p->damage_turns = 0;
        p->loan_locked = false;
        p->in_boom = false;
        p->in_decline = false;
        p->boom_rounds_remaining = 0;
        p->decline_rounds_remaining = 0;
        p->building_condition = 100;
        p->maintenance_skipped = 0;
        p->structural_damage = false;
    }
    /* Remaining property slots (22-27) are unused, mark as owner -2 */
    for (i = 22; i < 28; i++) {
        gs->properties[i].owner = -2; /* sentinel: not a real property */
    }
}

void init_railways(GameState *gs) {
    int i;
    char *names[4] = {
        "Colombo Fort Railway Station",
        "Kandy Railway Station",
        "Galle Railway Station",
        "Jaffna Railway Station"
    };
    int prices[4] = { 2000, 2000, 2000, 2000 };
    for (i = 0; i < 4; i++) {
        Railway *r = &gs->railways[i];
        strncpy(r->name, names[i], 63);
        r->name[63] = '\0';
        r->purchase_price = prices[i];
        r->mortgage_value = 1000;
        r->base_mortgage_value = 1000;
        r->owner = -1;
        r->mortgaged = false;
        r->loan_locked = false;
    }
}

void init_utilities(GameState *gs) {
    Utility *u0 = &gs->utilities[0];
    strncpy(u0->name, "Ceylon Electricity Board", 63);
    u0->name[63] = '\0';
    u0->purchase_price = 2000;
    u0->mortgage_value = 1000;
    u0->base_mortgage_value = 1000;
    u0->owner = -1;
    u0->mortgaged = false;
    u0->loan_locked = false;

    Utility *u1 = &gs->utilities[1];
    strncpy(u1->name, "National Water Supply and Drainage Board", 63);
    u1->name[63] = '\0';
    u1->purchase_price = 2000;
    u1->mortgage_value = 1000;
    u1->base_mortgage_value = 1000;
    u1->owner = -1;
    u1->mortgaged = false;
    u1->loan_locked = false;
}

void init_national_cards(GameState *gs) {
    char *descs[20] = {
        "Tourism Hype - Hotels earn double rent for 5 rounds",
        "Fuel Shortage - Railway rent doubles for 5 rounds",
        "Heavy Floods - Random coastal property damaged",
        "Political Rally - One random property closed for 2 rounds",
        "Stock Market Rise - All property values increase by 10%",
        "Economic Downturn - Property values decrease by 15%",
        "Housing Subsidy - House construction cost reduced by 30%",
        "Interest Rate Cut - Loan interest reduced by 2%",
        "Interest Rate Increase - Loan interest increased by 2%",
        "Tax Amnesty - Each player receives LKR 2,000",
        "Power Failure - Utility income halved for 3 rounds",
        "Foreign Funding - Commercial property values increase by 15%",
        "Port Expansion - Railway station values increase by 20%",
        "Festival Season - Hotels receive 50% additional rent",
        "Labour Strike - Construction suspended for 2 rounds",
        "Insurance Discount - Premiums reduced by 20%",
        "Property Revaluation - Random property group appreciates by 15%",
        "Currency Depreciation - Construction costs increase by 10%",
        "Government Grant - Random player receives LKR 5,000",
        "National Disaster - Random developed property damaged"
    };
    for (int i = 0; i < MAX_CARDS; i++) {
        strncpy(gs->national_cards[i].description, descs[i], 63);
        gs->national_cards[i].description[63] = '\0';
        gs->national_cards[i].effect_type = i;
        gs->national_cards[i].duration = 5;
        gs->national_cards[i].magnitude = 0;
    }
    gs->card_deck_index = 0;
}

/* Shuffle national cards using Fisher-Yates */
static void shuffle_cards(GameState *gs) {
    NationalCard temp;
    for (int i = MAX_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        temp = gs->national_cards[i];
        gs->national_cards[i] = gs->national_cards[j];
        gs->national_cards[j] = temp;
    }
}

void move_player(GameState *gs, int player_idx, int steps) {
    Player *p = &gs->players[player_idx];
    int old_pos = p->position;
    int new_pos = (old_pos + steps) % BOARD_SIZE;

    p->position = new_pos;

    printf("%s moves from Square %d to Square %d.\n",
           p->strategy_name, old_pos, new_pos);

    /* Check if passed GO */
    if (old_pos + steps >= BOARD_SIZE) {
        printf("%s passed GO.\n", p->strategy_name);
        printf("Collected LKR %d.\n", GO_SALARY);
        p->cash += GO_SALARY;
        printf("Current Balance : LKR %d.\n", p->cash);
    }
}

void resolve_landing(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    BoardSquare *sq = &gs->board[p->position];

    switch (sq->type) {
        case SQUARE_GO:
            /* Already handled in move_player */
            break;
        case SQUARE_PROPERTY:
            handle_property_landing(gs, player_idx, sq->ref.property_index);
            break;
        case SQUARE_RAILWAY:
            handle_railway_landing(gs, player_idx, sq->ref.railway_index);
            break;
        case SQUARE_UTILITY:
            handle_utility_landing(gs, player_idx, sq->ref.utility_index);
            break;
        case SQUARE_TAX:
            handle_tax(gs, player_idx);
            break;
        case SQUARE_EVENT:
            handle_event_square(gs, player_idx);
            break;
        case SQUARE_INSURANCE:
            handle_insurance_square(gs, player_idx, p->position);
            break;
        case SQUARE_BANK:
            handle_bank_square(gs, player_idx);
            break;
        case SQUARE_SPECIAL:
            if (p->position == 30) {
                handle_go_to_jail(gs, player_idx);
            }
            /* position 10 (Jail/Just Visiting) and 20 (Free Parking) - nothing happens */
            break;
    }
}

void handle_property_landing(GameState *gs, int player_idx, int prop_idx) {
    if (prop_idx < 0 || prop_idx >= 22) return;
    Property *prop = &gs->properties[prop_idx];
    Player *p = &gs->players[player_idx];

    if (prop->damage_turns > 0) {
        /* Property damaged, no rent */
        printf("%s landed on %s. Property is damaged - no rent collected.\n",
               p->strategy_name, prop->name);
        return;
    }

    /* Check if property is closed by an event */
    if (p->modifiers.property_closed_index == prop_idx && p->modifiers.property_closed_rounds > 0) {
        printf("%s landed on %s. Property is closed due to event.\n",
               p->strategy_name, prop->name);
        return;
    }

    if (prop->owner == -1) {
        /* Unowned - player may purchase */
        printf("%s landed on unowned %s.\n", p->strategy_name, prop->name);
        if (should_buy_property(gs, player_idx, prop_idx)) {
            if (can_afford(gs, player_idx, prop->purchase_price)) {
                deduct_cash(gs, player_idx, prop->purchase_price);
                prop->owner = player_idx;
                gs->players[player_idx].properties_owned[gs->players[player_idx].prop_count++] = prop_idx;
                printf("%s purchased %s for LKR %d.\n",
                       p->strategy_name, prop->name, prop->purchase_price);
                printf("Remaining Balance : LKR %d.\n", p->cash);
            }
        } else {
            printf("%s declined to purchase %s.\n", p->strategy_name, prop->name);
            /* Property goes to auction */
            auction_property(gs, prop_idx);
        }
    } else if (prop->owner != player_idx) {
        /* Owned by another player - pay rent */
        if (!prop->mortgaged) {
            int dice = 0; /* rent doesn't depend on dice for properties */
            int rent = calculate_rent(gs, prop_idx, dice);
            printf("%s landed on %s.\n", p->strategy_name, prop->name);
            printf("Rent Paid : LKR %d.\n", rent);
            printf("Owner : %s.\n", gs->players[prop->owner].strategy_name);
            deduct_cash(gs, player_idx, rent);
            add_cash(gs, prop->owner, rent);
        } else {
            printf("%s landed on %s (mortgaged). No rent collected.\n",
                   p->strategy_name, prop->name);
        }
    } else {
        /* Own property - may renovate */
        if (should_renovate(gs, player_idx, prop_idx)) {
            renovate_property(gs, player_idx, prop_idx);
        }
    }
}

/* Simple railway/utility purchase decision based on strategy */
static bool should_buy_railway(GameState *gs, int player_idx, int price) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:
            return (p->cash - price) >= 250;
        case PLAYER_CONSERVATIVE:
            return (p->cash - price) >= (p->cash / 2);
        case PLAYER_RISK_TAKER:
            return p->cash >= price;
        case PLAYER_OPPORTUNISTIC:
            return (p->cash - price) >= 1000;
        default: return false;
    }
}

static bool should_buy_utility(GameState *gs, int player_idx, int price) {
    return should_buy_railway(gs, player_idx, price); /* Same logic */
}

void handle_railway_landing(GameState *gs, int player_idx, int rail_idx) {
    if (rail_idx < 0 || rail_idx >= 4) return;
    Railway *rail = &gs->railways[rail_idx];
    Player *p = &gs->players[player_idx];

    if (rail->owner == -1) {
        printf("%s landed on unowned %s.\n", p->strategy_name, rail->name);
        if (should_buy_railway(gs, player_idx, rail->purchase_price)) {
            if (can_afford(gs, player_idx, rail->purchase_price)) {
                deduct_cash(gs, player_idx, rail->purchase_price);
                rail->owner = player_idx;
                gs->players[player_idx].railways_owned[gs->players[player_idx].rail_count++] = rail_idx;
                printf("%s purchased %s for LKR %d.\n",
                       p->strategy_name, rail->name, rail->purchase_price);
                printf("Remaining Balance : LKR %d.\n", p->cash);
            }
        } else {
            printf("%s declined to purchase %s.\n", p->strategy_name, rail->name);
        }
    } else if (rail->owner != player_idx) {
        if (!rail->mortgaged) {
            int rent = calculate_railway_rent(gs, rail_idx);
            printf("%s landed on %s.\n", p->strategy_name, rail->name);
            printf("Rent Paid : LKR %d.\n", rent);
            printf("Owner : %s.\n", gs->players[rail->owner].strategy_name);
            deduct_cash(gs, player_idx, rent);
            add_cash(gs, rail->owner, rent);
        }
    }
}

void handle_utility_landing(GameState *gs, int player_idx, int util_idx) {
    if (util_idx < 0 || util_idx >= 2) return;
    Utility *util = &gs->utilities[util_idx];
    Player *p = &gs->players[player_idx];

    if (util->owner == -1) {
        printf("%s landed on unowned %s.\n", p->strategy_name, util->name);
        if (should_buy_utility(gs, player_idx, util->purchase_price)) {
            if (can_afford(gs, player_idx, util->purchase_price)) {
                deduct_cash(gs, player_idx, util->purchase_price);
                util->owner = player_idx;
                gs->players[player_idx].utilities_owned[gs->players[player_idx].util_count++] = util_idx;
                printf("%s purchased %s for LKR %d.\n",
                       p->strategy_name, util->name, util->purchase_price);
                printf("Remaining Balance : LKR %d.\n", p->cash);
            }
        } else {
            printf("%s declined to purchase %s.\n", p->strategy_name, util->name);
        }
    } else if (util->owner != player_idx) {
        if (!util->mortgaged) {
            int dice_val = roll_dice();
            int rent = calculate_utility_rent(gs, util_idx, dice_val);
            printf("%s landed on %s.\n", p->strategy_name, util->name);
            printf("Dice rolled: %d. Rent Paid : LKR %d.\n", dice_val, rent);
            printf("Owner : %s.\n", gs->players[util->owner].strategy_name);
            deduct_cash(gs, player_idx, rent);
            add_cash(gs, util->owner, rent);
        }
    }
}

void handle_tax(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    int tax = gs->income_tax_amount;
    printf("%s landed on Income Tax.\n", p->strategy_name);
    printf("Tax Paid : LKR %d.\n", tax);
    deduct_cash(gs, player_idx, tax);
}

void handle_event_square(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    printf("%s landed on National Event Card.\n", p->strategy_name);
    draw_national_card(gs, player_idx);
}

void handle_insurance_square(GameState *gs, int player_idx, int square_idx) {
    Player *p = &gs->players[player_idx];
    char *company = (square_idx == 17) ? "Sri Lanka Insurance" : "Ceylinco Insurance";
    printf("%s landed on %s.\n", p->strategy_name, company);

    /* Player may purchase/renew insurance for owned properties */
    for (int i = 0; i < p->prop_count; i++) {
        int prop_idx = p->properties_owned[i];
        if (prop_idx < 0 || prop_idx >= 22) continue;
        Property *prop = &gs->properties[prop_idx];
        if (prop->loan_locked) continue;

        InsuranceType ins_type;
        if (should_buy_insurance(gs, player_idx, prop_idx, &ins_type)) {
            purchase_insurance(gs, player_idx, prop_idx, ins_type);
        }
    }
}

void handle_bank_square(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    printf("%s landed on Bank of Ceylon.\n", p->strategy_name);

    /* Decide what banking action to take */
    if (p->active_loan.status == LOAN_ACTIVE) {
        /* Has existing loan - may repay, extend, or increase */
        if (should_repay_loan(gs, player_idx, NULL)) {
            int amount;
            if (should_repay_loan(gs, player_idx, &amount)) {
                if (amount >= p->active_loan.amount + p->active_loan.accumulated_interest) {
                    repay_loan_full(gs, player_idx);
                } else {
                    repay_loan(gs, player_idx, amount);
                }
            } else {
                repay_loan_full(gs, player_idx);
            }
        }
        /* Could also extend or increase - strategy determines this */
    } else {
        /* No active loan - may obtain one */
        int amount;
        if (should_obtain_loan(gs, player_idx, &amount)) {
            grant_loan(gs, player_idx, amount);
        }
    }
}

void handle_go_to_jail(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    printf("%s goes to Jail.\n", p->strategy_name);
    p->position = 10;
    p->in_jail = true;
    p->jail_turns = 0;
}

int get_property_group_size(PropertyGroup group) {
    switch (group) {
        case GROUP_BROWN:       return 2;
        case GROUP_LIGHT_BLUE:  return 3;
        case GROUP_PINK:        return 3;
        case GROUP_ORANGE:      return 3;
        case GROUP_RED:         return 3;
        case GROUP_YELLOW:      return 3;
        case GROUP_GREEN:       return 3;
        case GROUP_DARK_BLUE:   return 2;
        default:                return 0;
    }
}

bool player_has_monopoly(GameState *gs, int player_idx, PropertyGroup group) {
    Player *p = &gs->players[player_idx];
    int needed = get_property_group_size(group);
    int owned = 0;

    for (int i = 0; i < p->prop_count; i++) {
        int prop_idx = p->properties_owned[i];
        if (prop_idx >= 0 && prop_idx < 22 && gs->properties[prop_idx].group == group) {
            owned++;
        }
    }
    return owned >= needed;
}
