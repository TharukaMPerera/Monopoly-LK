#include "structures.h"

National_event_cards national_deck[20] = {
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
    NATIONAL_DISASTER};

int deck_top = 0;
int card_counters[20] = {0};

int active_economic_event = -1;
int active_regulation = -1;
int boom_group = -1;
int decline_group = -1;

void national_event_function(player players[], property board[], int drawer)
{
    National_event_cards card = national_deck[deck_top];
    printf("\n%s draws a National Event Card.\n", players[drawer].name);

    if (card == TOURISM_HYPE)
    {

        card_counters[TOURISM_HYPE] = 5;
        printf("Card : Tourism Hype. Hotels earn double rent for 5 rounds.\n");
    }
    else if (card == POLITICAL_RALLY)
    {

        card_counters[POLITICAL_RALLY] = 2;
        int closed_property = (rand() % 40);
        printf("Card : Political Rally. %s is closed for 2 rounds.\n", board[closed_property].name);
    }
    else if (card == STOCK_MARKET_RISE)
    {
        for (int i = 0; i < 40; i++)
        {
            if ((board[i].type == land) || (board[i].type == railway) || (board[i].type == utility))
            {
                board[i].purchase_price = (int)(board[i].purchase_price * 1.10);
                board[i].mortgage_value = (int)(board[i].mortgage_value * 1.10);
            }
        }
        printf("Card : Stock Market Rise. All property values increased by 10%%.\n");
    }
    else if (card == ECONOMIC_DOWNTURN)
    {

        for (int i = 0; i < 40; i++)
        {
            if ((board[i].type == land) || (board[i].type == railway) || (board[i].type == utility))
            {
                board[i].purchase_price = (int)(board[i].purchase_price * 0.85);
                board[i].mortgage_value = (int)(board[i].mortgage_value * 0.85);
            }
        }
        printf("Card : Economic Downturn. All property values decreased by 15%%.\n");
    }
    else if (card == HOUSING_SUBSIDY)
    {

        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].house_cost = (int)(board[i].house_cost * 0.70);
                board[i].hotel_cost = (int)(board[i].hotel_cost * 0.70);
            }
        }
        printf("Card : Housing Subsidy. House construction costs reduced by 30%%.\n");
    }
    else if (card == TAX_AMNESTY)
    {
        for (int i = 1; i < 5; i++)
        {
            if (players[i].bankrupt != active)
            {
                players[i].cash += 2000;
                printf("%s received LKR 2,000 from Tax Amnesty (Balance : LKR %d).\n",
                       players[i].name, players[i].cash);
            }
        }
    }
    else if (card == FESTIVAL_SEASON)
    {
        card_counters[FESTIVAL_SEASON] = 15;
        printf("Card : Festival Season. Hotels receive 50%% additional rent.\n");
    }
    else if (card == CURRENCY_DEPRECIATION)
    {
        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].house_cost = (int)(board[i].house_cost * 1.10);
                board[i].hotel_cost = (int)(board[i].hotel_cost * 1.10);
            }
        }
        printf("Card : Currency Depreciation. Construction costs increased by 10%%.\n");
    }
    else if (card == GOVERNMENT_GRANT)
    {
        int grant_player;
        do
        {
            grant_player = (rand() % 4) + 1;
        } while (players[grant_player].bankrupt == active);
        players[grant_player].cash += 5000;
        printf("Card : Government Grant. %s received LKR 5,000 (Balance : LKR %d).\n",
               players[grant_player].name, players[grant_player].cash);
    }
    // other events has to added

    National_event_cards drawn_card = card;
    for (int i = deck_top; i < 19; i++)
    {
        national_deck[i] = national_deck[i + 1];
    }
    national_deck[19] = drawn_card;
}

void update_national_event_counters(void)
{
    for (int i = 0; i < 20; i++)
    {
        if (card_counters[i] > 0)
        {
            card_counters[i]--;
        }
    }
}

void disasters(player players[], property board[])
{

    int developed[40];
    int developed_count = 0;
    for (int i = 0; i < 40; i++)
    {
        if (board[i].type != land)
        {
            continue;
        }
        if (board[i].current_owner == BANKER)
        {
            continue;
        }
        if (players[board[i].current_owner].bankrupt == active)
        {
            continue;
        }
        if ((board[i].no_of_houses > 0) || (board[i].no_of_hotels == 1))
        {
            developed[developed_count++] = i;
        }
    }

    if (developed_count == 0)
    {
        printf("Disaster check : no developed properties exist, no disaster strikes.\n");
        return;
    }

    Disasters disaster_name = (Disasters)(rand() % 5);
    int place = developed[rand() % developed_count];
    Player_id owner = board[place].current_owner;

    char *disaster_names[5] = {"Fire", "Flood", "Riot", "Building Collapse", "Electrical Failure"};
    printf("\n%s occurred.\nAffected Property : %s.\n", disaster_names[disaster_name], board[place].name);

    players[owner].buildings[place].condition = 0.0;

    int building_investment = (board[place].no_of_houses * board[place].house_cost) + (board[place].no_of_hotels * board[place].hotel_cost);
    int repair_cost = (int)(building_investment * 0.50);

    int covered = 0;
    double coverage = 0.0;
    int policy = NO_INSURANCE;
    if (players[owner].policies[place].active == active)
    {
        policy = players[owner].policies[place].type;
        if ((policy == BASIC_PROPERTY_INSURANCE) && ((disaster_name == FIRE) || (disaster_name == FLOOD)))
        {
            covered = 1;
            coverage = 0.80;
        }
        else if ((policy == COMPREHENSIVE_INSURANCE) || (policy == BUSINESS_INTERRUPTION_INSURANCE))
        {
            covered = 1;
            coverage = 1.00;
        }
    }

    if (covered)
    {
        int compensation = (int)(repair_cost * coverage);
        players[owner].cash += compensation;
        printf("Insurance Claim Approved.\nCompensation Paid : LKR %d.\n", compensation);

        
        if ((policy == BUSINESS_INTERRUPTION_INSURANCE) && (board[place].no_of_hotels == 1))
        {
            int lost_income = board[place].base_rental * 10 * 5;
            players[owner].cash += lost_income;
            printf("Business Interruption : lost hotel rental income compensated with LKR %d.\n", lost_income);
        }
    }
    else
    {
        printf("No valid insurance policy on this property. The owner bears the full repair cost.\n");
    }

    if (players[owner].cash >= repair_cost)
    {
        players[owner].cash -= repair_cost;
        players[owner].buildings[place].condition = 1.0;
        printf("%s paid the repair cost of LKR %d. Building repaired.\n", players[owner].name, repair_cost);
    }
    else
    {
        printf("%s cannot afford the repair cost of LKR %d. Building stays closed.\n",
               players[owner].name, repair_cost);
    }
}

void update_insurance_policies(player players[], property board[])
{
    
    for (int x = 1; x < 5; x++)
    {
        for (int i = 0; i < 40; i++)
        {
            if (players[x].policies[i].active != active)
            {
                continue;
            }

            players[x].policies[i].rounds_remaining--;

            if (players[x].policies[i].rounds_remaining <= 0)
            {
                printf("Insurance policy on %s has expired.\n", board[i].name);
                players[x].policies[i].active = inactive;
                players[x].policies[i].type = NO_INSURANCE;
                board[i].insurance_status = inactive;
            }
            else if (players[x].policies[i].rounds_remaining <= 3)
            {
                printf("Insurance policy on %s expires in %d rounds.\n",
                       board[i].name, players[x].policies[i].rounds_remaining);
            }
        }
    }


    for (int x = 1; x < 5; x++)
    {
        for (int i = 0; i < 40; i++)
        {
            if ((board[i].type != land) || (board[i].current_owner != x))
            {
                continue;
            }
            if (players[x].buildings[i].type == no_buildings)
            {
                continue;
            }
            if (players[x].buildings[i].condition > 0.0)
            {
                continue;
            }

            int building_investment = (board[i].no_of_houses * board[i].house_cost) + (board[i].no_of_hotels * board[i].hotel_cost);
            int repair_cost = (int)(building_investment * 0.50);
            if (players[x].cash >= repair_cost)
            {
                players[x].cash -= repair_cost;
                players[x].buildings[i].condition = 1.0;
                printf("%s repaired %s for LKR %d. Building reopened.\n", players[x].name, board[i].name, repair_cost);
            }
        }
    }
}

//under constructoin
void special_disaster(void)    
{
    
    int type = rand() % 6;
    if (type == 0)
    {
        printf("\nTsunami warning issued - coastal properties at risk\n");
    }
    else if (type == 1)
    {
        printf("\nEarthquake tremors felt near high-rise developments\n");
    }
    else if (type == 2)
    {
        printf("\nFire spread risk detected near developed zonesn");
    }
    else if (type == 3)
    {
        printf("\nCascading riot escalation reported\n");
    }
    else if (type == 4)
    {
        printf("\nProperty lockdown ordered for inspection\n");
    }
    else
    {
        printf("\nInsurance audit triggered on recent claims\n");
    }
}


void economic_events(Economic_events event_name, property board[])
{
    
    if (active_economic_event == ECONOMIC_RECESSION)
    {
        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].purchase_price = (int)(board[i].purchase_price * (100.0 / 85.0));
                board[i].base_rental = (int)(board[i].base_rental * (100.0 / 90.0));
            }
        }
    }
    else if (active_economic_event == GOVERNMENT_HOUSING_PROGRAM)
    {
        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].house_cost = (int)(board[i].house_cost * (100.0 / 75.0));
            }
        }
    }

    if (event_name == ECONOMIC_RECESSION)
    {
      
        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].purchase_price = (int)(board[i].purchase_price * 0.85);
                board[i].base_rental = (int)(board[i].base_rental * 0.90);
            }
        }
        printf("\nEconomic Recession - property values down 15%%, rent down 10%%.\n");
        
        
        active_economic_event = ECONOMIC_RECESSION;
    }
    else if (event_name == GOVERNMENT_HOUSING_PROGRAM)
    {
     
        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].house_cost = (int)(board[i].house_cost * 0.75);
            }
        }
        printf("\nGovernment Housing Programme - house construction costs reduced by 25%%.\n");
        active_economic_event = GOVERNMENT_HOUSING_PROGRAM;
    }
    else
    {
        //have to make others 
        active_economic_event = -1;
    }
}


void government_regulation(Government_regulations regulation_name, property board[], double *income_tax_rate)
{
    if (active_regulation == INCREASE_PROPERTY_TAX)
    {
        *income_tax_rate = 0.15;
    }
    else if (active_regulation == HOUSING_SUBSIDY_REGULATION)
    {
        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].house_cost = (int)(board[i].house_cost * (100.0 / 70.0));
            }
        }
    }

    if (regulation_name == INCREASE_PROPERTY_TAX)
    {
        *income_tax_rate *= 1.50;
        printf("\nGovernment Regulation : Increase Property Tax - Income Tax rate increased by 50%%.\n");
        active_regulation = INCREASE_PROPERTY_TAX;
    }
    else if (regulation_name == HOUSING_SUBSIDY_REGULATION)
    {

        for (int i = 0; i < 40; i++)
        {
            if (board[i].type == land)
            {
                board[i].house_cost = (int)(board[i].house_cost * 0.70);
            }
        }
        printf("\nGovernment Regulation : Housing Subsidy - house construction costs reduced by 30%%.\n");
        active_regulation = HOUSING_SUBSIDY_REGULATION;
    }
    else
    {
        //have to make ithers
        active_regulation = -1;
    }
}


void revert_boom_effects(property board[], int group)
{
    for (int i = 0; i < 40; i++)
    {
        if (board[i].color == group)
        {
            board[i].purchase_price = (int)(board[i].purchase_price * (100.0 / 120.0));
            board[i].mortgage_value = (int)(board[i].mortgage_value * (100.0 / 115.0));
            board[i].base_rental = (int)(board[i].base_rental * (100.0 / 125.0));
            board[i].house_cost = (int)(board[i].house_cost * (100.0 / 110.0));
            board[i].hotel_cost = (int)(board[i].hotel_cost * (100.0 / 110.0));
        }
    }
}

void revert_decline_effects(property board[], int group)
{
    for (int i = 0; i < 40; i++)
    {
        if (board[i].color == group)
        {
            board[i].purchase_price = (int)(board[i].purchase_price * (100.0 / 85.0));
            board[i].base_rental = (int)(board[i].base_rental * (100.0 / 80.0));
            board[i].mortgage_value = (int)(board[i].mortgage_value * (100.0 / 90.0));
        }
    }
}

void apply_boom_effects(property board[], int group)
{
    for (int i = 0; i < 40; i++)
    {
        if (board[i].color == group)
        {
            board[i].purchase_price = (int)(board[i].purchase_price * 1.20);
            board[i].mortgage_value = (int)(board[i].mortgage_value * 1.15);
            board[i].base_rental = (int)(board[i].base_rental * 1.25);
            board[i].house_cost = (int)(board[i].house_cost * 1.10);
            board[i].hotel_cost = (int)(board[i].hotel_cost * 1.10);
        }
    }
}

void apply_decline_effects(property board[], int group)
{
    for (int i = 0; i < 40; i++)
    {
        if (board[i].color == group)
        {
            board[i].purchase_price = (int)(board[i].purchase_price * 0.85);
            board[i].base_rental = (int)(board[i].base_rental * 0.80);
            board[i].mortgage_value = (int)(board[i].mortgage_value * 0.90);
        }
    }
}


// overwrite the effects instead of stacking them.
void dynamic_property_market(property board[], int *market_decline_group)
{
    // revert the previous boom/decline effects
    if (boom_group >= brown)
    {
        revert_boom_effects(board, boom_group);
    }
    if (decline_group >= brown)
    {
        revert_decline_effects(board, decline_group);
    }


    int new_boom;
    do
    {
        new_boom = brown + (rand() % 8);
    } while (new_boom == boom_group);

    int new_decline;
    do
    {
        new_decline = brown + (rand() % 8);
    } while ((new_decline == decline_group) || (new_decline == new_boom));

    boom_group = new_boom;
    decline_group = new_decline;
    *market_decline_group = new_decline;

    apply_boom_effects(board, boom_group);
    apply_decline_effects(board, decline_group);

    printf("\nDynamic Property Market : boom on the selected group (values +20%%, rent +25%%, mortgage +15%%, construction +10%%).\n");
    printf("Dynamic Property Market : decline on another group (values -15%%, rent -20%%, mortgage -10%%, auction start -25%%).\n");
}
