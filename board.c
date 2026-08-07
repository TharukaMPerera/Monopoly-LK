#include "structures.h"

void Monopoly_board()
{
    property board[40];

    // board[x] = (property){index, color, type, name, original_price, purchase_price, mortgage_value, base_rental, house_cost, current_owner, hotel_cost, mortgage_status, insurance_status, no_of_houses, no_of_hotels, age};
    board[0] = (property){0, none, start, "Start", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[1] = (property){1, brown, land, "Pettah", 1500, 1500, 750, 0, 500, BANK, 2000, inactive, inactive, 0, 0, 0};
    board[2] = (property){2, none, event, "Community Development Fund", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[3] = (property){3, brown, land, "Maradana", 1500, 1500, 750, 0, 500, BANK, 2000, inactive, inactive, 0, 0, 0};
    board[4] = (property){4, none, tax, "Income Tax", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[5] = (property){5, none, railway, "Colombo Fort Railway Station", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[6] = (property){6, light_blue, land, "Bambalapitiya", 2500, 2500, 1250, 0, 750, BANK, 3000, inactive, inactive, 0, 0, 0};
    board[7] = (property){7, none, event, "National Event Card", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[8] = (property){8, light_blue, land, "Wellawatta", 2500, 2500, 1250, 0, 750, BANK, 3000, inactive, inactive, 0, 0, 0};
    board[9] = (property){9, light_blue, land, "Mount Lavinia", 2500, 2500, 1250, 0, 750, BANK, 3000, inactive, inactive, 0, 0, 0};
    board[10] = (property){10, none, special, "Jail / Just Visiting", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[11] = (property){11, pink, land, "Nugegoda", 3500, 3500, 1750, 0, 1000, BANK, 4000, inactive, inactive, 0, 0, 0};
    board[12] = (property){12, none, utility, "Ceylon Electricity Board", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[13] = (property){13, pink, land, "Maharagama", 3500, 3500, 1750, 0, 1000, BANK, 4000, inactive, inactive, 0, 0, 0};
    board[14] = (property){14, pink, land, "Kottawa", 3500, 3500, 1750, 0, 1000, BANK, 4000, inactive, inactive, 0, 0, 0};
    board[15] = (property){15, none, railway, "Kandy Railway Station", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[16] = (property){16, orange, land, "Negombo", 4500, 4500, 2250, 0, 1250, BANK, 5000, inactive, inactive, 0, 0, 0};
    board[17] = (property){17, none, insurance_companies, "lanka Insurance", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[18] = (property){18, orange, land, "Katunayake", 4500, 4500, 2250, 0, 1250, BANK, 5000, inactive, inactive, 0, 0, 0};
    board[19] = (property){19, orange, land, "Ja-Ela", 4500, 4500, 2250, 0, 1250, BANK, 5000, inactive, inactive, 0, 0, 0};
    board[20] = (property){20, none, special, "Free Parking", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[21] = (property){21, red, land, "Kandy City", 5500, 5500, 2750, 0, 1500, BANK, 6000, inactive, inactive, 0, 0, 0};
    board[22] = (property){22, none, event, "National Event Card", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[23] = (property){23, red, land, "Peradeniya", 5500, 5500, 2750, 0, 1500, BANK, 6000, inactive, inactive, 0, 0, 0};
    board[24] = (property){24, red, land, "Katugastota", 5500, 5500, 2750, 0, 1500, BANK, 6000, inactive, inactive, 0, 0, 0};
    board[25] = (property){25, none, railway, "Galle Railway Station", 0, 0, 0, 0, 0, BANK, inactive, inactive, 0, 0, 0};
    board[26] = (property){26, yellow, land, "Galle Fort", 6500, 6500, 3250, 0, 2000, BANK, 8000, inactive, inactive, 0, 0, 0};
    board[27] = (property){27, yellow, land, "Unawatuna", 6500, 6500, 3250, 0, 2000, BANK, 8000, inactive, inactive, 0, 0, 0};
    board[28] = (property){28, none, utility, "National Water Supply and Drainage Board", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[29] = (property){29, yellow, land, "Hikkaduwa", 6500, 6500, 3250, 0, 2000, BANK, 8000, inactive, inactive, 0, 0, 0};
    board[30] = (property){30, none, special, "Go to Jail", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[31] = (property){31, green, land, "Jaffna Town", 8000, 8000, 4000, 0, 2500, BANK, 10000, inactive, inactive, 0, 0, 0};
    board[32] = (property){32, green, land, "Nallur", 8000, 8000, 4000, 0, 2500, BANK, 10000, inactive, inactive, 0, 0, 0};
    board[33] = (property){33, none, insurance_companies, "Ceylinco Insurance", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[34] = (property){34, green, land, "Trincomalee", 8000, 8000, 4000, 0, 2500, BANK, 10000, inactive, inactive, 0, 0, 0};
    board[35] = (property){35, none, railway, "Jaffna Railway Station", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[36] = (property){36, none, event, "National Event Card", 0, 0, 0, 0, 0, NONE, 0, inactive, inactive, 0, 0, 0};
    board[37] = (property){37, dark_blue, land, "Nuwara Eliya", 10000, 10000, 5000, 0, 3000, BANK, 12000, inactive, inactive, 0, 0, 0};
    board[38] = (property){38, none, bank, "Bank of Ceylon", 0, 0, 0, 0, 0, BANK, 0, inactive, inactive, 0, 0, 0};
    board[39] = (property){39, dark_blue, land, "Galle Face", 10000, 10000, 5000, 0, 3000, BANK, 12000, inactive, inactive, 0, 0, 0};
}
