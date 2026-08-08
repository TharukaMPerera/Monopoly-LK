#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/* Forward declarations */
extern int roll_dice(void);
extern bool player_has_monopoly(GameState *gs, int player_idx, PropertyGroup group);
extern int get_property_group_size(PropertyGroup group);

bool can_afford(GameState *gs, int player_idx, int amount) {
    return gs->players[player_idx].cash >= amount;
}

void deduct_cash(GameState *gs, int player_idx, int amount) {
    Player *p = &gs->players[player_idx];
    p->cash -= amount;
    if (p->cash < 0) p->cash = 0;
}

void add_cash(GameState *gs, int player_idx, int amount) {
    gs->players[player_idx].cash += amount;
}

int get_max_loan_amount(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    int total_mortgage = 0;

    for (int i = 0; i < p->prop_count; i++) {
        int pi = p->properties_owned[i];
        if (pi >= 0 && pi < 22) {
            Property *prop = &gs->properties[pi];
            if (!prop->mortgaged && !prop->loan_locked) {
                total_mortgage += prop->mortgage_value;
            }
        }
    }
    for (int i = 0; i < p->rail_count; i++) {
        int ri = p->railways_owned[i];
        if (ri >= 0 && ri < 4) {
            if (!gs->railways[ri].mortgaged && !gs->railways[ri].loan_locked) {
                total_mortgage += gs->railways[ri].mortgage_value;
            }
        }
    }
    for (int i = 0; i < p->util_count; i++) {
        int ui = p->utilities_owned[i];
        if (ui >= 0 && ui < 2) {
            if (!gs->utilities[ui].mortgaged && !gs->utilities[ui].loan_locked) {
                total_mortgage += gs->utilities[ui].mortgage_value;
            }
        }
    }
    return (int)(total_mortgage * 0.75);
}

void grant_loan(GameState *gs, int player_idx, int amount) {
    Player *p = &gs->players[player_idx];
    p->active_loan.amount = amount;
    p->active_loan.accumulated_interest = 0;
    p->active_loan.interest_rate = gs->base_interest_rate;
    p->active_loan.duration_remaining = LOAN_DURATION;
    p->active_loan.status = LOAN_ACTIVE;
    p->active_loan.collateral_count = 0;
    p->cash += amount;

    /* Mark eligible properties as collateral */
    for (int i = 0; i < p->prop_count && p->active_loan.collateral_count < 20; i++) {
        int pi = p->properties_owned[i];
        if (pi >= 0 && pi < 22) {
            Property *prop = &gs->properties[pi];
            if (!prop->mortgaged && !prop->loan_locked) {
                prop->loan_locked = true;
                p->active_loan.collateral_indices[p->active_loan.collateral_count++] = pi;
            }
        }
    }
    for (int i = 0; i < p->rail_count && p->active_loan.collateral_count < 20; i++) {
        int ri = p->railways_owned[i];
        if (ri >= 0 && ri < 4 && !gs->railways[ri].mortgaged && !gs->railways[ri].loan_locked) {
            gs->railways[ri].loan_locked = true;
            p->active_loan.collateral_indices[p->active_loan.collateral_count++] = -(ri + 1); /* negative for railway */
        }
    }
    for (int i = 0; i < p->util_count && p->active_loan.collateral_count < 20; i++) {
        int ui = p->utilities_owned[i];
        if (ui >= 0 && ui < 2 && !gs->utilities[ui].mortgaged && !gs->utilities[ui].loan_locked) {
            gs->utilities[ui].loan_locked = true;
            p->active_loan.collateral_indices[p->active_loan.collateral_count++] = -(ui + 101); /* negative for utility */
        }
    }

    printf("%s obtained a secured loan.\n", p->strategy_name);
    printf("Loan Amount : LKR %d.\n", amount);
    printf("Collateral :\n");
    for (int i = 0; i < p->active_loan.collateral_count; i++) {
        int idx = p->active_loan.collateral_indices[i];
        if (idx >= 0) {
            printf("%s\n", gs->properties[idx].name);
        } else if (idx <= -101) {
            printf("%s\n", gs->utilities[-(idx + 101)].name);
        } else {
            printf("%s\n", gs->railways[-(idx + 1)].name);
        }
    }
    printf("Interest Rate : %d%%\n", p->active_loan.interest_rate);
    printf("Duration : %d Rounds\n", LOAN_DURATION);
}

bool repay_loan(GameState *gs, int player_idx, int amount) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return false;

    int total_owed = p->active_loan.amount + p->active_loan.accumulated_interest;
    if (amount > p->cash) amount = p->cash;
    if (amount > total_owed) amount = total_owed;

    p->cash -= amount;

    /* Pay interest first, then principal */
    if (amount >= p->active_loan.accumulated_interest) {
        amount -= p->active_loan.accumulated_interest;
        p->active_loan.accumulated_interest = 0;
        p->active_loan.amount -= amount;
    } else {
        p->active_loan.accumulated_interest -= amount;
        amount = 0;
    }

    printf("%s repaid LKR %d.\n", p->strategy_name, amount > 0 ? amount + p->active_loan.accumulated_interest : amount);
    printf("Outstanding Balance : LKR %d.\n",
           p->active_loan.amount + p->active_loan.accumulated_interest);

    /* Check if fully repaid */
    if (p->active_loan.amount <= 0 && p->active_loan.accumulated_interest <= 0) {
        p->active_loan.status = LOAN_REPAID;
        /* Release collateral */
        for (int i = 0; i < p->active_loan.collateral_count; i++) {
            int idx = p->active_loan.collateral_indices[i];
            if (idx >= 0 && idx < 22) {
                gs->properties[idx].loan_locked = false;
            } else if (idx <= -101) {
                int ui = -(idx + 101);
                if (ui >= 0 && ui < 2) gs->utilities[ui].loan_locked = false;
            } else {
                int ri = -(idx + 1);
                if (ri >= 0 && ri < 4) gs->railways[ri].loan_locked = false;
            }
        }
    }
    return true;
}

void repay_loan_full(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    int total = p->active_loan.amount + p->active_loan.accumulated_interest;
    repay_loan(gs, player_idx, total);
}

void extend_loan(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return;
    p->active_loan.duration_remaining += 10;
    printf("%s extended loan period by 10 rounds.\n", p->strategy_name);
}

void increase_loan(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return;
    int max_loan = get_max_loan_amount(gs, player_idx);
    int current_total = p->active_loan.amount + p->active_loan.accumulated_interest;
    if (max_loan > current_total) {
        int additional = max_loan - current_total;
        p->active_loan.amount += additional;
        p->cash += additional;
        printf("%s increased loan by LKR %d.\n", p->strategy_name, additional);
    }
}

void process_loan_default(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    if (p->active_loan.status != LOAN_ACTIVE) return;

    printf("%s has defaulted.\n", p->strategy_name);

    /* Foreclose collateral */
    for (int i = 0; i < p->active_loan.collateral_count; i++) {
        int idx = p->active_loan.collateral_indices[i];
        if (idx >= 0 && idx < 22) {
            Property *prop = &gs->properties[idx];
            prop->owner = -1;
            prop->num_houses = 0;
            prop->has_hotel = false;
            prop->mortgaged = false;
            prop->loan_locked = false;
            prop->insurance.type = INSURANCE_NONE;
            prop->insurance.active = false;
            prop->building_condition = 100;
            prop->structural_damage = false;
            prop->maintenance_skipped = 0;
            /* Remove from player's property list */
            for (int j = 0; j < p->prop_count; j++) {
                if (p->properties_owned[j] == idx) {
                    /* Shift remaining */
                    for (int k = j; k < p->prop_count - 1; k++)
                        p->properties_owned[k] = p->properties_owned[k + 1];
                    p->prop_count--;
                    break;
                }
            }
        } else if (idx <= -101) {
            int ui = -(idx + 101);
            if (ui >= 0 && ui < 2) {
                gs->utilities[ui].owner = -1;
                gs->utilities[ui].loan_locked = false;
                gs->utilities[ui].mortgaged = false;
                for (int j = 0; j < p->util_count; j++) {
                    if (p->utilities_owned[j] == ui) {
                        for (int k = j; k < p->util_count - 1; k++)
                            p->utilities_owned[k] = p->utilities_owned[k + 1];
                        p->util_count--;
                        break;
                    }
                }
            }
        } else {
            int ri = -(idx + 1);
            if (ri >= 0 && ri < 4) {
                gs->railways[ri].owner = -1;
                gs->railways[ri].loan_locked = false;
                gs->railways[ri].mortgaged = false;
                for (int j = 0; j < p->rail_count; j++) {
                    if (p->railways_owned[j] == ri) {
                        for (int k = j; k < p->rail_count - 1; k++)
                            p->railways_owned[k] = p->railways_owned[k + 1];
                        p->rail_count--;
                        break;
                    }
                }
            }
        }
    }

    printf("Collateral has been foreclosed.\n");
    printf("Outstanding debt cleared.\n");

    p->active_loan.status = LOAN_DEFAULTED;
    p->active_loan.amount = 0;
    p->active_loan.accumulated_interest = 0;
    p->active_loan.collateral_count = 0;
}

void accumulate_interest(GameState *gs) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *p = &gs->players[i];
        if (p->bankrupt) continue;
        if (p->active_loan.status == LOAN_ACTIVE) {
            int interest = (int)(p->active_loan.amount * p->active_loan.interest_rate / 100.0);
            p->active_loan.accumulated_interest += interest;
            p->active_loan.duration_remaining--;

            /* Check for default */
            if (p->active_loan.duration_remaining <= 0) {
                /* Check if can repay */
                int total_owed = p->active_loan.amount + p->active_loan.accumulated_interest;
                if (p->cash < total_owed) {
                    process_loan_default(gs, i);
                }
            }
        }
    }
}

void purchase_insurance(GameState *gs, int player_idx, int prop_idx, InsuranceType type) {
    Player *p = &gs->players[player_idx];
    Property *prop = &gs->properties[prop_idx];
    int prop_value = prop->purchase_price;

    int premium = 0;
    char *type_name = "";
    switch (type) {
        case INSURANCE_BASIC:
            premium = (int)(prop_value * 0.05);
            type_name = "Basic Insurance";
            break;
        case INSURANCE_COMPREHENSIVE:
            premium = (int)(prop_value * 0.10);
            type_name = "Comprehensive Insurance";
            break;
        case INSURANCE_BUSINESS:
            premium = (int)(prop_value * 0.15);
            type_name = "Business Interruption Insurance";
            break;
        default: return;
    }

    /* Apply insurance regulation modifier */
    premium = (int)(premium * (1.0 + gs->insurance_premium_modifier));
    if (premium < 1) premium = 1;

    if (!can_afford(gs, player_idx, premium)) return;

    deduct_cash(gs, player_idx, premium);
    prop->insurance.type = type;
    prop->insurance.active = true;
    prop->insurance.rounds_remaining = INSURANCE_DURATION;
    prop->insurance.premium = premium;

    printf("%s purchased.\n", type_name);
    printf("Property : %s\n", prop->name);
    printf("Premium : LKR %d.\n", premium);
}

void process_insurance_expiry(GameState *gs) {
    for (int i = 0; i < 22; i++) {
        Property *prop = &gs->properties[i];
        if (prop->owner >= 0 && prop->insurance.active) {
            prop->insurance.rounds_remaining--;
            if (prop->insurance.rounds_remaining == 3) {
                printf("Insurance policy on %s expires in 3 rounds.\n", prop->name);
            }
            if (prop->insurance.rounds_remaining <= 0) {
                prop->insurance.active = false;
                prop->insurance.type = INSURANCE_NONE;
            }
        }
    }
}

void apply_inflation(GameState *gs, double rate) {
    printf("Inflation: %.0f%%\n", rate * 100);

    for (int i = 0; i < 22; i++) {
        Property *prop = &gs->properties[i];
        prop->purchase_price = (int)(prop->purchase_price * (1.0 + rate));
        prop->house_cost = (int)(prop->house_cost * (1.0 + rate));
        prop->hotel_cost = (int)(prop->hotel_cost * (1.0 + rate));
        prop->current_rent = (int)(prop->current_rent * (1.0 + rate));
        prop->mortgage_value = (int)(prop->mortgage_value * (1.0 + rate));
        if (prop->insurance.active) {
            prop->insurance.premium = (int)(prop->insurance.premium * (1.0 + rate));
        }
    }
    /* Interest rate for new loans is affected by inflation */
    gs->base_interest_rate = (int)(gs->base_interest_rate * (1.0 + rate));
    if (gs->base_interest_rate < 1) gs->base_interest_rate = 1;
}

void apply_property_depreciation(GameState *gs) {
    for (int i = 0; i < 22; i++) {
        Property *prop = &gs->properties[i];
        if (prop->owner < 0) continue;
        prop->age++;
        if (prop->age > 50 && !prop->renovated) {
            int new_dep = ((prop->age - 50) / 5) * 1;
            if (new_dep > 30) new_dep = 30;
            prop->depreciation_pct = new_dep;
            if (prop->depreciation_pct > 0 && prop->depreciation_pct % 5 == 0) {
                /* Print only at threshold changes */
                int old_val = prop->purchase_price;
                int new_val = (int)(prop->base_purchase_price * (1.0 - prop->depreciation_pct / 100.0));
                if (old_val > new_val + 100) {
                    printf("Property %s has depreciated by %d%%.\n",
                           prop->name, prop->depreciation_pct);
                    printf("Current Value LKR %d.\n",
                           (int)(prop->base_purchase_price * (1.0 - prop->depreciation_pct / 100.0)));
                }
            }
        }
    }
}

void apply_building_depreciation(GameState *gs) {
    for (int i = 0; i < 22; i++) {
        Property *prop = &gs->properties[i];
        if (prop->owner < 0) continue;
        if (prop->num_houses > 0 || prop->has_hotel) {
            prop->building_condition -= 2;
            if (prop->building_condition < 0) prop->building_condition = 0;
            prop->maintenance_skipped++;

            if (prop->maintenance_skipped > 20 && !prop->structural_damage) {
                prop->structural_damage = true;
                printf("Structural damage on %s!\n", prop->name);
            }
        }
    }
}

void perform_maintenance(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    bool maintained = false;

    for (int i = 0; i < p->prop_count; i++) {
        int pi = p->properties_owned[i];
        if (pi < 0 || pi >= 22) continue;
        Property *prop = &gs->properties[pi];

        if (prop->building_condition < 100 && (prop->num_houses > 0 || prop->has_hotel)) {
            int maint_cost;
            if (prop->has_hotel) {
                maint_cost = (int)(prop->base_hotel_cost * 0.08);
            } else {
                maint_cost = (int)(prop->base_house_cost * 0.05) * prop->num_houses;
            }
            /* Structural damage increases maintenance cost by 50% */
            if (prop->structural_damage) {
                maint_cost = maint_cost * 3 / 2;
            }

            if (can_afford(gs, player_idx, maint_cost)) {
                deduct_cash(gs, player_idx, maint_cost);
                prop->building_condition = 100;
                prop->maintenance_skipped = 0;
                if (!maintained) {
                    maintained = true;
                }
            }
        }
    }
}

void apply_building_condition(GameState *gs) {
    /* Building condition affects rent - handled in calculate_rent */
    (void)gs;
}

void process_bankruptcy(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    p->bankrupt = true;

    printf("%s has been declared bankrupt.\n", p->strategy_name);
    printf("Remaining assets transferred to the Bank.\n");

    /* Return all properties to bank */
    for (int i = 0; i < p->prop_count; i++) {
        int pi = p->properties_owned[i];
        if (pi >= 0 && pi < 22) {
            Property *prop = &gs->properties[pi];
            prop->owner = -1;
            prop->num_houses = 0;
            prop->has_hotel = false;
            prop->loan_locked = false;
            prop->mortgaged = false;
            prop->insurance.active = false;
            prop->insurance.type = INSURANCE_NONE;
            prop->building_condition = 100;
            prop->structural_damage = false;
            prop->maintenance_skipped = 0;
            prop->depreciation_pct = 0;
            prop->age = 0;
        }
    }
    /* Return railways */
    for (int i = 0; i < p->rail_count; i++) {
        int ri = p->railways_owned[i];
        if (ri >= 0 && ri < 4) {
            gs->railways[ri].owner = -1;
            gs->railways[ri].loan_locked = false;
            gs->railways[ri].mortgaged = false;
        }
    }
    /* Return utilities */
    for (int i = 0; i < p->util_count; i++) {
        int ui = p->utilities_owned[i];
        if (ui >= 0 && ui < 2) {
            gs->utilities[ui].owner = -1;
            gs->utilities[ui].loan_locked = false;
            gs->utilities[ui].mortgaged = false;
        }
    }
    /* Clear loan */
    p->active_loan.status = LOAN_NONE;
    p->active_loan.amount = 0;
    p->active_loan.accumulated_interest = 0;
    p->active_loan.collateral_count = 0;

    p->prop_count = 0;
    p->rail_count = 0;
    p->util_count = 0;
    p->cash = 0;
}

void check_bankruptcy(GameState *gs) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *p = &gs->players[i];
        if (p->bankrupt) continue;

        /* Check if player is bankrupt (cash < 0 and no assets to sell) */
        if (p->cash <= 0) {
            bool has_assets = false;
            if (p->prop_count > 0 || p->rail_count > 0 || p->util_count > 0)
                has_assets = true;

            /* Try to sell properties to cover debts */
            if (!has_assets && p->cash < 0) {
                process_bankruptcy(gs, i);
            }
        }

        /* Loan default already handled in accumulate_interest */
        /* Check if loan default left player with nothing */
        if (p->prop_count == 0 && p->rail_count == 0 && p->util_count == 0 &&
            p->cash <= 0 && p->active_loan.status == LOAN_DEFAULTED) {
            p->bankrupt = true;
            printf("%s has no remaining assets and is bankrupt.\n", p->strategy_name);
        }
    }
}

void renovate_property(GameState *gs, int player_idx, int prop_idx) {
    Property *prop = &gs->properties[prop_idx];
    Player *p = &gs->players[player_idx];
    int cost = (int)(prop->purchase_price * 0.10);
    if (!can_afford(gs, player_idx, cost)) return;

    deduct_cash(gs, player_idx, cost);
    prop->renovated = true;
    prop->depreciation_pct = 0;
    prop->age = 0;
    prop->current_rent = prop->base_rent;

    /* Adjust rent for current market conditions */
    if (prop->in_boom) prop->current_rent = (int)(prop->current_rent * 1.25);
    if (prop->in_decline) prop->current_rent = (int)(prop->current_rent * 0.80);

    printf("%s renovated %s at cost LKR %d.\n", p->strategy_name, prop->name, cost);
}

int calculate_rent(GameState *gs, int prop_idx, int dice_value) {
    (void)dice_value;
    Property *prop = &gs->properties[prop_idx];
    if (prop->mortgaged || prop->owner < 0) return 0;

    Player *owner = &gs->players[prop->owner];
    int rent = prop->base_rent;

    /* Apply development multiplier */
    if (prop->has_hotel) {
        rent = rent * 10;
    } else {
        switch (prop->num_houses) {
            case 0: break; /* 1x */
            case 1: rent = rent * 2; break;
            case 2: rent = rent * 3; break;
            case 3: rent = rent * 5; break;
            case 4: rent = rent * 7; break;
        }
    }

    /* Apply building condition modifier */
    int cond = prop->building_condition;
    if (prop->num_houses > 0 || prop->has_hotel) {
        if (cond >= 90) {
            /* 100% rent */
        } else if (cond >= 75) {
            rent = rent * 90 / 100;
        } else if (cond >= 50) {
            rent = rent * 75 / 100;
        } else if (cond >= 25) {
            rent = rent * 50 / 100;
        } else {
            return 0; /* Building closed */
        }
    }

    /* Apply market boom/decline */
    if (prop->in_boom) rent = (int)(rent * 1.25);
    if (prop->in_decline) rent = (int)(rent * 0.80);

    /* Apply economic recession rent decrease */
    if (gs->economic_recession) rent = (int)(rent * 0.90);

    /* Apply regional card modifiers */
    /* Southern Tourism Boom */
    if (gs->current_regional_card == REG_SOUTHERN_TOURISM) {
        if (prop_idx == 14 || prop_idx == 15 || prop_idx == 16) /* Galle Fort, Unawatuna, Hikkaduwa */
            rent = (int)(rent * 1.40);
    }
    /* Airport Expansion */
    if (gs->current_regional_card == REG_AIRPORT) {
        if (prop_idx == 8 || prop_idx == 9 || prop_idx == 10) /* Negombo, Katunayake, Ja-Ela */
            rent = (int)(rent * 1.30);
    }
    /* Beach Pollution */
    if (gs->current_regional_card == REG_BEACH_POLLUTION) {
        if (prop_idx >= 14 && prop_idx <= 16) /* Southern coastal */
            rent = (int)(rent * 0.70);
    }
    /* Water Shortage */
    if (gs->current_regional_card == REG_WATER_SHORTAGE) {
        if (prop_idx >= 0 && prop_idx <= 4) /* surrounding properties */
            rent = (int)(rent * 0.90);
    }

    /* Festival Season card effect */
    if (owner->modifiers.festival_season && prop->has_hotel) {
        rent = (int)(rent * 1.50);
    }

    /* Tourism Hype card effect */
    if (owner->modifiers.double_hotel_rent && prop->has_hotel) {
        rent = rent * 2;
    }

    /* Political Unrest - hotel rent drops 50% */
    if (gs->political_unrest && prop->has_hotel) {
        rent = rent / 2;
    }

    /* Tourism Boom economic event - hotels double rent */
    if (gs->tourism_boom && prop->has_hotel) {
        rent = rent * 2;
    }

    if (rent < 1) rent = 1;
    return rent;
}

int calculate_railway_rent(GameState *gs, int rail_idx) {
    (void)rail_idx;
    /* Count how many railways the owner has */
    Player *owner = NULL;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        for (int j = 0; j < gs->players[i].rail_count; j++) {
            if (gs->players[i].railways_owned[j] == rail_idx) {
                owner = &gs->players[i];
                break;
            }
        }
        if (owner) break;
    }
    if (!owner || gs->railways[rail_idx].mortgaged) return 0;

    int count = owner->rail_count;
    int rent = 0;
    switch (count) {
        case 1: rent = 250; break;
        case 2: rent = 500; break;
        case 3: rent = 1000; break;
        case 4: rent = 2000; break;
    }

    /* Apply modifiers */
    if (owner->modifiers.double_railway_rent) rent *= 2;
    if (gs->fuel_crisis) rent *= 2;
    if (gs->current_gov_regulation == GOV_RAILWAY_MODERN)
        rent = (int)(rent * 1.25);
    if (gs->current_regional_card == REG_TRANSPORT_STRIKE)
        rent = (int)(rent * 0.60);

    return rent;
}

int calculate_utility_rent(GameState *gs, int util_idx, int dice_value) {
    (void)util_idx;
    Player *owner = NULL;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        for (int j = 0; j < gs->players[i].util_count; j++) {
            if (gs->players[i].utilities_owned[j] == util_idx) {
                owner = &gs->players[i];
                break;
            }
        }
        if (owner) break;
    }
    if (!owner || gs->utilities[util_idx].mortgaged) return 0;

    int count = owner->util_count;
    int rent;
    if (count >= 2) {
        rent = 10 * dice_value;
    } else {
        rent = 4 * dice_value;
    }

    /* Apply modifiers */
    if (owner->modifiers.half_utility_income) rent /= 2;
    if (gs->current_gov_regulation == GOV_ELECTRICITY_REVISION)
        rent = (int)(rent * 1.20);
    if (gs->current_regional_card == REG_ELECTRICITY_TARIFF)
        rent = (int)(rent * 1.25);
    if (gs->current_regional_card == REG_WATER_SHORTAGE) {
        /* Water utility +20%, but this only applies to water utility */
        if (util_idx == 1) rent = (int)(rent * 1.20);
    }

    return rent;
}

int get_property_market_value(GameState *gs, int prop_idx) {
    if (prop_idx < 0 || prop_idx >= 22) return 0;
    Property *prop = &gs->properties[prop_idx];
    int value = prop->purchase_price;

    /* Adjust for depreciation */
    value = (int)(value * (1.0 - prop->depreciation_pct / 100.0));

    /* Adjust for boom/decline */
    if (prop->in_boom) value = (int)(value * 1.20);
    if (prop->in_decline) value = (int)(value * 0.85);

    /* Add building value */
    if (prop->has_hotel) {
        value += prop->hotel_cost;
    } else {
        value += prop->num_houses * prop->house_cost;
    }

    return value;
}

void auction_property(GameState *gs, int prop_idx) {
    Property *prop = &gs->properties[prop_idx];
    int market_value = get_property_market_value(gs, prop_idx);
    int opening_bid = market_value / 2;
    if (opening_bid < AUCTION_INCREMENT) opening_bid = AUCTION_INCREMENT;

    printf("Auction Started.\n");
    printf("Property : %s\n", prop->name);
    printf("Opening Bid : LKR %d.\n", opening_bid);

    bool active[MAX_PLAYERS] = { true, true, true, true };
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (gs->players[i].bankrupt) active[i] = false;
        if (gs->players[i].cash < opening_bid) active[i] = false;
    }

    int current_bid = opening_bid;
    int winner = -1;
    int active_count = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) if (active[i]) active_count++;

    if (active_count == 0) {
        printf("No players able to bid. Property remains with Bank.\n");
        return;
    }

    int rounds_without_bid = 0;
    while (active_count >= 2 || (active_count == 1 && rounds_without_bid == 0)) {
        for (int i = 0; i < MAX_PLAYERS && active_count >= 2; i++) {
            if (!active[i]) continue;
            if (gs->players[i].cash < current_bid + AUCTION_INCREMENT) {
                printf("%s withdraws.\n", gs->players[i].strategy_name);
                active[i] = false;
                active_count--;
                continue;
            }
            int bid = player_bid_amount(gs, i, current_bid, prop_idx);
            if (bid > current_bid) {
                current_bid = bid;
                winner = i;
                printf("%s bids LKR %d.\n", gs->players[i].strategy_name, bid);
                rounds_without_bid = 0;
            } else {
                printf("%s withdraws.\n", gs->players[i].strategy_name);
                active[i] = false;
                active_count--;
            }
        }
        rounds_without_bid++;
        if (rounds_without_bid > 2) break; /* Safety */
    }

    /* Last active player wins */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (active[i] && i != winner) {
            winner = i;
            break;
        }
    }

    if (winner >= 0 && current_bid <= gs->players[winner].cash) {
        deduct_cash(gs, winner, current_bid);
        prop->owner = winner;
        gs->players[winner].properties_owned[gs->players[winner].prop_count++] = prop_idx;
        printf("%s wins the auction.\n", gs->players[winner].strategy_name);
    } else {
        printf("No winner. Property remains with Bank.\n");
    }
}

int calculate_net_worth(GameState *gs, int player_idx) {
    Player *p = &gs->players[player_idx];
    if (p->bankrupt) return 0;

    int total = p->cash;

    /* Property values */
    for (int i = 0; i < p->prop_count; i++) {
        int pi = p->properties_owned[i];
        if (pi >= 0 && pi < 22) {
            total += get_property_market_value(gs, pi);
            if (gs->properties[pi].has_hotel)
                total += gs->properties[pi].hotel_cost;
            else
                total += gs->properties[pi].num_houses * gs->properties[pi].house_cost;
        }
    }

    /* Railways */
    for (int i = 0; i < p->rail_count; i++) {
        int ri = p->railways_owned[i];
        if (ri >= 0 && ri < 4) total += gs->railways[ri].purchase_price;
    }

    /* Utilities */
    for (int i = 0; i < p->util_count; i++) {
        int ui = p->utilities_owned[i];
        if (ui >= 0 && ui < 2) total += gs->utilities[ui].purchase_price;
    }

    /* Subtract outstanding loans */
    if (p->active_loan.status == LOAN_ACTIVE) {
        total -= p->active_loan.amount;
        total -= p->active_loan.accumulated_interest;
    }

    return total;
}
