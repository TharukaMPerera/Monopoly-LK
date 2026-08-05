#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "board.c"
#include "players.c"

int order[4];

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

void first_go(int *order)
{
    int scores[4];
    int dice_01, dice_02;
    dice_01 = roll_dice();
    dice_02 = roll_dice();
    int total = dice_01 + dice_02;

    for (int i = 0; i < 4; i++)
    {
        scores[i] = total;
    }

    for (int i = 0; i < 4; i++)
    {
        printf("Player %d rolled: %d\n", i + 1, scores[i]);
    }
    order[0] = scores[0];
    for (int i = 1; i < 4; i++)
    {
        if (scores[i] > scores[i - 1])
        {
            order[i] = scores[i];
        }
    }
}

void buy(int player_id, int property_id, int *owned_properties[40], player players[4], property board[40])
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

void loan(int player_id, int player_request, player players[4], property board[40])
{
    if (players[1].total_mortgage_value > 0)
    {
        if (players[1].loans_owned.active == inactive)
        {
            if (player_request <= players[1].total_mortgage_value)
            {
                players[1].cash += player_request;
                players[1].total_mortgage_value -= player_request;
                printf("%s has taken a loan of $%d.\n", players[1].name, player_request);
                for (int i = 0; i < 40; i++)
                {
                    if (board[i].current_owner == 1)
                    {
                        board[i].mortgage_status = 1;
                    }
                }
                players[1].loans_owned.amount += player_request;
                players[1].loans_owned.loan_duration = 20;
                players[1].loans_owned.interest_rate = 0.05;
                players[1].loans_owned.active = active;
            }
            else
            {
                printf("%s cannot take a loan of $%d as it exceeds the total mortgage value of $%d.\n", players[1].name, player_request, players[1].total_mortgage_value);
            }
        }
    }

    else
    {
        printf("%s has no properties to act as collateral.\n", players[1].name);
    }
}
