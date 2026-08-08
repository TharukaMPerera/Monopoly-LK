#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"

int roll_dice(void) {
    int d1 = (rand() % 6) + 1;
    int d2 = (rand() % 6) + 1;
    return d1 + d2;
}

void init_game(GameState *gs) {
    srand((unsigned int)time(NULL));

    memset(gs, 0, sizeof(GameState));

    /* Initialize players */
    const char *names[] = {"Aggressive Investor", "Conservative Banker",
                            "Risk Taker", "Opportunistic Trader"};
    PlayerStrategy strats[] = {PLAYER_AGGRESSIVE, PLAYER_CONSERVATIVE,
                                PLAYER_RISK_TAKER, PLAYER_OPPORTUNISTIC};

    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *p = &gs->players[i];
        p->id = i;
        strncpy(p->strategy_name, names[i], 31);
        p->strategy_name[31] = '\0';
        p->strategy = strats[i];
        p->cash = STARTING_CASH;
        p->position = 0;
        p->in_jail = false;
        p->jail_turns = 0;
        p->bankrupt = false;
        p->active_loan.status = LOAN_NONE;
        p->prop_count = 0;
        p->rail_count = 0;
        p->util_count = 0;
        for (int j = 0; j < 28; j++) p->properties_owned[j] = -1;
        for (int j = 0; j < 4; j++) p->railways_owned[j] = -1;
        for (int j = 0; j < 2; j++) p->utilities_owned[j] = -1;
        memset(&p->modifiers, 0, sizeof(PlayerModifiers));
        p->modifiers.property_closed_index = -1;
    }

    /* Initialize board components */
    init_board(gs);
    init_properties(gs);
    init_railways(gs);
    init_utilities(gs);
    init_national_cards(gs);

    /* Initialize game state */
    gs->current_round = 0;
    gs->current_player = 0;
    gs->game_over = false;
    gs->winner = -1;
    gs->inflation_rate = 0.0;
    gs->inflation_rounds_remaining = 500;
    gs->base_interest_rate = 8;
    gs->income_tax_amount = 1000;
    gs->base_income_tax = 1000;
    gs->disaster_countdown = 10;
    gs->inflation_countdown = 10;
    gs->economic_event_countdown = 15;
    gs->gov_regulation_countdown = 20;
    gs->regional_card_countdown = 15;
    gs->market_review_countdown = 10;
    gs->prop_age_countdown = 1;
    gs->insurance_premium_modifier = 0.0;
    gs->construction_cost_modifier = 0.0;
    gs->railway_rent_modifier = 0.0;
    gs->utility_rent_modifier = 0.0;

    for (int i = 0; i < 8; i++) {
        gs->group_last_boom_round[i] = -30;
        gs->group_last_decline_round[i] = -30;
    }
}

void determine_turn_order(GameState *gs) {
    printf("Determining the First Player\n");

    int rolls[MAX_PLAYERS];
    int highest = 0;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        int d1 = (rand() % 6) + 1;
        int d2 = (rand() % 6) + 1;
        rolls[i] = d1 + d2;
        printf("%s rolls %d.\n", gs->players[i].strategy_name, rolls[i]);
        if (rolls[i] > highest) highest = rolls[i];
    }

    /* Handle ties with rerolls */
    bool tied = true;
    while (tied) {
        tied = false;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            for (int j = i + 1; j < MAX_PLAYERS; j++) {
                if (rolls[i] == rolls[j] && rolls[i] == highest) {
                    tied = true;
                    int d1 = (rand() % 6) + 1;
                    int d2 = (rand() % 6) + 1;
                    rolls[i] = d1 + d2;
                    printf("%s rerolls %d.\n", gs->players[i].strategy_name, rolls[i]);
                    d1 = (rand() % 6) + 1;
                    d2 = (rand() % 6) + 1;
                    rolls[j] = d1 + d2;
                    printf("%s rerolls %d.\n", gs->players[j].strategy_name, rolls[j]);
                }
            }
        }
    }

    /* Sort by roll descending */
    int order[MAX_PLAYERS] = {0, 1, 2, 3};
    for (int i = 0; i < MAX_PLAYERS; i++) {
        for (int j = i + 1; j < MAX_PLAYERS; j++) {
            if (rolls[order[j]] > rolls[order[i]]) {
                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        gs->turn_order[i] = order[i];
    }

    printf("%s will begin the game.\n", gs->players[order[0]].strategy_name);
    printf("Turn order:\n");
    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("%s\n", gs->players[order[i]].strategy_name);
    }
}

void player_turn(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    if (p->bankrupt) return;

    /* Resolve pending decisions at start of turn */
    execute_player_decisions(gs, player_idx);

    /* Handle jail */
    if (p->in_jail) {
        /* execute_player_decisions already handled jail logic */
        return;
    }

    /* Roll dice and move */
    int dice = roll_dice();
    printf("%s rolled %d.\n", p->strategy_name, dice);

    move_player(gs, player_idx, dice);
    resolve_landing(gs, player_idx);

    /* Post-landing decisions: buy property if eligible */
    BoardSquare *sq = &gs->board[p->position];
    if (sq->type == SQUARE_PROPERTY) {
        int prop_idx = sq->ref.property_index;
        if (prop_idx >= 0 && prop_idx < 22 &&
            gs->properties[prop_idx].owner == -1) {
            /* Already handled in handle_property_landing */
        }
    }

    /* Development phase */
    while (should_develop(gs, player_idx)) {
        execute_development(gs, player_idx);
    }

    /* Check bankruptcy after all actions */
    check_bankruptcy(gs);
}

void end_round(GameState *gs) {
    /* Accumulate loan interest */
    accumulate_interest(gs);

    /* Process insurance expiry */
    process_insurance_expiry(gs);

    /* Apply property depreciation */
    apply_property_depreciation(gs);

    /* Apply building depreciation */
    apply_building_depreciation(gs);

    /* Check periodic events */
    check_market_review(gs);
    check_disasters(gs);
    check_economic_events(gs);
    check_gov_regulations(gs);
    check_regional_cards(gs);

    /* Update all prices and modifiers (includes inflation) */
    update_all_prices(gs);

    /* Check bankruptcies */
    check_bankruptcy(gs);

    /* Print round summary */
    print_round_summary(gs);

    /* Print market conditions */
    print_market_conditions(gs);

    /* Check game end */
    int solvent_count = 0;
    int last_solvent = -1;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!gs->players[i].bankrupt) {
            solvent_count++;
            last_solvent = i;
        }
    }

    if (solvent_count == 1) {
        gs->game_over = true;
        gs->winner = last_solvent;
    }
}

void print_round_summary(GameState *gs) {
    printf("=============================================\n");
    printf("Round %d Summary\n", gs->current_round);
    printf("=============================================\n");

    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *p = &gs->players[i];
        int nw = calculate_net_worth(gs, i);
        int hotels = 0;
        for (int j = 0; j < p->prop_count; j++) {
            int pi = p->properties_owned[j];
            if (pi >= 0 && pi < 22 && gs->properties[pi].has_hotel) hotels++;
        }

        printf("%s\n", p->strategy_name);
        printf("Cash : LKR %d\n", p->cash);
        printf("Net Worth : LKR %d\n", nw);
        printf("Properties : %d\n", p->prop_count);
        printf("Hotels : %d\n", hotels);
        if (p->active_loan.status == LOAN_ACTIVE) {
            printf("Outstanding Loan : LKR %d\n",
                   p->active_loan.amount + p->active_loan.accumulated_interest);
        } else {
            printf("Outstanding Loan : None\n");
        }
        if (p->bankrupt) {
            printf("STATUS : BANKRUPT\n");
        }
        printf("---------------------------------------------\n");
    }
    printf("=============================================\n");
}

void print_market_conditions(GameState *gs) {
    printf("=========================================\n");
    printf("Current Market Conditions\n");
    printf("=========================================\n");

    /* Find active boom group */
    int boom_group = -1, boom_rounds = 0;
    for (int i = 0; i < 22; i++) {
        if (gs->properties[i].in_boom) {
            boom_group = gs->properties[i].group;
            boom_rounds = gs->properties[i].boom_rounds_remaining;
            break;
        }
    }
    if (boom_group >= 0) {
        const char *gnames[] = {"Brown", "Light Blue", "Pink", "Orange", "Red", "Yellow", "Green", "Dark Blue"};
        printf("Market Boom\n");
        printf("-------------\n");
        printf("%s (+20%%)\n", gnames[boom_group]);
        printf("Rounds Remaining : %d\n", boom_rounds);
    }

    /* Find active decline group */
    int dec_group = -1, dec_rounds = 0;
    for (int i = 0; i < 22; i++) {
        if (gs->properties[i].in_decline) {
            dec_group = gs->properties[i].group;
            dec_rounds = gs->properties[i].decline_rounds_remaining;
            break;
        }
    }
    if (dec_group >= 0) {
        const char *gnames[] = {"Brown", "Light Blue", "Pink", "Orange", "Red", "Yellow", "Green", "Dark Blue"};
        printf("Market Decline\n");
        printf("----------------\n");
        printf("%s (-15%%)\n", gnames[dec_group]);
        printf("Rounds Remaining : %d\n", dec_rounds);
    }

    if (gs->regional_card_rounds_remaining > 0) {
        const char *rc_names[] = {
            "Southern Tourism Boom", "Port City Expansion", "IT Industry Growth",
            "Northern Development Programme", "Tea Export Boom", "Airport Expansion",
            "University City Growth", "Beach Pollution", "Flood Damage",
            "Transport Strike", "Electricity Tariff Increase", "Water Shortage"
        };
        printf("Regional Development\n");
        printf("-----------------------\n");
        printf("%s\n", rc_names[gs->current_regional_card]);
        printf("Rounds Remaining : %d\n", gs->regional_card_rounds_remaining);
    }

    printf("Inflation\n");
    printf("------------\n");
    printf("%+.0f%%\n", gs->inflation_rate * 100);

    printf("Current Loan Interest\n");
    printf("-----------------------\n");
    printf("%d%%\n", gs->base_interest_rate);

    printf("=========================================\n");
}

void run_simulation(GameState *gs) {
    printf("MONOPOLY-LK Simulation\n");

    for (int i = 0; i < MAX_PLAYERS; i++) {
        printf("Player %d : %s\n", i + 1, gs->players[i].strategy_name);
    }
    printf("Each player begins with LKR %d.\n", STARTING_CASH);

    determine_turn_order(gs);

    /* Main game loop */
    while (!gs->game_over && gs->current_round < MAX_ROUNDS) {
        gs->current_round++;

        printf("\n=============================================\n");
        printf("ROUND %d\n", gs->current_round);
        printf("=============================================\n");

        /* Each player takes a turn */
        for (int t = 0; t < MAX_PLAYERS; t++) {
            int player_idx = gs->turn_order[t];
            if (gs->players[player_idx].bankrupt) continue;

            printf("\n--- %s's Turn ---\n", gs->players[player_idx].strategy_name);
            player_turn(gs, player_idx);

            if (gs->game_over) break;
        }

        if (!gs->game_over) {
            end_round(gs);
        }
    }

    declare_winner(gs);
}

void declare_winner(GameState *gs) {
    printf("\n");
    printf("=============================================\n");
    printf("GAME OVER\n");

    int winner = gs->winner;
    if (winner < 0) {
        /* Find player with highest net worth */
        int best_nw = -999999999;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            if (!gs->players[i].bankrupt) {
                int nw = calculate_net_worth(gs, i);
                if (nw > best_nw) {
                    best_nw = nw;
                    winner = i;
                }
            }
        }
    }

    if (winner >= 0) {
        Player *p = &gs->players[winner];
        int nw = calculate_net_worth(gs, winner);
        printf("Winner\n");
        printf("%s\n", p->strategy_name);
        printf("Total Cash\n");
        printf("LKR %d\n", p->cash);

        int prop_value = 0;
        for (int i = 0; i < p->prop_count; i++) {
            int pi = p->properties_owned[i];
            if (pi >= 0 && pi < 22) {
                prop_value += get_property_market_value(gs, pi);
            }
        }
        printf("Total Property Value\n");
        printf("LKR %d\n", prop_value);

        if (p->active_loan.status == LOAN_ACTIVE) {
            printf("Outstanding Loans\n");
            printf("LKR %d\n", p->active_loan.amount + p->active_loan.accumulated_interest);
        } else {
            printf("Outstanding Loans\n");
            printf("None\n");
        }
        printf("Net Worth\n");
        printf("LKR %d\n", nw);
    }

    printf("=============================================\n");
}
