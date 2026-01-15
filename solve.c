/**
 * file solve.c
 * @author Stirling Gould
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#define NUM_ROWS 6
#define NUM_COLS 7

#define GAME_ONGOING -1;
#define RED 0
#define YELLOW 1
#define TIE 2

#define HEX_BOARD_FULL 0x007F7F7F7F7F7F7Full
#define HEX_SINGLE_COLUMN 0x0101010101010101ull

// bitshift instructions for: |  -  \  / win conditions
int winDirections[] =        {8, 1, 7, 9};

typedef struct {
    uint64_t slots_red; // Each bit represents a position filled/unfilled with a red piece
    uint64_t slots_yellow; // Each bit represents a position filled/unfilled with a yellow piece
    // Note that no position should have a 1 in both slots_red and slots_yellow, +8 unused bits
} Board;

void displayBoard(Board bd){
    printf("╔══╦══╦══╦══╦══╦══╦══╗\n");
    for(int r = NUM_ROWS - 1; r >= 0; r--) {
        printf("║");
        for(int c = 0; c < NUM_COLS; c++) {
            uint64_t pos = (r * (NUM_COLS + 1) + c); // NUM_COLS + 1: Add extra zero column for padding, efficient win conditions
            if(((bd.slots_yellow >> pos) & 0x1) == 1){
                printf("🟡");
            } else if (((bd.slots_red >> pos) & 0x1) == 1){
                printf("🔴");
            } else {
                printf("  ");
            }

            printf("║");
        }
        printf("\n");
    }
    printf("╚━━╩━━╩━━╩━━╩━━╩━━╩━━╝\n");
}

void placeInPos(Board *bd, uint64_t pos, bool color){
    if(color == RED){
        bd->slots_red |= (0x1ull << pos);
    } else {
        bd->slots_yellow |= (0x1ull << pos);
    }
}

int stackHeight(Board bd, int slot){
    uint64_t column = (bd.slots_red | bd.slots_yellow) & (HEX_SINGLE_COLUMN << slot);
    return __builtin_popcountll(column);
}

int winState(Board bd){
    for(int d = 0; d < 4; d++){
        int shift = winDirections[d];

        uint64_t red_wins = (bd.slots_red) & 
                            (bd.slots_red >> shift) & 
                            (bd.slots_red >> (2 * shift)) & 
                            (bd.slots_red >> (3 * shift));
        
        // Returns 0 if red_wins is *any* nonzero integer
        if(red_wins) return RED;

        uint64_t yellow_wins = (bd.slots_yellow) & 
                            (bd.slots_yellow >> shift) & 
                            (bd.slots_yellow >> (2 * shift)) & 
                            (bd.slots_yellow >> (3 * shift));
        
        // Returns 1 if yellow_wins is *any* nonzero integer
        if(yellow_wins) return YELLOW; 
    }

    // All slots are filled and no player has won
    if((bd.slots_red | bd.slots_yellow) == HEX_BOARD_FULL) return TIE;
    
    // All win directions searched, no player has won, and the board is not full
    return GAME_ONGOING;
}

int main(){
    Board bd = {0x0, 0x0}; // Initialize empty board
    displayBoard(bd);
    int turn = 0;

    while(1){
        printf("> Place piece in row 1-7:\n");

        int slot;
        if(scanf("%d", &slot) == 1 && slot >= 1 && slot <= NUM_COLS){
            slot--;
        } else { // User input is not a number, or not 1-7.
            printf("Invalid slot.\n");
            continue;
        }

        int sh = stackHeight(bd, slot);
        if(sh >= 6){
            printf("Slot %d is already full!\n", (slot + 1));
            continue;
        }

        placeInPos(&bd, sh * (NUM_COLS + 1) + slot, turn % 2);
        displayBoard(bd);

        int ws = winState(bd);
        switch(ws){
            case -2: printf("It's a tie!\n"); exit(0);
            case -1: turn++; continue;
            case 0: printf("Red(🔴) wins!\n"); exit(0);
            case 1: printf("Yellow(🟡) wins!\n"); exit(0);
        }
    }
}
