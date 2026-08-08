#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/* Forward declarations */
extern int roll_dice(void);
extern void add_cash(GameState *gs, int player_idx, int amount);
extern int calculate_rent(GameState *gs, int prop_idx, int dice_value);
extern void apply_inflation(GameState *gs, double rate);

/* Economic event names */
static const char *econ_event_names[] = {
    "Tourism Boom",
    "Fuel Crisis",
    "Heavy Monsoon",
    "Economic Recession",
    "Stock Market Boom",
    "Government Housing Programme",
    "Foreign Investment",
    "Political Unrest"
};

/* Government regulation names */
static const char *gov_reg_names[] = {
    "Increase Property Tax",
    "Reduce Loan Interest",
    "Housing Subsidy",
    "Luxury Property Tax",
    "Railway Modernization",
    "Electricity Tariff Revision",
    "Insurance Regulation",
    "Anti-Speculation Act"
};

/* Regional card names */
static const char *regional_card_names[] = {
    "Southern Tourism Boom",
    "Port City Expansion",
    "IT Industry Growth",
    "Northern Development Programme",
    "Tea Export Boom",
    "Airport Expansion",
    "University City Growth",
    "Beach Pollution",
    "Flood Damage",
    "Transport Strike",
    "Electricity Tariff Increase",
    "Water Shortage"
};

static const char *disaster_names[] = {
    "Fire",
    "Flood",
    "Riot",
    "Building Collapse",
    "Electrical Failure"
};

void check_economic_events(GameState *gs) {
    gs->economic_event_countdown--;
    if (gs->economic_event_countdown > 0) return;
    gs->economic_event_countdown = 15;

    int evt = rand() % 8;
    gs->current_economic_event = (EconomicEventType)evt;
    gs->economic_event_rounds_remaining = 15;

    printf("Economic Event\n");
    printf("%s\n", econ_event_names[evt]);

    apply_economic_event(gs, gs->current_economic_event);
}

static void clear_economic_flags(GameState *gs) {
    gs->tourism_boom = false;
    gs->fuel_crisis = false;
    gs->heavy_monsoon = false;
    gs->economic_recession = false;
    gs->stock_market_boom = false;
    gs->housing_programme = false;
    gs->foreign_investment = false;
    gs->political_unrest = false;
}

void apply_economic_event(GameState *gs, EconomicEventType evt) {
    clear_economic_flags(gs);

    switch (evt) {
        case ECON_TOURISM_BOOM:
            gs->tourism_boom = true;
            printf("Hotels receive double rent.\n");
            printf("Southern coastal properties increase by 15%%.\n");
            /* Increase southern coastal properties */
            for (int i = 14; i <= 16; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.15);
                gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 1.15);
                gs->properties[i].mortgage_value = (int)(gs->properties[i].mortgage_value * 1.15);
            }
            break;
        case ECON_FUEL_CRISIS:
            gs->fuel_crisis = true;
            printf("Railway rent doubles.\n");
            printf("Property development costs increase 20%%.\n");
            gs->construction_cost_modifier += 0.20;
            for (int i = 0; i < 22; i++) {
                gs->properties[i].house_cost = (int)(gs->properties[i].house_cost * 1.20);
                gs->properties[i].hotel_cost = (int)(gs->properties[i].hotel_cost * 1.20);
            }
            break;
        case ECON_HEAVY_MONSOON:
            gs->heavy_monsoon = true;
            printf("Flood risk increases.\n");
            printf("Insurance premiums increase.\n");
            printf("Coastal properties lose 10%% value.\n");
            gs->insurance_premium_modifier += 0.15;
            for (int i = 0; i < 22; i++) {
                /* Coastal properties: Galle Fort, Unawatuna, Hikkaduwa, etc. */
                if (i >= 14 && i <= 16) { /* Yellow group (southern coastal) */
                    gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 0.90);
                    gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 0.90);
                }
            }
            break;
        case ECON_RECESSION:
            gs->economic_recession = true;
            printf("Property values decrease 15%%.\n");
            printf("Rent decreases 10%%.\n");
            printf("Loan interest increases by 15%%.\n");
            for (int i = 0; i < 22; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 0.85);
                gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 0.90);
            }
            gs->base_interest_rate = (int)(gs->base_interest_rate * 1.15);
            break;
        case ECON_STOCK_BOOM:
            gs->stock_market_boom = true;
            printf("Property values increase 10%%.\n");
            printf("Loan interest decreases by 10%%.\n");
            for (int i = 0; i < 22; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.10);
                gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 1.10);
            }
            gs->base_interest_rate = (int)(gs->base_interest_rate * 0.90);
            break;
        case ECON_HOUSING_PROGRAMME:
            gs->housing_programme = true;
            printf("House construction costs reduce 25%%.\n");
            gs->construction_cost_modifier -= 0.25;
            for (int i = 0; i < 22; i++) {
                gs->properties[i].house_cost = (int)(gs->properties[i].house_cost * 0.75);
            }
            break;
        case ECON_FOREIGN_INVESTMENT:
            gs->foreign_investment = true;
            printf("Commercial properties increase 20%%.\n");
            /* Dark blue are commercial */
            for (int i = 20; i <= 21; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.20);
                gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 1.20);
                gs->properties[i].mortgage_value = (int)(gs->properties[i].mortgage_value * 1.20);
            }
            break;
        case ECON_POLITICAL_UNREST:
            gs->political_unrest = true;
            printf("Riot probability doubles.\n");
            printf("Hotel occupancy decreases and the hotel rent drops by 50%%.\n");
            printf("Business interruption claims increases.\n");
            break;
    }
}

void check_gov_regulations(GameState *gs) {
    gs->gov_regulation_countdown--;
    if (gs->gov_regulation_countdown > 0) return;
    gs->gov_regulation_countdown = 20;

    /* Reset previous regulation effects */
    switch (gs->current_gov_regulation) {
        case GOV_INCREASE_TAX:
            gs->income_tax_amount = gs->base_income_tax;
            break;
        case GOV_REDUCE_INTEREST:
            gs->base_interest_rate -= 2;
            break;
        case GOV_HOUSING_SUBSIDY:
            gs->construction_cost_modifier -= (-0.30);
            break;
        case GOV_RAILWAY_MODERN:
            gs->railway_rent_modifier = 0;
            break;
        case GOV_ELECTRICITY_REVISION:
            gs->utility_rent_modifier = 0;
            break;
        case GOV_INSURANCE_REG:
            gs->insurance_premium_modifier -= (-0.15);
            break;
        case GOV_ANTI_SPECULATION:
            gs->anti_speculation_active = false;
            break;
        case GOV_LUXURY_TAX:
            gs->luxury_tax_active = false;
            break;
    }

    int reg = rand() % 8;
    gs->current_gov_regulation = (GovRegulationType)reg;
    gs->gov_regulation_rounds_remaining = 20;

    printf("Government Regulation\n");
    printf("%s Introduced.\n", gov_reg_names[reg]);

    apply_gov_regulation(gs, gs->current_gov_regulation);
}

void apply_gov_regulation(GameState *gs, GovRegulationType reg) {
    switch (reg) {
        case GOV_INCREASE_TAX:
            printf("Income Tax increases by 50%%.\n");
            gs->income_tax_amount = (int)(gs->income_tax_amount * 1.5);
            break;
        case GOV_REDUCE_INTEREST:
            printf("Interest decreases by 2%%.\n");
            gs->base_interest_rate -= 2;
            if (gs->base_interest_rate < 1) gs->base_interest_rate = 1;
            break;
        case GOV_HOUSING_SUBSIDY:
            printf("House construction costs reduced by 30%%.\n");
            gs->construction_cost_modifier -= 0.30;
            for (int i = 0; i < 22; i++) {
                gs->properties[i].house_cost = (int)(gs->properties[i].house_cost * 0.70);
            }
            break;
        case GOV_LUXURY_TAX:
            printf("Hotels incur an annual maintenance tax of 25%% of property value.\n");
            gs->luxury_tax_active = true;
            break;
        case GOV_RAILWAY_MODERN:
            printf("Railway rents increase 25%%.\n");
            gs->railway_rent_modifier = 0.25;
            break;
        case GOV_ELECTRICITY_REVISION:
            printf("Utility rents increase 20%%.\n");
            gs->utility_rent_modifier = 0.20;
            break;
        case GOV_INSURANCE_REG:
            printf("Insurance premiums decrease 15%%.\n");
            printf("Coverage remains unchanged.\n");
            gs->insurance_premium_modifier -= 0.15;
            break;
        case GOV_ANTI_SPECULATION:
            printf("Players may own at most three undeveloped properties.\n");
            printf("Additional purchases require immediate development within five rounds.\n");
            gs->anti_speculation_active = true;
            break;
    }
}

void check_regional_cards(GameState *gs) {
    gs->regional_card_countdown--;
    if (gs->regional_card_countdown > 0) return;
    gs->regional_card_countdown = 15;

    int card = rand() % 12;
    gs->current_regional_card = (RegionalCardType)card;
    gs->regional_card_rounds_remaining = 15;

    printf("Regional Development Card drawn:\n");
    printf("%s\n", regional_card_names[card]);

    apply_regional_card(gs, gs->current_regional_card);
}

void apply_regional_card(GameState *gs, RegionalCardType card) {
    switch (card) {
        case REG_SOUTHERN_TOURISM:
            printf("Galle Fort, Unawatuna and Hikkaduwa rental income +40%%\n");
            break;
        case REG_PORT_CITY:
            printf("Pettah, Maradana and Colombo Fort Station values +25%%\n");
            gs->properties[0].purchase_price = (int)(gs->properties[0].purchase_price * 1.25);
            gs->properties[1].purchase_price = (int)(gs->properties[1].purchase_price * 1.25);
            gs->properties[0].current_rent = (int)(gs->properties[0].current_rent * 1.25);
            gs->properties[1].current_rent = (int)(gs->properties[1].current_rent * 1.25);
            gs->railways[0].purchase_price = (int)(gs->railways[0].purchase_price * 1.25);
            break;
        case REG_IT_GROWTH:
            printf("Maharagama, Nugegoda and Kottawa values +20%%\n");
            for (int i = 5; i <= 7; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.20);
            }
            break;
        case REG_NORTHERN_DEV:
            printf("Jaffna Town, Nallur and Trincomalee values +30%%\n");
            for (int i = 17; i <= 19; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.30);
            }
            break;
        case REG_TEA_EXPORT:
            printf("Nuwara Eliya value +35%%\n");
            gs->properties[20].purchase_price = (int)(gs->properties[20].purchase_price * 1.35);
            break;
        case REG_AIRPORT:
            printf("Negombo, Katunayake and Ja-Ela rents +30%%\n");
            break;
        case REG_UNIVERSITY:
            printf("Peradeniya and Kandy City values +20%%\n");
            gs->properties[11].purchase_price = (int)(gs->properties[11].purchase_price * 1.20);
            gs->properties[12].purchase_price = (int)(gs->properties[12].purchase_price * 1.20);
            break;
        case REG_BEACH_POLLUTION:
            printf("Southern coastal rents -30%%\n");
            break;
        case REG_FLOOD_DAMAGE:
            printf("Low-lying coastal properties lose 20%% value\n");
            for (int i = 14; i <= 16; i++) {
                gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 0.80);
            }
            break;
        case REG_TRANSPORT_STRIKE:
            printf("Railway revenue reduced by 40%%\n");
            break;
        case REG_ELECTRICITY_TARIFF:
            printf("Utility rent +25%%\n");
            break;
        case REG_WATER_SHORTAGE:
            printf("Water utility revenue +20%%; surrounding properties -10%%\n");
            break;
    }
}

void check_disasters(GameState *gs) {
    gs->disaster_countdown--;
    if (gs->disaster_countdown > 0) return;
    gs->disaster_countdown = 10;

    /* Find a random developed property */
    int developed[28];
    int count = 0;
    for (int i = 0; i < 22; i++) {
        if (gs->properties[i].owner >= 0 &&
            (gs->properties[i].num_houses > 0 || gs->properties[i].has_hotel)) {
            developed[count++] = i;
        }
    }
    if (count == 0) return;

    int target = developed[rand() % count];
    Property *prop = &gs->properties[target];
    int disaster = rand() % 5;

    printf("%s occurred.\n", disaster_names[disaster]);
    printf("Affected Property : %s.\n", prop->name);

    /* Check insurance */
    if (prop->insurance.active) {
        int compensation = 0;
        switch (prop->insurance.type) {
            case INSURANCE_BASIC:
                /* 80% of repair cost */
                compensation = (int)(prop->house_cost * 0.80);
                break;
            case INSURANCE_COMPREHENSIVE:
                /* 100% of repair cost */
                compensation = prop->house_cost;
                if (prop->has_hotel) compensation = prop->hotel_cost;
                break;
            case INSURANCE_BUSINESS:
                /* Repair cost + 5 rounds lost rental income */
                compensation = (prop->has_hotel ? prop->hotel_cost : prop->house_cost);
                compensation += calculate_rent(gs, target, 0) * 5;
                break;
            default: break;
        }
        printf("Insurance Claim Approved.\n");
        printf("Compensation Paid : LKR %d.\n", compensation);
        add_cash(gs, prop->owner, compensation);
    } else {
        printf("No insurance - owner bears full repair cost.\n");
    }

    /* Damage the property */
    prop->damage_turns = 2; /* Property closed for repairs */
}

void check_market_review(GameState *gs) {
    gs->market_review_countdown--;
    if (gs->market_review_countdown > 0) return;
    gs->market_review_countdown = 10;

    review_property_market(gs);
}

void review_property_market(GameState *gs) {
    /* Select a random group for boom */
    int boom_group;
    do {
        boom_group = rand() % 8;
    } while (gs->current_round - gs->group_last_boom_round[boom_group] < 30);

    /* Select a random group for decline (must be different) */
    int decline_group;
    do {
        decline_group = rand() % 8;
    } while (decline_group == boom_group ||
             gs->current_round - gs->group_last_decline_round[decline_group] < 30);

    gs->group_last_boom_round[boom_group] = gs->current_round;
    gs->group_last_decline_round[decline_group] = gs->current_round;

    /* Apply boom to all properties in the group */
    for (int i = 0; i < 22; i++) {
        if (gs->properties[i].group == (PropertyGroup)boom_group) {
            gs->properties[i].in_boom = true;
            gs->properties[i].boom_rounds_remaining = 10;
            gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.15);
            gs->properties[i].mortgage_value = (int)(gs->properties[i].mortgage_value * 1.15);
            gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 1.25);
            gs->properties[i].house_cost = (int)(gs->properties[i].house_cost * 1.10);
            gs->properties[i].hotel_cost = (int)(gs->properties[i].hotel_cost * 1.10);
        }
        if (gs->properties[i].group == (PropertyGroup)decline_group) {
            gs->properties[i].in_decline = true;
            gs->properties[i].decline_rounds_remaining = 10;
            gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 0.85);
            gs->properties[i].current_rent = (int)(gs->properties[i].current_rent * 0.80);
            gs->properties[i].mortgage_value = (int)(gs->properties[i].mortgage_value * 0.90);
        }
    }

    const char *group_names[] = {"Brown", "Light Blue", "Pink", "Orange", "Red", "Yellow", "Green", "Dark Blue"};
    printf("Market Boom: %s properties (+15-25%%)\n", group_names[boom_group]);
    printf("Market Decline: %s properties (-10-20%%)\n", group_names[decline_group]);
}

void draw_national_card(GameState *gs, int player_idx) {
    NationalCard *card = &gs->national_cards[gs->card_deck_index];
    Player *p = &gs->players[player_idx];

    printf("National Event Card: %s\n", card->description);

    switch (card->effect_type) {
        case 0: /* Tourism Hype */
            p->modifiers.double_hotel_rent = true;
            p->modifiers.double_hotel_rent_rounds = 5;
            break;
        case 1: /* Fuel Shortage */
            p->modifiers.double_railway_rent = true;
            p->modifiers.double_railway_rent_rounds = 5;
            break;
        case 2: { /* Heavy Floods */
            /* Damage random coastal property if owned */
            int coastal[] = {14, 15, 16}; /* Galle Fort, Unawatuna, Hikkaduwa */
            int target = coastal[rand() % 3];
            if (gs->properties[target].owner == player_idx) {
                gs->properties[target].damage_turns = 2;
                printf("Flood damaged %s!\n", gs->properties[target].name);
            }
            break;
        }
        case 3: /* Political Rally */
            /* Close random property */
            for (int i = 0; i < p->prop_count; i++) {
                int pi = p->properties_owned[i];
                if (pi >= 0 && pi < 22) {
                    p->modifiers.property_closed_index = pi;
                    p->modifiers.property_closed_rounds = 2;
                    printf("Property %s closed for 2 rounds.\n", gs->properties[pi].name);
                    break;
                }
            }
            break;
        case 4: /* Stock Market Rise */
            for (int i = 0; i < 22; i++) {
                if (gs->properties[i].owner == player_idx) {
                    gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.10);
                }
            }
            break;
        case 5: /* Economic Downturn */
            for (int i = 0; i < 22; i++) {
                if (gs->properties[i].owner == player_idx) {
                    gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 0.85);
                }
            }
            break;
        case 6: /* Housing Subsidy */
            gs->construction_cost_modifier -= 0.30;
            break;
        case 7: /* Interest Rate Cut */
            gs->base_interest_rate -= 2;
            if (gs->base_interest_rate < 1) gs->base_interest_rate = 1;
            break;
        case 8: /* Interest Rate Increase */
            gs->base_interest_rate += 2;
            break;
        case 9: /* Tax Amnesty */
            printf("Each player receives LKR 2,000.\n");
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (!gs->players[i].bankrupt) gs->players[i].cash += 2000;
            }
            break;
        case 10: /* Power Failure */
            p->modifiers.half_utility_income = true;
            p->modifiers.half_utility_income_rounds = 3;
            break;
        case 11: /* Foreign Funding */
            for (int i = 20; i <= 21; i++) { /* Dark blue = commercial */
                if (gs->properties[i].owner == player_idx) {
                    gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.15);
                }
            }
            break;
        case 12: /* Port Expansion */
            for (int i = 0; i < 4; i++) {
                if (gs->railways[i].owner == player_idx) {
                    gs->railways[i].purchase_price = (int)(gs->railways[i].purchase_price * 1.20);
                }
            }
            break;
        case 13: /* Festival Season */
            p->modifiers.festival_season = true;
            p->modifiers.festival_rounds = 5;
            break;
        case 14: /* Labour Strike */
            p->modifiers.construction_suspended = true;
            p->modifiers.construction_suspended_rounds = 2;
            break;
        case 15: /* Insurance Discount */
            p->modifiers.insurance_discount = true;
            p->modifiers.insurance_discount_rounds = 5;
            break;
        case 16: { /* Property Revaluation */
            int grp = rand() % 8;
            for (int i = 0; i < 22; i++) {
                if (gs->properties[i].group == (PropertyGroup)grp) {
                    gs->properties[i].purchase_price = (int)(gs->properties[i].purchase_price * 1.15);
                }
            }
            break;
        }
        case 17: /* Currency Depreciation */
            gs->construction_cost_modifier += 0.10;
            for (int i = 0; i < 22; i++) {
                gs->properties[i].house_cost = (int)(gs->properties[i].house_cost * 1.10);
                gs->properties[i].hotel_cost = (int)(gs->properties[i].hotel_cost * 1.10);
            }
            break;
        case 18: /* Government Grant */
            printf("Random player receives LKR 5,000.\n");
            {
                int lucky = rand() % MAX_PLAYERS;
                if (!gs->players[lucky].bankrupt) {
                    gs->players[lucky].cash += 5000;
                    printf("%s received LKR 5,000!\n", gs->players[lucky].strategy_name);
                }
            }
            break;
        case 19: { /* National Disaster */
            /* Damage random developed property */
            int dev[28], dc = 0;
            for (int i = 0; i < 22; i++) {
                if (gs->properties[i].num_houses > 0 || gs->properties[i].has_hotel) {
                    dev[dc++] = i;
                }
            }
            if (dc > 0) {
                int t = dev[rand() % dc];
                gs->properties[t].damage_turns = 2;
                printf("National disaster damaged %s!\n", gs->properties[t].name);
            }
            break;
        }
    }

    /* Move card to bottom of deck */
    gs->card_deck_index = (gs->card_deck_index + 1) % MAX_CARDS;
}

void update_all_prices(GameState *gs) {
    /* Reset boom/decline counters */
    for (int i = 0; i < 22; i++) {
        if (gs->properties[i].in_boom) {
            gs->properties[i].boom_rounds_remaining--;
            if (gs->properties[i].boom_rounds_remaining <= 0) {
                gs->properties[i].in_boom = false;
            }
        }
        if (gs->properties[i].in_decline) {
            gs->properties[i].decline_rounds_remaining--;
            if (gs->properties[i].decline_rounds_remaining <= 0) {
                gs->properties[i].in_decline = false;
            }
        }
    }

    /* Update player modifiers */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Player *p = &gs->players[i];
        if (p->modifiers.double_hotel_rent_rounds > 0) {
            p->modifiers.double_hotel_rent_rounds--;
            if (p->modifiers.double_hotel_rent_rounds <= 0)
                p->modifiers.double_hotel_rent = false;
        }
        if (p->modifiers.double_railway_rent_rounds > 0) {
            p->modifiers.double_railway_rent_rounds--;
            if (p->modifiers.double_railway_rent_rounds <= 0)
                p->modifiers.double_railway_rent = false;
        }
        if (p->modifiers.half_utility_income_rounds > 0) {
            p->modifiers.half_utility_income_rounds--;
            if (p->modifiers.half_utility_income_rounds <= 0)
                p->modifiers.half_utility_income = false;
        }
        if (p->modifiers.construction_suspended_rounds > 0) {
            p->modifiers.construction_suspended_rounds--;
            if (p->modifiers.construction_suspended_rounds <= 0)
                p->modifiers.construction_suspended = false;
        }
        if (p->modifiers.property_closed_rounds > 0) {
            p->modifiers.property_closed_rounds--;
            if (p->modifiers.property_closed_rounds <= 0)
                p->modifiers.property_closed_index = -1;
        }
        if (p->modifiers.festival_rounds > 0) {
            p->modifiers.festival_rounds--;
            if (p->modifiers.festival_rounds <= 0)
                p->modifiers.festival_season = false;
        }
    }

    /* Update economic event timer */
    if (gs->economic_event_rounds_remaining > 0) {
        gs->economic_event_rounds_remaining--;
        if (gs->economic_event_rounds_remaining <= 0) {
            clear_economic_flags(gs);
        }
    }

    /* Update gov regulation timer */
    if (gs->gov_regulation_rounds_remaining > 0) {
        gs->gov_regulation_rounds_remaining--;
    }

    /* Update regional card timer */
    if (gs->regional_card_rounds_remaining > 0) {
        gs->regional_card_rounds_remaining--;
    }

    /* Update inflation */
    if (gs->inflation_rounds_remaining > 0) {
        gs->inflation_rounds_remaining--;

        /* Apply inflation every 10 rounds */
        gs->inflation_countdown--;
        if (gs->inflation_countdown <= 0) {
            gs->inflation_countdown = 10;
            double rates[] = { -0.03, 0.0, 0.02, 0.05, 0.08, 0.12 };
            int r = rand() % 6;
            gs->inflation_rate = rates[r];
            apply_inflation(gs, gs->inflation_rate);
        }
    }

    /* Repair damaged properties */
    for (int i = 0; i < 22; i++) {
        if (gs->properties[i].damage_turns > 0) {
            gs->properties[i].damage_turns--;
        }
    }
}
