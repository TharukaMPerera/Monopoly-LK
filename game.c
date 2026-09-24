#include "structures.h"

void main_game_loop(player players[], int *round_counter, property board[], int owned_properties[])
{
    activity game_status = active;
    double income_tax_rate = 0.15;
    int market_decline_group = -1;
    double current_inflation_rate = 0.0;
    while ((game_status == active) && (*round_counter < 500))
    {
        print_stats(players, round_counter);
        round_counter_function(players, round_counter);

        if ((*round_counter > 0) && ((*round_counter % 15) == 0))
        {
            economic_events((Economic_events)(rand() % 8), board);
            special_disaster();
        }

        if ((*round_counter > 0) && ((*round_counter % 20) == 0))
        {
            government_regulation((Government_regulations)(rand() % 8), board, &income_tax_rate);
        }

        // player moving function for all 4 players
        for (Player_id player_id = 1; player_id < 5; player_id++)
        {
            if (players[player_id].bankrupt != active)
            {
                player_moving_function(players, board, player_id);
                if ((players[player_id].location == 7) || (players[player_id].location == 22) || (players[player_id].location == 36))
                {
                    national_event_function(players, board, player_id);
                }
                if (players[player_id].location == 4)
                {
                    int assets = total_property_assets(board, player_id);
                    int tax_base = players[player_id].cash + assets;
                    int income_tax = (int)(tax_base * (income_tax_rate + current_inflation_rate));
                    if (players[player_id].cash >= income_tax)
                    {
                        players[player_id].cash -= income_tax;
                        printf("%s paid Income Tax of LKR %d (%.1f%% of cash + assets).\n", players[player_id].name, income_tax, (income_tax_rate + current_inflation_rate) * 100);
                    }
                    else
                    {
                        printf("%s cannot afford Income Tax of LKR %d.\n", players[player_id].name, income_tax);
                    }
                }
                if (players[player_id].location == 2)
                {
                    int assets = total_property_assets(board, player_id);
                    int fund_tax = (int)(assets * (0.10 + current_inflation_rate));
                    if (players[player_id].cash >= fund_tax)
                    {
                        players[player_id].cash -= fund_tax;
                        printf("%s paid Community Development Fund tax of LKR %d (%.1f%% of property assets).\n", players[player_id].name, fund_tax, (0.10 + current_inflation_rate) * 100);
                    }
                    else
                    {
                        printf("%s cannot afford Community Development Fund tax of LKR %d.\n", players[player_id].name, fund_tax);
                    }
                }
                if (players[player_id].location == 38)
                {
                    if (players[player_id].loans_owned.active == active)
                    {
                        loan_payoff_function(player_id, players);
                    }
                    else
                    {
                        int request = (int)(players[player_id].total_mortgage_value * 0.75);
                        if (request > 0)
                        {
                            loan_function(player_id, request, players, board);
                        }
                    }
                }
            }
            players[player_id].player_round_counter++;
        }
        if ((*round_counter % 10) == 0)
        {
            inflation(board, &current_inflation_rate);
            dynamic_property_market(board, &market_decline_group);
        }
        buy_function(owned_properties, players, board); // buy and rent
        for (Player_id player_id = 1; player_id < 5; player_id++)
        {
            if (players[player_id].bankrupt == active)
            {
                continue;
            }
            int place = players[player_id].location;
            if ((board[place].type == land || board[place].type == utility || board[place].type == railway) && (board[place].current_owner == BANKER))
            {
                auction_function(place, players, board, owned_properties, market_decline_group);
            }
        }
        insurance_function(players, board);
        build_function(players, board);
        rent_function(players, board);

        go_to_jail(players, board); // jail things
        escape_jail(players);

        depreciation_function(players, board); // breaking and building things
        renovation_function(players, board);

        if ((*round_counter > 0) && ((*round_counter % 10) == 0))
        {
            disasters(players, board);
        }

        (*round_counter)++;
        update_national_event_counters();
        update_insurance_policies(players, board);
        loan_interest_accumulation(players, board);

        bankruptcy(players, &game_status, board, owned_properties);
    }

    if (*round_counter >= 500)
    {
        printf("\n############MAXIMUM ROUNDS REACHED##############\n");
        int winner = 1;
        int winner_net_worth = -1;
        for (int i = 1; i < 5; i++)
        {
            int net_worth = players[i].cash + players[i].total_mortgage_value - (int)players[i].loans_owned.amount;
            if (net_worth > winner_net_worth)
            {
                winner_net_worth = net_worth;
                winner = i;
            }
        }
        printf("\nTHE WINNER IS : %s with a net worth of LKR %d\n", players[winner].name, winner_net_worth);
    }
}

void initial_game_loop()
{

    srand(time(NULL));
    int round_counter = 0;
    int owned_properties[40];
    for (int i = 0; i < 40; i++)
    {
        owned_properties[i] = BANKER;
    }
    player players[5];
    property board[40];
    Monopoly_board(board);
    game_start_print();
    player_initializer(players);
    player_order(players);
    main_game_loop(players, &round_counter, board, owned_properties);
}
