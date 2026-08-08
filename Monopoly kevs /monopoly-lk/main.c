#include <stdio.h>
#include <stdlib.h>
#include "types.h"

int main(void) {
    GameState gs;
    init_game(&gs);
    run_simulation(&gs);
    return 0;
}
