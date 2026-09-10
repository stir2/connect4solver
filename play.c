/**
 * file play.c
 * @author Stirling Gould
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "solve.h"

extern const int height;
extern const int width;
extern const int max_turns;
 
int main(){
    Board bd = {.move_count = 0};
    for(int i = 0; i < STORED_HEIGHT; i++){
        bd.mask[i] = 0x0;
        bd.current_player[i] = 0x0;
    }
    displayBoard(&bd, true);
    
    while(1){
        if(bd.move_count % 2 == 0){
            printf("Turn #%d, 🔴 Red to play.\n", bd.move_count);
        } else {
            printf("Turn #%d, 🟡 Yellow to play.\n", bd.move_count);
        }
        printf("> Place piece in row 1-7: ");

        int slot;
        if(scanf("%d", &slot) == 1 && slot >= 1 && slot <= width) {
            slot--;
        } else { // User input is not a number, or not 1-7.
            printf("Invalid slot.\n");
            continue;
        }

        if(!canPlay(&bd, slot)){
            printf("Slot %d is already full!\n", (slot + 1));
            continue;
        }

        play(&bd, slot);

        displayBoard(&bd, true);

        if(playerWinsLocal(&bd, slot)){
            if(bd.move_count % 2 == 0){
                printf("🟡 Yellow wins!\n"); exit(0);
            } else {
                printf("🔴 Red wins!\n"); exit(0);
            }
        } else if(bd.move_count == max_turns) {
            printf("It's a tie!\n"); exit(0);
        }
        swapPlayers(&bd);
    }

    return 0;
}