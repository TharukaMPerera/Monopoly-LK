#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "board.c"
#include "players.c"

static int order[4];

void game_start_print(player players[4]){
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
    srand(1);
    dice = rand() % 6 + 1;

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

// function first go is under construction
void first_go(int *order)
{
    int scores[4];
    int dice_01, dice_02;
    int higgest = 0;

    for (int i = 0; i < 4; i++)
    {
        dice_01 = roll_dice();
        dice_02 = roll_dice();
        scores[i] = dice_01 + dice_02;
        printf("Player %d rolled %d and %d for a total of %d.\n", i + 1, dice_01, dice_02, scores[i]);
        if (scores[i] > higgest)
        {
            higgest = scores[i];
        }
    }
    int tied = 1;
    while (1){  // while there is a tie
        tied = 0;
        for (int i = 0; i < 4; i++)
        {
            for (int j = i + 1; j < 4; j++)
            {
                if (scores[i] == scores[j])
                {
                    tied = 1;
                    printf("Player %d and Player %d are tied with a score of %d. Rerolling...\n", i + 1, j + 1, scores[i]);
                    dice_01 = roll_dice();
                    dice_02 = roll_dice();
                    scores[i] = dice_01 + dice_02;
                    printf("Player %d rerolled and got %d and %d for a total of %d.\n", i + 1, dice_01, dice_02, scores[i]);
                    dice_01 = roll_dice();
                    dice_02 = roll_dice();
                    scores[j] = dice_01 + dice_02;
                    printf("Player %d rerolled and got %d and %d for a total of %d.\n", j + 1, dice_01, dice_02, scores[j]);
                }
            }
        }
    }


}


// the buy of opotunistic trader has to be fixed
void buy_function(int player_id, int property_id, int *owned_properties[40], player players[5], property board[40])
{
    switch (player_id)
    {
    case 1:
    {
        if ((players[1].cash >= board[property_id].purchase_price) /*&&(the future rent)*/)
        {
            players[1].cash -= board[property_id].purchase_price;
            *owned_properties[property_id] = 1;
            board[property_id].current_owner = 1;
            players[1].num_properties++;
            printf("%s has purchased %s for $%d.\n", players[1].name, board[property_id].name, board[property_id].purchase_price);
            board[property_id].mortgage_value = board[property_id].purchase_price * 0.75;
            players[1].total_mortgage_value += board[property_id].mortgage_value;
        }
    }
    case 2:
    {
        int reserve_cash = players[2].cash - board[property_id].purchase_price;
        if (reserve_cash >= (players[2].cash * 0.5))
        {
            players[2].cash -= board[property_id].purchase_price;
            *owned_properties[property_id] = 2;
            board[property_id].current_owner = 2;
            players[2].num_properties++;
            printf("%s has purchased %s for $%d.\n", players[2].name, board[property_id].name, board[property_id].purchase_price);
            board[property_id].mortgage_value = board[property_id].purchase_price * 0.75;
            players[2].total_mortgage_value += board[property_id].mortgage_value;
        }
        break;
    }
    case 3:
    {
        if (players[3].cash >= board[property_id].purchase_price)
        {
            players[3].cash -= board[property_id].purchase_price;
            *owned_properties[property_id] = 3;
            board[property_id].current_owner = 3;
            players[3].num_properties++;
            printf("%s has purchased %s for $%d.\n", players[3].name, board[property_id].name, board[property_id].purchase_price);
            board[property_id].mortgage_value = board[property_id].purchase_price * 0.75;
            players[3].total_mortgage_value += board[property_id].mortgage_value;
        }
        break;
    }
        /*case 4:{i dont know what the fuck is going on }*/

    default:
        break;
    }
}

void loan_function(int player_id, int player_request, player players[5], property board[40])
{
    if (players[player_id].total_mortgage_value > 0)
    {
        if (players[player_id].loans_owned.active == inactive)
        {
            if (player_request <= players[player_id].total_mortgage_value)
            {
                players[player_id].cash += player_request;
                players[player_id].total_mortgage_value -= player_request;
                printf("%s has taken a loan of $%d.\n", players[player_id].name, player_request);
                for (int i = 0; i < 40; i++)
                {
                    if (board[i].current_owner == 1)
                    {
                        board[i].mortgage_status = 1;
                    }
                }
                players[player_id].loans_owned.amount += player_request;
                players[player_id].loans_owned.loan_duration = 20;
                players[player_id].loans_owned.interest_rate = 0.05;
                players[player_id].loans_owned.active = active;
            }
            else
            {
                printf("%s cannot take a loan of $%d as it exceeds the total mortgage value of $%d.\n", players[player_id].name, player_request, players[player_id].total_mortgage_value);
            }
        }
    }

    else
    {
        printf("%s has no properties to act as collateral.\n", players[player_id].name);
    }
}

void inflation(property board[40], double inflation_rate)
{
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
    // insurance premiums and repair costs and the loan intrest rates has to be added im too lazy to add it rn
}

// the randomizer has to be fixed yet
void auction_function(int property_id, player players[5], property board[40])
{
    int bought = 0;
    int highest_bid = 0;
    int bid = 0;

    if (((board[property_id].purchase_price) * 0.50) > board[property_id].original_price)
    {
        highest_bid = ((board[property_id].purchase_price) * 0.50);
    }
    else
    {
        highest_bid = board[property_id].original_price;
    }
    while (bought == 0) // auction loop
    {
        int skipped = 0;
        int skipped_players[4] = {0, 0, 0, 0};
        for (int player_id = 1; player_id < 5; player_id++)
        {
            switch (player_id)
            {
            case 1:
            {
                if ((board[property_id].current_owner != 1) && ((board[property_id].type == land) || (board[property_id].type == utility) || (board[property_id].type == railway)))
                {
                    if ((players[1].in_jail != active) && ((board[property_id].purchase_price) * 1.20 <= highest_bid))
                    {
                        bid = highest_bid + (rand() % 250 + 1);

                        if ((highest_bid > (board[property_id].purchase_price) * 1.20) && (players[1].cash >= bid))
                        {
                            printf("%s has bid $%d for %s.\n", players[1].name, bid, board[property_id].name);
                            highest_bid = bid;
                            break;
                        }
                        else
                        {
                            printf("%s won't bid anymore for %s.\n", players[1].name, board[property_id].name);
                            skipped_players[0] = 1;
                            break;
                        }
                    }
                }
            }
            case 2:
            {
                if ((board[property_id].current_owner != 2) && ((board[property_id].type == land) || (board[property_id].type == utility) || (board[property_id].type == railway)))
                {
                    if ((players[2].in_jail != active) && (highest_bid < board[property_id].original_price) && ((players[2].cash - bid) >= (players[2].cash * 0.5)))
                    {
                        bid = highest_bid + (rand() % 250 + 1);

                        if ((players[2].cash >= bid) && (highest_bid < board[property_id].original_price) && ((players[2].cash - bid) >= (players[2].cash * 0.5)))
                        {
                            printf("%s has bid $%d for %s.\n", players[2].name, bid, board[property_id].name);
                            highest_bid = bid;
                            break;
                        }
                        else
                        {
                            printf("%s won't bid anymore for %s.\n", players[2].name, board[property_id].name);
                            skipped_players[1] = 1;
                            break;
                        }
                    }
                }
            }
            case 3:
            {

                if ((board[property_id].current_owner != 3) && ((board[property_id].type == land) || (board[property_id].type == utility) || (board[property_id].type == railway)))
                {
                    if ((players[3].in_jail != active))
                    {
                        bid = highest_bid + (rand() % 250 + 1);

                        if (players[3].cash >= bid)
                        {
                            printf("%s has bid $%d for %s.\n", players[3].name, bid, board[property_id].name);
                            highest_bid = bid;
                            break;
                        }
                        else
                        {
                            printf("%s won't bid anymore for %s.\n", players[3].name, board[property_id].name);
                            skipped_players[2] = 1;
                            break;
                        }
                    }
                }
            }
            case 4:
            {
                if ((board[property_id].current_owner != 4) && ((board[property_id].type == land) || (board[property_id].type == utility) || (board[property_id].type == railway)))
                {
                    if ((players[4].in_jail != active))
                    {
                        bid = highest_bid + (rand() % 250 + 1);

                        if (players[4].cash >= bid)
                        {
                            printf("%s has bid $%d for %s.\n", players[4].name, bid, board[property_id].name);
                            highest_bid = bid;
                            break;
                        }
                        else
                        {
                            printf("%s won't bid anymore for %s.\n", players[4].name, board[property_id].name);
                            skipped_players[3] = 1;
                            break;
                        }
                    }
                }
            }
            }
        }// WINNER PART
        for (int i = 0; i < 4; i++)
        {
            if (skipped_players[i] == 1)
            {
                skipped++;
            }
            if (skipped == 3)
            {
                bought = 1;
                for (int j = 0; j < 4; j++)
                {
                    if (skipped_players[j] == 0)
                    {
                        players[j + 1].cash -= highest_bid;
                        board[property_id].current_owner = j + 1;
                        players[j + 1].num_properties++;
                        *players[j + 1].owned_properties[property_id] = j + 1;
                        players[j + 1].total_mortgage_value += board[property_id].mortgage_value;
                        printf("%s has won the auction for %s with a bid of $%d.\n", players[j + 1].name, board[property_id].name, highest_bid);
                        bought = 1;
                        break;
                    }
                }
            }
        }
    }
}
// the auction of the opotunistic trader also have to be fixed 