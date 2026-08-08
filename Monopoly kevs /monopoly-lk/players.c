#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/* Forward declarations */
extern int roll_dice(void);
extern bool can_afford(GameState *gs, int player_idx, int amount);
extern int get_max_loan_amount(GameState *gs, int player_idx);
extern bool player_has_monopoly(GameState *gs, int player_idx, PropertyGroup group);
extern int get_property_market_value(GameState *gs, int prop_idx);
extern int calculate_rent(GameState *gs, int prop_idx, int dice_value);

/* Check if player can build evenly within a group */
static int can_build_evenly(GameState *gs, int player_idx, PropertyGroup group, int target_houses) {
    Player *p = &gs->players[player_idx];
    int size = 0;
    int props_in_group[3];
    int min_houses = 5; /* max possible */

    for (int i = 0; i < p->prop_count; i++) {
        int pi = p->properties_owned[i];
        if (pi >= 0 && pi < 22 && gs->properties[pi].group == group) {
            props_in_group[size++] = pi;
            if (gs->properties[pi].has_hotel) {
                min_houses = 5;
            } else if (gs->properties[pi].num_houses < min_houses) {
                min_houses = gs->properties[pi].num_houses;
            }
        }
    }
    if (size == 0) return 0;
    /* Can build up to (min_houses + 1) evenly */
    return (target_houses <= min_houses + 1) ? size : 0;
}

/* ---------- AGGRESSIVE INVESTOR ---------- */
static bool aggressive_buy_property(GameState *gs, int player_idx, int prop_idx) {
    Player *p = &gs->players[player_idx];
    Property *prop = &gs->properties[prop_idx];
    /* Always buy if can afford at least one future rent */
    int reserve = (prop->base_rent > 0) ? prop->base_rent : 250;
    return (p->cash - prop->purchase_price) >= reserve;
}

static int aggressive_bid_limit(GameState *gs, int player_idx, int prop_idx) {
    Property *prop = &gs->properties[prop_idx];
    int market_val = get_property_market_value(gs, prop_idx);
    int limit = (int)(market_val * 1.2);
    if (limit > gs->players[player_idx].cash) limit = gs->players[player_idx].cash;
    return limit;
}

static bool aggressive_develop(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    /* Build whenever monopoly exists and funds allow */
    for (int g = 0; g < 8; g++) {
        if (player_has_monopoly(gs, player_idx, g)) {
            /* Find property in group with fewest buildings */
            for (int i = 0; i < p->prop_count; i++) {
                int pi = p->properties_owned[i];
                if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                    Property *prop = &gs->properties[pi];
                    if (!prop->mortgaged && !prop->loan_locked) {
                        /* Check if we can upgrade to hotel (have 4 houses) */
                        if (prop->num_houses >= 4 && !prop->has_hotel) {
                            if (can_afford(gs, player_idx, prop->hotel_cost)) {
                                printf("%s constructed one house on %s.\n",
                                       p->strategy_name, prop->name);
                                printf("Construction Cost : LKR %d.\n", prop->house_cost);
                                return true;
                            }
                        }
                        /* Build a house */
                        if (prop->num_houses < 4 && !prop->has_hotel) {
                            if (can_afford(gs, player_idx, prop->house_cost)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

static bool aggressive_should_insure(GameState *gs, int player_idx, int prop_idx, InsuranceType *out) {
    Property *prop = &gs->properties[prop_idx];
    if (prop->insurance.active) return false;
    if (prop->has_hotel) {
        *out = INSURANCE_COMPREHENSIVE;
        return true;
    }
    if (prop->num_houses > 0) {
        *out = INSURANCE_BASIC;
        return true;
    }
    return false;
}

static bool aggressive_should_loan(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status == LOAN_ACTIVE) return false;
    int max_loan = get_max_loan_amount(gs, player_idx);
    if (max_loan < 500) return false;
    /* Get loan if it increases projected rental income potential */
    *amount = max_loan;
    return true;
}

static bool aggressive_should_repay(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return false;
    int total_owed = p->active_loan.amount + p->active_loan.accumulated_interest;
    /* Repay only when cash exceeds 2x outstanding */
    if (p->cash > total_owed * 2) {
        if (amount) *amount = total_owed;
        return true;
    }
    return false;
}

/* ---------- CONSERVATIVE BANKER ---------- */
static bool conservative_buy_property(GameState *gs, int player_idx, int prop_idx) {
    Player *p = &gs->players[player_idx];
    Property *prop = &gs->properties[prop_idx];
    /* Buy only if 50% cash remains */
    return (p->cash - prop->purchase_price) >= (p->cash / 2);
}

static int conservative_bid_limit(GameState *gs, int player_idx, int prop_idx) {
    int market_val = get_property_market_value(gs, prop_idx);
    int limit = market_val; /* below market value only */
    if (limit > gs->players[player_idx].cash) limit = gs->players[player_idx].cash;
    return limit;
}

static bool conservative_develop(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    /* No hotels until loans settled */
    if (p->active_loan.status == LOAN_ACTIVE) return false;
    /* Only build houses if we have substantial cash reserve */
    if (p->cash < 10000) return false;

    for (int g = 0; g < 8; g++) {
        if (player_has_monopoly(gs, player_idx, g)) {
            for (int i = 0; i < p->prop_count; i++) {
                int pi = p->properties_owned[i];
                if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                    Property *prop = &gs->properties[pi];
                    if (!prop->mortgaged && prop->num_houses < 3 && !prop->has_hotel) {
                        if (can_afford(gs, player_idx, prop->house_cost)) {
                            /* Keep 40% cash reserve */
                            if ((p->cash - prop->house_cost) >= (p->cash * 40 / 100)) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

static bool conservative_should_insure(GameState *gs, int player_idx, int prop_idx, InsuranceType *out) {
    Property *prop = &gs->properties[prop_idx];
    if (prop->insurance.active) return false;
    /* Comprehensive for all developed properties */
    if (prop->num_houses > 0 || prop->has_hotel) {
        *out = INSURANCE_COMPREHENSIVE;
        return true;
    }
    return false;
}

static bool conservative_should_loan(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status == LOAN_ACTIVE) return false;
    /* Only borrow if bankruptcy is imminent */
    if (p->cash < 200) {
        int max_loan = get_max_loan_amount(gs, player_idx);
        if (max_loan > 0) {
            *amount = max_loan / 2; /* borrow minimally */
            return true;
        }
    }
    return false;
}

static bool conservative_should_repay(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return false;
    int total_owed = p->active_loan.amount + p->active_loan.accumulated_interest;
    /* Repay immediately if possible */
    if (p->cash >= total_owed && p->cash - total_owed >= 5000) {
        if (amount) *amount = total_owed;
        return true;
    }
    /* Partial repayment */
    if (p->cash > 20000) {
        if (amount) *amount = p->cash - 10000;
        return true;
    }
    return false;
}

/* ---------- RISK TAKER ---------- */
static bool risktaker_buy_property(GameState *gs, int player_idx, int prop_idx) {
    Property *prop = &gs->properties[prop_idx];
    Player *p = &gs->players[player_idx];
    /* Always buy if possible */
    return can_afford(gs, player_idx, prop->purchase_price);
}

static int risktaker_bid_limit(GameState *gs, int player_idx, int prop_idx) {
    (void)prop_idx;
    /* Bid until cash exhausted */
    return gs->players[player_idx].cash;
}

static bool risktaker_develop(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    /* Build hotels ASAP */
    for (int g = GROUP_DARK_BLUE; g >= 0; g--) {
        if (player_has_monopoly(gs, player_idx, g)) {
            for (int i = 0; i < p->prop_count; i++) {
                int pi = p->properties_owned[i];
                if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                    Property *prop = &gs->properties[pi];
                    if (!prop->mortgaged && !prop->loan_locked) {
                        if (prop->num_houses >= 4 && !prop->has_hotel) {
                            if (can_afford(gs, player_idx, prop->hotel_cost)) return true;
                        }
                        if (prop->num_houses < 4 && !prop->has_hotel) {
                            if (can_afford(gs, player_idx, prop->house_cost)) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

static bool risktaker_should_insure(GameState *gs, int player_idx, int prop_idx, InsuranceType *out) {
    (void)player_idx;
    (void)prop_idx;
    (void)out;
    /* Only buy insurance after experiencing financial loss */
    return false; /* simplified - in reality would track past losses */
}

static bool risktaker_should_loan(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status == LOAN_ACTIVE) {
        /* Try to increase loan */
        int max_loan = get_max_loan_amount(gs, player_idx);
        int current_total = p->active_loan.amount + p->active_loan.accumulated_interest;
        if (max_loan > current_total + 1000) {
            return false; /* would need a separate increase_loan path */
        }
        return false;
    }
    int max_loan = get_max_loan_amount(gs, player_idx);
    if (max_loan > 0) {
        *amount = max_loan;
        return true;
    }
    return false;
}

static bool risktaker_should_repay(GameState *gs, int player_idx, int *amount) {
    (void)gs;
    (void)player_idx;
    (void)amount;
    /* Risk taker rarely repays voluntarily */
    return false;
}

/* ---------- OPPORTUNISTIC TRADER ---------- */
static bool opportunistic_buy_property(GameState *gs, int player_idx, int prop_idx) {
    Player *p = &gs->players[player_idx];
    Property *prop = &gs->properties[prop_idx];
    /* Buy only if projected appreciation exceeds construction costs */
    double appreciation = 0.05; /* assumed base appreciation */
    if (prop->in_boom) appreciation += 0.15;
    if (prop->in_decline) appreciation -= 0.10;
    if (appreciation <= 0) return false;
    int projected_value = (int)(prop->purchase_price * (1 + appreciation));
    return (projected_value > prop->purchase_price) && can_afford(gs, player_idx, prop->purchase_price);
}

static int opportunistic_bid_limit(GameState *gs, int player_idx, int prop_idx) {
    int market_val = get_property_market_value(gs, prop_idx);
    /* Bid up to market value for discount auctions */
    int limit = market_val * 90 / 100;
    if (limit > gs->players[player_idx].cash) limit = gs->players[player_idx].cash;
    return limit;
}

static bool opportunistic_develop(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    /* Delay during inflation, accelerate during housing subsidy */
    bool subsidy_active = gs->current_gov_regulation == GOV_HOUSING_SUBSIDY;
    bool inflation_high = gs->inflation_rate > 0.05;

    if (inflation_high && !subsidy_active && p->cash < 15000) return false;

    for (int g = 0; g < 8; g++) {
        if (player_has_monopoly(gs, player_idx, g)) {
            /* Check if group is in boom (good to build) */
            bool group_boom = false;
            for (int i = 0; i < p->prop_count; i++) {
                int pi = p->properties_owned[i];
                if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                    if (gs->properties[pi].in_boom) group_boom = true;
                }
            }
            bool should_build = group_boom || subsidy_active || !inflation_high;

            if (should_build) {
                for (int i = 0; i < p->prop_count; i++) {
                    int pi = p->properties_owned[i];
                    if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                        Property *prop = &gs->properties[pi];
                        if (!prop->mortgaged && !prop->loan_locked && !prop->has_hotel) {
                            if (prop->num_houses < 4) {
                                if (can_afford(gs, player_idx, prop->house_cost)) return true;
                            }
                            if (prop->num_houses >= 4 && !prop->has_hotel) {
                                if (can_afford(gs, player_idx, prop->hotel_cost)) return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

static bool opportunistic_should_insure(GameState *gs, int player_idx, int prop_idx, InsuranceType *out) {
    Property *prop = &gs->properties[prop_idx];
    if (prop->insurance.active) return false;
    /* Only insure high-value developments */
    if (prop->has_hotel) {
        *out = INSURANCE_COMPREHENSIVE;
        return true;
    }
    return false;
}

static bool opportunistic_should_loan(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status == LOAN_ACTIVE) return false;
    /* Borrow only if projected return exceeds borrowing cost */
    int max_loan = get_max_loan_amount(gs, player_idx);
    if (max_loan < 2000) return false;
    int interest_rate = gs->base_interest_rate;
    if (interest_rate > 10) return false; /* too expensive */

    /* Check if we have monopolies to develop */
    for (int g = 0; g < 8; g++) {
        if (player_has_monopoly(gs, player_idx, g)) {
            *amount = max_loan / 2;
            return true;
        }
    }
    return false;
}

static bool opportunistic_should_repay(GameState *gs, int player_idx, int *amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return false;
    int total_owed = p->active_loan.amount + p->active_loan.accumulated_interest;
    /* Repay when interest is high and we have cash */
    if (gs->base_interest_rate >= 12 && p->cash >= total_owed) {
        if (amount) *amount = total_owed;
        return true;
    }
    return false;
}

/* ---------- PUBLIC INTERFACE ---------- */

bool should_buy_property(GameState *gs, int player_idx, int prop_idx) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:    return aggressive_buy_property(gs, player_idx, prop_idx);
        case PLAYER_CONSERVATIVE:  return conservative_buy_property(gs, player_idx, prop_idx);
        case PLAYER_RISK_TAKER:    return risktaker_buy_property(gs, player_idx, prop_idx);
        case PLAYER_OPPORTUNISTIC: return opportunistic_buy_property(gs, player_idx, prop_idx);
        default: return false;
    }
}

int get_auction_bid_limit(GameState *gs, int player_idx, int prop_idx) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:    return aggressive_bid_limit(gs, player_idx, prop_idx);
        case PLAYER_CONSERVATIVE:  return conservative_bid_limit(gs, player_idx, prop_idx);
        case PLAYER_RISK_TAKER:    return risktaker_bid_limit(gs, player_idx, prop_idx);
        case PLAYER_OPPORTUNISTIC: return opportunistic_bid_limit(gs, player_idx, prop_idx);
        default: return 0;
    }
}

bool should_develop(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    /* Check if construction is suspended */
    if (p->modifiers.construction_suspended && p->modifiers.construction_suspended_rounds > 0)
        return false;
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:    return aggressive_develop(gs, player_idx);
        case PLAYER_CONSERVATIVE:  return conservative_develop(gs, player_idx);
        case PLAYER_RISK_TAKER:    return risktaker_develop(gs, player_idx);
        case PLAYER_OPPORTUNISTIC: return opportunistic_develop(gs, player_idx);
        default: return false;
    }
}

void execute_development(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    /* Try to build the cheapest possible house/hotel across all monopolies */
    int best_prop = -1;
    int best_cost = 99999999;
    bool build_hotel = false;

    for (int g = GROUP_DARK_BLUE; g >= 0; g--) {
        if (!player_has_monopoly(gs, player_idx, g)) continue;

        /* Find the minimum houses in this group */
        int min_houses = 5;
        for (int i = 0; i < p->prop_count; i++) {
            int pi = p->properties_owned[i];
            if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                if (!gs->properties[pi].has_hotel) {
                    if (gs->properties[pi].num_houses < min_houses) {
                        min_houses = gs->properties[pi].num_houses;
                    }
                }
            }
        }

        if (min_houses >= 5) continue; /* All have hotels */

        for (int i = 0; i < p->prop_count; i++) {
            int pi = p->properties_owned[i];
            if (pi >= 0 && pi < 22 && gs->properties[pi].group == g) {
                Property *prop = &gs->properties[pi];
                if (prop->mortgaged || prop->loan_locked) continue;

                if (prop->num_houses >= 4 && !prop->has_hotel && prop->num_houses <= min_houses + 1) {
                    /* Can build hotel */
                    int cost = prop->hotel_cost;
                    /* Apply gov regulation modifiers */
                    if (gs->current_gov_regulation == GOV_HOUSING_SUBSIDY)
                        cost = cost * 70 / 100;
                    cost = (int)(cost * (1.0 + gs->construction_cost_modifier));

                    if (cost < best_cost && can_afford(gs, player_idx, cost)) {
                        best_cost = cost;
                        best_prop = pi;
                        build_hotel = true;
                    }
                } else if (prop->num_houses < 4 && !prop->has_hotel && prop->num_houses <= min_houses) {
                    int cost = prop->house_cost;
                    if (gs->current_gov_regulation == GOV_HOUSING_SUBSIDY)
                        cost = cost * 70 / 100;
                    cost = (int)(cost * (1.0 + gs->construction_cost_modifier));

                    if (cost < best_cost && can_afford(gs, player_idx, cost)) {
                        best_cost = cost;
                        best_prop = pi;
                        build_hotel = false;
                    }
                }
            }
        }
    }

    if (best_prop >= 0) {
        Property *prop = &gs->properties[best_prop];
        deduct_cash(gs, player_idx, best_cost);

        if (build_hotel) {
            printf("%s upgraded %s to a Hotel.\n", p->strategy_name, prop->name);
            prop->has_hotel = true;
            prop->num_houses = 0; /* Hotel replaces houses */
        } else {
            printf("%s constructed one house on %s.\n", p->strategy_name, prop->name);
            printf("Construction Cost : LKR %d.\n", best_cost);
            prop->num_houses++;
        }
    }
}

bool should_buy_insurance(GameState *gs, int player_idx, int prop_idx, InsuranceType *out_type) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:    return aggressive_should_insure(gs, player_idx, prop_idx, out_type);
        case PLAYER_CONSERVATIVE:  return conservative_should_insure(gs, player_idx, prop_idx, out_type);
        case PLAYER_RISK_TAKER:    return risktaker_should_insure(gs, player_idx, prop_idx, out_type);
        case PLAYER_OPPORTUNISTIC: return opportunistic_should_insure(gs, player_idx, prop_idx, out_type);
        default: return false;
    }
}

bool should_obtain_loan(GameState *gs, int player_idx, int *out_amount) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:    return aggressive_should_loan(gs, player_idx, out_amount);
        case PLAYER_CONSERVATIVE:  return conservative_should_loan(gs, player_idx, out_amount);
        case PLAYER_RISK_TAKER:    return risktaker_should_loan(gs, player_idx, out_amount);
        case PLAYER_OPPORTUNISTIC: return opportunistic_should_loan(gs, player_idx, out_amount);
        default: return false;
    }
}

bool should_repay_loan(GameState *gs, int player_idx, int *out_amount) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:    return aggressive_should_repay(gs, player_idx, out_amount);
        case PLAYER_CONSERVATIVE:  return conservative_should_repay(gs, player_idx, out_amount);
        case PLAYER_RISK_TAKER:    return risktaker_should_repay(gs, player_idx, out_amount);
        case PLAYER_OPPORTUNISTIC: return opportunistic_should_repay(gs, player_idx, out_amount);
        default: return false;
    }
}

bool should_renovate(GameState *gs, int player_idx, int prop_idx) {
    Player *p = &gs->players[player_idx];
    Property *prop = &gs->properties[prop_idx];
    if (prop->depreciation_pct == 0) return false;

    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:
            return prop->depreciation_pct >= 5;
        case PLAYER_CONSERVATIVE:
            return prop->depreciation_pct >= 10;
        case PLAYER_RISK_TAKER:
            return prop->depreciation_pct >= 20;
        case PLAYER_OPPORTUNISTIC:
            return prop->depreciation_pct >= 15;
        default: return false;
    }
}

bool should_sell_property(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    switch (p->strategy) {
        case PLAYER_AGGRESSIVE:
            return false; /* never voluntarily sell */
        case PLAYER_CONSERVATIVE:
            return false;
        case PLAYER_RISK_TAKER:
            /* Sell lower-value to finance premium */
            if (p->cash < 500 && p->prop_count > 3) return true;
            return false;
        case PLAYER_OPPORTUNISTIC:
            /* Sell properties expected to decrease */
            for (int i = 0; i < p->prop_count; i++) {
                int pi = p->properties_owned[i];
                if (pi >= 0 && pi < 22 && gs->properties[pi].in_decline)
                    return true;
            }
            return false;
        default: return false;
    }
}

int player_bid_amount(GameState *gs, int player_idx, int current_bid, int prop_idx) {
    Player *p = &gs->players[player_idx];
    int limit = get_auction_bid_limit(gs, player_idx, prop_idx);

    /* Conservative player withdraws if bid exceeds limit */
    if (p->strategy == PLAYER_CONSERVATIVE && current_bid >= limit) {
        return -1; /* withdraw */
    }

    int bid = current_bid + AUCTION_INCREMENT;
    if (bid > limit) return -1; /* can't bid further */
    if (bid > p->cash) return -1;

    /* Risk taker and aggressive bid more aggressively */
    if (p->strategy == PLAYER_RISK_TAKER || p->strategy == PLAYER_AGGRESSIVE) {
        /* Jump bid sometimes */
        int jump = current_bid + AUCTION_INCREMENT * (1 + rand() % 3);
        if (jump <= limit && jump <= p->cash) {
            bid = jump;
        }
    }

    return bid;
}

void execute_player_decisions(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];

    /* 1. Perform maintenance at start of turn */
    perform_maintenance(gs, player_idx);

    /* 2. Resolve jail */
    if (p->in_jail) {
        p->jail_turns++;
        /* Try to pay bail or roll doubles */
        if (p->cash >= JAIL_BAIL &&
            (p->strategy == PLAYER_AGGRESSIVE || p->strategy == PLAYER_RISK_TAKER)) {
            p->cash -= JAIL_BAIL;
            p->in_jail = false;
            p->jail_turns = 0;
            printf("%s paid bail of LKR %d.\n", p->strategy_name, JAIL_BAIL);
        } else {
            int d1 = (rand() % 6) + 1;
            int d2 = (rand() % 6) + 1;
            if (d1 == d2) {
                p->in_jail = false;
                p->jail_turns = 0;
                printf("%s rolled doubles and left jail.\n", p->strategy_name);
            } else if (p->jail_turns >= MAX_JAIL_TURNS) {
                p->cash -= JAIL_BAIL;
                if (p->cash < 0) p->cash = 0;
                p->in_jail = false;
                p->jail_turns = 0;
                printf("%s released from jail after 3 turns.\n", p->strategy_name);
            } else {
                printf("%s remains in jail (turn %d).\n", p->strategy_name, p->jail_turns);
                return; /* No further actions in jail */
            }
        }
    }

    /* 3. Handle building depreciation - check for renovation needs */
    for (int i = 0; i < p->prop_count; i++) {
        int pi = p->properties_owned[i];
        if (pi >= 0 && pi < 22) {
            Property *prop = &gs->properties[pi];
            if (prop->structural_damage && should_renovate(gs, player_idx, pi)) {
                renovate_property(gs, player_idx, pi);
            }
        }
    }
}
