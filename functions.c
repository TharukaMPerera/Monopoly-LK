
#include "structures.h"

void game_start_print()
{
    printf("________Welcome to Monopoly LK!________\n");
    printf("____________The Players are____________\n");
    printf("     Player 1: AGGRESIVE INVESTOR\n");
    printf("     Player 2: CONSERVATIVE BANKER\n");
    printf("     Player 3: RISK TAKER\n");
    printf("     Player 4: OPPORTUNISTIC TRADER\n");
}

int roll_dice()
{
    int dice;
    dice = ((rand() % 6) + 1);

    return dice;
}

int double_roll(int dice_01, int dice_02)
{
    if (dice_01 == dice_02)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// function player order is under construction
void player_order(player players[])
{

    activity swapped = active;

    for (int i = 1; i < 5; i++)
    {
        if (players[i].is_order_assigned == inactive)
        {
            players[i].dice_value = roll_dice() + roll_dice();
            printf("%s rolls %i.\n", players[i].name, players[i].dice_value);
        }
    }

    while (swapped)
    {
        swapped = inactive;
        for (int i = 1; i < 4; i++)
        {
            player temp;
            if ((players[i].is_order_assigned == inactive) && (players[i].dice_value < players[i + 1].dice_value))
            {
                temp = players[i];
                players[i] = players[i + 1];
                players[i + 1] = temp;

                swapped = active;
            }
        }
    }

    for (int j = 1; j < 5; j++)
    {
        printf("-----------------------------------------\n");
        printf("Player %d: %s\n", j, players[j].name);
    }
}

void print_stats(player players[], int *round_counter)
{
    printf("\n##########################################\n");
    printf("ROUND : %d\n", *round_counter);
    printf("Player Name_____Available Cash____Properties\n");
    for (int i = 1; i < 5; i++)
    {
        printf("%-23s  %6d LKR  %2d\n", players[i].name, players[i].cash, players[i].num_properties);
    }
    printf("\n");
}

// the buy of opotunistic trader has to be fixed
void buy_function(int owned_properties[], player players[], property board[])
{
    for (int x = 1; x < 5; x++)
    {
        int place = players[x].location;
        if ((board[place].type != land) && (board[place].type != utility) && (board[place].type != railway))
        {
            continue;
        }
        if (board[place].current_owner != BANKER)
        {
            continue;
        }
        if (players[x].bankrupt == active)
        {
            continue;
        }

        activity buy = inactive;

        switch (players[x].player_id)
        {
        case AGGRESSIVE_INVESTOR:
        {
            if (players[x].cash >= board[place].purchase_price)
            {
                buy = active;
            }
            break;
        }
        case CONSERVATIVE_BANKER:
        {
            int reserve_cash = players[x].cash - board[place].purchase_price;

            if (reserve_cash >= (players[x].cash * 0.5))
            {
                buy = active;
            }
            break;
        }
        case RISK_TAKER:
        {
            if (players[x].cash >= board[place].purchase_price)
            {
                buy = active;
            }
            break;
        }
        case OPPORTUNISTIC_TRADER:
        {

            int projected_appreciation = board[place].original_price - board[place].purchase_price;
            if ((players[x].cash >= board[place].purchase_price) && (projected_appreciation > board[place].house_cost))
            {
                buy = active;
            }
            break;
        }
        default:
        {
            break;
        }
        }

        if (buy)
        {
            players[x].cash -= board[place].purchase_price;
            owned_properties[place] = x;
            board[place].current_owner = x;
            players[x].num_properties++;
            board[place].mortgage_value = board[place].purchase_price * 0.75;
            players[x].total_mortgage_value += board[place].mortgage_value;
            printf("\n%s has purchased %s for $%d.\n", players[x].name, board[place].name, board[place].purchase_price);
        }
        else
        {
            printf("\n%s declined to purchase %s.\n", players[x].name, board[place].name);
        }
    }
}

void insurance_function(player players[], property board[])
{
    for (int x = 1; x < 5; x++)
    {
        if (players[x].bankrupt == active)
        {
            continue;
        }
        int place = players[x].location;
        if ((place != 17) && (place != 33))
        {
            continue;
        }

        for (int i = 0; i < 40; i++)
        {
            if ((board[i].type != land) || (board[i].current_owner != players[x].player_id))
            {
                continue;
            }
            if (players[x].policies[i].active == active)
            {
                continue;
            }

            activity has_building = inactive;

            if ((board[i].no_of_houses > 0) || (board[i].no_of_hotels == 1))
            {
                has_building = active;
            }
            policy_type wanted = NO_INSURANCE;

            switch (players[x].player_id)
            {
            case AGGRESSIVE_INVESTOR:
            {
                if (board[i].no_of_houses > 0)
                {
                    wanted = BASIC_PROPERTY_INSURANCE;
                }
                else if (board[i].no_of_hotels == 1)
                {
                    wanted = COMPREHENSIVE_INSURANCE;
                }
                break;
            }
            case CONSERVATIVE_BANKER:
            {
                if (has_building)
                {
                    wanted = COMPREHENSIVE_INSURANCE;
                }
                break;
            }
            case RISK_TAKER:
            {
                if (board[i].no_of_houses > 0)
                {
                    wanted = BASIC_PROPERTY_INSURANCE;
                }
                else if (board[i].no_of_hotels == 1)
                {
                    wanted = COMPREHENSIVE_INSURANCE;
                }
                break;
            }
            case OPPORTUNISTIC_TRADER:
            {
                if (has_building && (board[i].purchase_price >= 6000))
                {
                    wanted = COMPREHENSIVE_INSURANCE;
                }
                break;
            }
            default:
            {
                break;
            }
            }

            if (wanted == NO_INSURANCE)
            {
                continue;
            }

            int premium = 0;
            if (wanted == BASIC_PROPERTY_INSURANCE)
            {
                premium = (int)(board[i].purchase_price * 0.05);
            }
            else if (wanted == COMPREHENSIVE_INSURANCE)
            {
                premium = (int)(board[i].purchase_price * 0.10);
            }
            else
            {
                premium = (int)(board[i].purchase_price * 0.15);
            }

            if (players[x].cash < premium)
            {
                continue;
            }

            players[x].cash -= premium;
            players[x].policies[i].active = active;
            players[x].policies[i].type = wanted;
            players[x].policies[i].rounds_remaining = 20;
            players[x].policies[i].premium = premium;
            players[x].policies[i].compensation = 0.0;
            board[i].insurance_status = active;

            char *policy_name = "Basic Property Insurance";
            if (wanted == COMPREHENSIVE_INSURANCE)
            {
                policy_name = "Comprehensive Insurance";
            }
            else if (wanted == BUSINESS_INTERRUPTION_INSURANCE)
            {
                policy_name = "Business Interruption Insurance";
            }

            printf("\n%s purchased.\nProperty : %s\nPremium : LKR %d.\n", policy_name, board[i].name, premium);
        }
    }
}

void rent_function(player players[], property board[])
{
    for (int x = 1; x < 5; x++)
    {
        int place = players[x].location;
        int rent_payable = 0;

        if (players[x].bankrupt == active)
        {
            continue;
        }
        if ((board[place].current_owner == BANKER) || (board[place].current_owner == x))
        {
            continue;
        }
        if (board[place].mortgage_status == active)
        {
            continue;
        }
        Player_id original_owner = board[place].current_owner;

        if ((board[place].type == land) || (board[place].type == utility) || (board[place].type == railway))
        {
            switch (board[place].type)
            {
            case land:
            {
                if (board[place].no_of_houses > 0)
                {
                    switch (board[place].no_of_houses)
                    {
                    case 1:
                    {
                        rent_payable = (board[place].base_rental * 2);
                        break;
                    }
                    case 2:
                    {
                        rent_payable = (board[place].base_rental * 3);
                        break;
                    }
                    case 3:
                    {
                        rent_payable = (board[place].base_rental * 5);
                        break;
                    }
                    case 4:
                    {
                        rent_payable = (board[place].base_rental * 7);
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                else if (board[place].no_of_hotels == 1)
                {
                    rent_payable = (board[place].base_rental * 10);
                }
                else
                {
                    rent_payable = board[place].base_rental;
                }

                double condition = 1.0;
                if ((board[place].no_of_houses > 0) || (board[place].no_of_hotels > 0))
                {
                    condition = players[original_owner].buildings[place].condition;
                }
                if (condition < 0.25)
                {
                    rent_payable = 0;
                    printf("%s's building on %s is closed, condition %.0f%%. No rent collected.\n",players[original_owner].name, board[place].name, condition * 100);
                }
                else if (condition < 0.50)
                {
                    rent_payable = (int)(rent_payable * 0.50);
                }
                else if (condition < 0.75)
                {
                    rent_payable = (int)(rent_payable * 0.75);
                }
                else if (condition < 0.90)
                {
                    rent_payable = (int)(rent_payable * 0.90);
                }
                break;
            }
            case railway:
            {
                int stations_owned = 0;
                for (int i = 0; i < 40; i++)
                {
                    if ((board[i].type == railway) && (board[i].current_owner == original_owner))
                    {
                        stations_owned++;
                    }
                }
                switch (stations_owned)
                {
                case 1:
                {
                    rent_payable = 250;
                    break;
                }
                case 2:
                {
                    rent_payable = 500;
                    break;
                }
                case 3:
                {
                    rent_payable = 1000;
                    break;
                }
                case 4:
                {
                    rent_payable = 2000;
                    break;
                }
                default:
                {
                    break;
                }
                }
                break;
            }
            case utility:
            {
                int utilities_owned = 0;
                for (int i = 0; i < 40; i++)
                {
                    if ((board[i].type == utility) && (board[i].current_owner == original_owner))
                    {
                        utilities_owned++;
                    }
                }
                if (utilities_owned == 1)
                {
                    rent_payable = 4 * players[x].last_roll;
                }
                else if (utilities_owned >= 2)
                {
                    rent_payable = 10 * players[x].last_roll;
                }
                break;
            }
            default:
            {
                break;
            }
            }
        }

        if (rent_payable > 0)
        {
            players[x].cash -= rent_payable;
            players[original_owner].cash += rent_payable;
            printf("%s payed rent of Rs.%d to %s\n", players[x].name, rent_payable, players[original_owner].name);
        }
    }
}

void loan_function(int player_id, int player_request, player players[], property board[])
{
    if (players[player_id].total_mortgage_value > 0)
    {
        if (players[player_id].loans_owned.active == inactive)
        {
           
            int max_loan = (int)(players[player_id].total_mortgage_value * 0.75);
            if (player_request <= max_loan)
            {
                players[player_id].cash += player_request;
                players[player_id].total_mortgage_value -= player_request;
                printf("%s has taken a loan of $%d.\n", players[player_id].name, player_request);
                for (int i = 0; i < 40; i++)
                {
                    if (board[i].current_owner == player_id)
                    {
                        board[i].mortgage_status = 1;
                    }
                }
                players[player_id].loans_owned.amount += player_request;
                players[player_id].loans_owned.loan_duration = 20;
                players[player_id].loans_owned.interest_rate = 0.08;
                players[player_id].loans_owned.active = active;
            }
            else
            {
                printf("%s cannot take a loan of $%d as it exceeds the maximum loan of $%d (75%% of mortgage value).\n", players[player_id].name, player_request, max_loan);
            }
        }
    }

    else
    {
        printf("%s has no properties to act as collateral.\n", players[player_id].name);
    }
}

void inflation(property board[], double *current_inflation_rate)
{
    double inflation_rate;
    int inflation_selecter = (rand() % 6 + 1);
    switch (inflation_selecter)
    {
    case 1:
        inflation_rate = (-3.0 / 100.0);
        printf("\n The inflation rate increased by -3 percent\n");
        break;
    case 2:
        inflation_rate = (0.0 / 100.0);
        printf("\n The inflation rate increased by 0 percent\n");
        break;
    case 3:
        inflation_rate = (2.0 / 100.0);
        printf("\n The inflation rate increased by 2 percent\n");
        break;
    case 4:
        inflation_rate = (5.0 / 100.0);
        printf("\n The inflation rate increased by 5 percent \n");
        break;
    case 5:
        inflation_rate = (8.0 / 100.0);
        printf("\n The inflation rate increased by 8 percent\n");
        break;
    case 6:
        inflation_rate = (12.0 / 100.0);
        printf("\n The inflation rate increased by 12 percent\n");
        break;
    default:
        break;
    }
    *current_inflation_rate = inflation_rate;
    for (int i = 0; i < 40; i++)
    {
        if (board[i].type == land)
        {
            board[i].purchase_price *= (1 + inflation_rate);
            board[i].mortgage_value *= (1 + inflation_rate);
            board[i].base_rental *= (1 + inflation_rate);
            board[i].house_cost *= (1 + inflation_rate);
            board[i].hotel_cost *= (1 + inflation_rate);
        }

        if (board[i].type == utility || board[i].type == railway)
        {
            board[i].purchase_price *= (1 + inflation_rate);
            board[i].mortgage_value *= (1 + inflation_rate);
            board[i].base_rental *= (1 + inflation_rate);
        }
    }
    // insurance premiums and repair costs and the loan intrest rates add karala ne 
}

//buildings hasnt been added
int total_property_assets(property board[], int owner_id)
{
    int assets = 0;
    for (int i = 0; i < 40; i++)
    {
        if (board[i].current_owner == owner_id &&
            (board[i].type == land || board[i].type == railway || board[i].type == utility))
        {
            assets += board[i].purchase_price;
        }
    }
    return assets;
}

// the randomizer has to be fixed yet
void auction_function(int property_id, player players[], property board[], int owned_properties[], int market_decline_group)
{
    if ((board[property_id].type != land) && (board[property_id].type != utility) && (board[property_id].type != railway))
    {
        return;
    }

    int market_value = board[property_id].purchase_price;
    int original_price = board[property_id].original_price;
    int highest_bid = (int)(market_value * 0.50);
    if (highest_bid < original_price)
    {
        highest_bid = original_price;
    }
  
    if (market_decline_group >= brown)
    {
        if (board[property_id].color == market_decline_group)
        {
            highest_bid = (int)(highest_bid * 0.75);
        }
    }
    int highest_bidder = 0;
    int withdrawn[5] = {0, 0, 0, 0, 0};

    printf("\nAuction Started.\nProperty : %s\nOpening Bid : LKR %d\n", board[property_id].name, highest_bid);

    while (1)
    {
        for (int i = 1; i < 5; i++)
        {
            if (players[i].bankrupt == active)
            {
                withdrawn[i] = 1;
                continue;
            }
            if (withdrawn[i])
            {
                continue;
            }

            int bid = highest_bid + 250;
            int bids = 0;

            switch (players[i].player_id)
            {
            case AGGRESSIVE_INVESTOR:
            {

                if ((bid <= (int)(market_value * 1.20)) && (players[i].cash >= bid))
                {
                    bids = 1;
                }
                break;
            }
            case CONSERVATIVE_BANKER:
            {

                if ((bid < market_value) && (players[i].cash >= bid) && ((players[i].cash - bid) >= (players[i].cash * 0.5)))
                {
                    bids = 1;
                }
                break;
            }
            case RISK_TAKER:
            {

                if (players[i].cash >= bid)
                {
                    bids = 1;
                }
                break;
            }
            case OPPORTUNISTIC_TRADER:
            {
                
                if ((bid <= (int)(market_value * 0.90)) && (players[i].cash >= bid) && ((players[i].cash - bid) >= (players[i].cash * 0.2)))
                {
                    bids = 1;
                }
                break;
            }
            default:
            {
                break;
            }
            }

            if (bids)
            {
                highest_bid = bid;
                highest_bidder = i;
                printf("\n%s bids LKR %d.\n", players[i].name, highest_bid);
            }
            else
            {
                withdrawn[i] = 1;
                printf("\n%s withdraws.\n", players[i].name);
            }
        }

        int active_remaining = 0;
        for (int i = 1; i < 5; i++)
        {
            if (!withdrawn[i])
            {
                active_remaining++;
            }
        }

       
        if (active_remaining <= 1)
        {
            break;
        }
    }

    if (highest_bidder != 0)
    {
        Player_id old_owner = board[property_id].current_owner;
        if (old_owner != BANKER)
        {
            players[old_owner].policies[property_id].active = inactive;
            players[old_owner].policies[property_id].type = NO_INSURANCE;
        }
        players[highest_bidder].cash -= highest_bid;
        board[property_id].current_owner = highest_bidder;
        owned_properties[property_id] = highest_bidder;
        players[highest_bidder].num_properties++;
        players[highest_bidder].total_mortgage_value += board[property_id].mortgage_value;
        board[property_id].insurance_status = inactive;
        printf("\n%s wins the auction for %s with a bid of LKR %d.\n", players[highest_bidder].name, board[property_id].name, highest_bid);
    }
    else
    {

        printf("\nNo bids were made for %s the property goes to the Bank.\n", board[property_id].name);
    }
}

void round_counter_function(player players[], int *round_counter)
{
    int min_rounds = players[1].player_round_counter;
    for (int i = 2; i < 5; i++)
    {
        if (players[i].bankrupt == active)
        {
            continue;
        }
        if (players[i].player_round_counter < min_rounds)
        {
            min_rounds = players[i].player_round_counter;
        }
    }
    *round_counter = min_rounds;
}

void player_moving_function(player players[], property board[], int player_id)
{
    if (players[player_id].in_jail == active)
    {
        printf("\n%s is in jail and skips this turn\n", players[player_id].name);
        return;
    }

    int roll_1 = roll_dice();
    int roll_2 = roll_dice();
    int total_steps = roll_1 + roll_2;

    printf("\n%s rolled a %d and %d (Total: %d)\n", players[player_id].name, roll_1, roll_2, total_steps);

    players[player_id].location += total_steps;
    players[player_id].last_roll = total_steps;

    
    if (players[player_id].location >= 40)
    {
        players[player_id].location %= 40;
        players[player_id].cash += 2000;
        printf("%s passed GO and collected LKR 2,000 Current Balance: $%d\n", players[player_id].name, players[player_id].cash);
    }
    int location = players[player_id].location;
    printf("%s landed on tile %s\n", players[player_id].name, board[location].name);
}

void bankruptcy(player players[], activity *game_status, property board[], int owned_properties[])
{

    int bankrupted_players = 0;
    for (int i = 1; i < 5; i++)
    {
        if ((players[i].cash < 0) && (players[i].bankrupt != active))
        {
            players[i].bankrupt = active;

            printf("\n%s is bankrupted. Assets transferred to the Bank.\n", players[i].name);

            for (int p = 0; p < 40; p++)
            {
                players[i].policies[p].active = inactive;
                players[i].policies[p].type = NO_INSURANCE;
            }

            for (int j = 0; j < 40; j++)
            {
                if (board[j].current_owner == i)
                {
                    board[j].current_owner = BANKER;
                    board[j].mortgage_status = inactive;
                    board[j].insurance_status = inactive;
                    owned_properties[j] = BANKER;
                }
            }

            bankrupted_players++;
        }
        else if (players[i].bankrupt == active)
        {
            bankrupted_players++;
        }
    }
    if (bankrupted_players == 3)
    {
        *game_status = inactive;
        printf("\n############GAME_OVER##############\n");

        for (int j = 1; j < 5; j++)
        {
            if (players[j].bankrupt != active)
            {
                printf("\nTHE WINNER IS :%s", players[j].name);
            }
        }
    }
}

void build_function(player players[], property board[])
{
    for (int x = 1; x < 5; x++)
    {
        if (players[x].bankrupt == active)
        {
            continue;
        }

        for (int g = brown; g <= dark_blue; g++)
        {
            int group_tiles[40];
            int group_size = 0;

            for (int i = 0; i < 40; i++)
            {
                if ((board[i].type == land) && (board[i].color == g))
                {
                    group_tiles[group_size++] = i;
                }
            }
            if (group_size == 0)
            {
                continue;
            }

            int monopoly = 1;
            for (int j = 0; j < group_size; j++)
            {
                int idx = group_tiles[j];
                if ((board[idx].current_owner != x) || (board[idx].mortgage_status == active))
                {
                    monopoly = 0;
                    break;
                }
            }
            if (!monopoly)
            {
                continue;
            }

            while (1)
            {
                int min_level = 100;
                for (int j = 0; j < group_size; j++)
                {
                    int idx = group_tiles[j];
                    int level = (board[idx].no_of_hotels * 5) + board[idx].no_of_houses;
                    if (level < min_level)
                    {
                        min_level = level;
                    }
                }

                if (min_level >= 5)
                {
                    break;
                }

                int building_cost = board[group_tiles[0]].house_cost;
                if (min_level == 4)
                {
                    building_cost = board[group_tiles[0]].hotel_cost;
                }

                int build = 0;
                switch (players[x].player_id)
                {
                case AGGRESSIVE_INVESTOR:
                case RISK_TAKER:
                {
                    if (players[x].cash >= building_cost)
                    {
                        build = 1;
                    }
                    break;
                }
                case CONSERVATIVE_BANKER:
                {
                    if ((players[x].cash - building_cost) >= (players[x].cash * 0.5))
                    {
                        build = 1;
                    }
                    break;
                }
                case OPPORTUNISTIC_TRADER:
                {
                    int rent_gain = board[group_tiles[0]].base_rental * 10;
                    if ((rent_gain > building_cost) && (players[x].cash >= building_cost))
                    {
                        build = 1;
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }

                if (!build)
                {
                    break;
                }

                int built_any = 0;
                for (int j = 0; j < group_size; j++)
                {
                    int idx = group_tiles[j];
                    int level = (board[idx].no_of_hotels * 5) + board[idx].no_of_houses;
                    if (level != min_level)
                    {
                        continue;
                    }
                    if (players[x].cash < building_cost)
                    {
                        break;
                    }
                    if (min_level == 4)
                    {
                        players[x].cash -= board[idx].hotel_cost;
                        board[idx].no_of_houses = 0;
                        board[idx].no_of_hotels = 1;
                        players[x].buildings[idx] = (building){hotel, 1.0};
                        printf("%s built a hotel on %s for LKR %d.\n", players[x].name, board[idx].name, board[idx].hotel_cost);
                    }
                    else
                    {
                        players[x].cash -= board[idx].house_cost;
                        board[idx].no_of_houses++;
                        players[x].buildings[idx] = (building){house, 1.0};
                        printf("%s built a house on %s for LKR %d.\n", players[x].name, board[idx].name, board[idx].house_cost);
                    }
                    built_any = 1;
                }
                if (!built_any)
                {
                    break;
                }
            }
        }
    }
}

void go_to_jail(player players[], property board[])
{
    for (int i = 1; i < 5; i++)
    {
        if (players[i].location == board[30].index)
        {
            players[i].location = 10;
            players[i].in_jail = active;
            printf("\n%s was sent to jail \n", players[i].name);
        }
    }
}

void escape_jail(player players[])
{
    int jail_escape_type = 0;

    for (int x = 1; x < 5; x++)
    {
        if (players[x].in_jail == active)
        {
            if (players[x].in_jail_counter < 1)
            {
                jail_escape_type = (rand() % 3 + 1);
                switch (jail_escape_type)
                {
                case 1:
                    int dice_01, dice_02;
                    dice_01 = roll_dice();
                    dice_02 = roll_dice();
                    if (double_roll(dice_01, dice_02))
                    {
                        players[x].in_jail = inactive;
                        players[x].in_jail_counter = 0;
                        printf("\n%s has escaped the jail by rolling doubles\n", players[x].name);
                    }
                    break;
                case 2:
                    players[x].cash -= 300;
                    players[x].in_jail = inactive;
                    players[x].in_jail_counter = 0;
                    printf("\n%s has escaped the jail by paying Rs.300\n", players[x].name);
                    break;
                case 3:
                    players[x].in_jail_counter++;
                    printf("\n%s has decided to stay in jail \n", players[x].name);
                    break;
                default:
                    break;
                }
            }
            else
            {
                players[x].in_jail_counter++;
            }
            if (players[x].in_jail_counter == 3)
            {
                players[x].in_jail = inactive;
                players[x].in_jail_counter = 0;
                printf("\n%s has escaped the jail by staying for three rounds\n", players[x].name);
            }
        }
    }
}

void depreciation_function(player players[], property board[])
{
    for (int i = 0; i < 40; i++)
    {
        board[i].age++;

        if (board[i].type == land)
        {
            int age = board[i].age;
            int aging_count = (age - 50) / 5;
            if ((age > 50) && ((age - 50) % 5 == 0) && (aging_count <= 30))
            {
                board[i].purchase_price = (int)(board[i].purchase_price * 0.99);
                board[i].mortgage_value = (int)(board[i].mortgage_value * 0.99);
                board[i].base_rental = (int)(board[i].base_rental * 0.99);
                printf("%s depreciated by 1%% (new value LKR %d, new rent LKR %d)\n",board[i].name, board[i].purchase_price, board[i].base_rental);
            }
        }
    }

    for (int x = 1; x < 5; x++)
    {
        if (players[x].bankrupt == active)
        {
            continue;
        }
        for (int i = 0; i < 40; i++)
        {
            if (players[x].buildings[i].type != no_buildings && board[i].current_owner == x)
            {
                players[x].buildings[i].condition -= 0.02;
                if (players[x].buildings[i].condition < 0.0)
                {
                    players[x].buildings[i].condition = 0.0;
                }
            }
        }
    }
}

void renovation_function(player players[], property board[])
{
    for (int x = 1; x < 5; x++)
    {
        if (players[x].bankrupt == active)
        {
            continue;
        }

        int place = players[x].location;
        if ((board[place].type != land) || (board[place].current_owner != x))
        {
            continue;
        }

        activity needs_renovation = inactive;
        int aging_count = (board[place].age - 50) / 5;
        if (board[place].age <= 50)
        {
            aging_count = 0;
        }
        if (aging_count > 30)
        {
            aging_count = 30;
        }
        if (board[place].age > 50)
        {
            needs_renovation = active;
        }
        if (players[x].buildings[place].type != no_buildings && players[x].buildings[place].condition < 1.0)
        {
            needs_renovation = active;
        }
        if (needs_renovation == inactive)
        {
            continue;
        }

        int renovation_cost = (int)(board[place].purchase_price * 0.10);
        if (players[x].cash < renovation_cost)
        {
            printf("%s cannot afford to renovate %s (needs LKR %d).\n",players[x].name, board[place].name, renovation_cost);
            continue;
        }

        for (int k = 0; k < aging_count; k++)
        {
            board[place].purchase_price = (int)(board[place].purchase_price * (100.0 / 99.0));
            board[place].mortgage_value = (int)(board[place].mortgage_value * (100.0 / 99.0));
            board[place].base_rental = (int)(board[place].base_rental * (100.0 / 99.0));
        }
        board[place].age = 0;
        if (players[x].buildings[place].type != no_buildings)
        {
            players[x].buildings[place].condition = 1.0;
        }
        players[x].cash -= renovation_cost;
        printf("%s renovated %s for LKR %d.\n",players[x].name, board[place].name, renovation_cost);
        printf("Value restored, age reset, condition back to 100%%.\n");
    }
}


void loan_interest_accumulation(player players[], property board[])
{
    for (int i = 1; i < 5; i++)
    {
        if (players[i].loans_owned.active == active)
        {
            double interest = players[i].loans_owned.amount * players[i].loans_owned.interest_rate;
            players[i].loans_owned.amount += interest;
            players[i].loans_owned.loan_duration--;
            printf("%s's loan accrued LKR %.2f interest. Balance : LKR %.2f (%d rounds left).\n", players[i].name, interest, players[i].loans_owned.amount, players[i].loans_owned.loan_duration);

            if (players[i].loans_owned.loan_duration <= 0)
            {
             
                printf("%s defaulted on their loan - pledged assets transferred to the Bank.\n", players[i].name);
                for (int p = 0; p < 40; p++)
                {
                    if (board[p].current_owner == i)
                    {
                        board[p].current_owner = BANKER;
                        board[p].no_of_houses = 0;
                        board[p].no_of_hotels = 0;
                        board[p].mortgage_status = inactive;
                        board[p].insurance_status = inactive;
                        players[i].buildings[p].type = no_buildings;
                        players[i].buildings[p].condition = 0.0;
                    }
                    players[i].policies[p].active = inactive;
                }
                players[i].loans_owned.amount = 0.0;
                players[i].loans_owned.active = inactive;
                players[i].loans_owned.loan_duration = 0;
                players[i].total_mortgage_value = 0;
            }
        }
    }
}




void loan_payoff_function(int player_id, player players[])
{
    if (players[player_id].loans_owned.active == active)
    {
        int owed = (int)players[player_id].loans_owned.amount;
        if (players[player_id].cash >= owed)
        {
            players[player_id].cash -= owed;
            players[player_id].total_mortgage_value += owed;
            players[player_id].loans_owned.amount = 0.0;
            players[player_id].loans_owned.active = inactive;
            players[player_id].loans_owned.loan_duration = 0;
            printf("%s repaid their loan in full - LKR %d.\n", players[player_id].name, owed);
        }
        else
        {
            printf("%s cannot repay their loan of LKR %d (only has LKR %d).\n",players[player_id].name, owed, players[player_id].cash);
        }
    }
}
