
/**
 * file solve.c
 * @author Stirling Gould
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define HEIGHT 6
#define WIDTH 7
#define MAX_TURNS HEIGHT * WIDTH

#define GAME_ONGOING -1
#define RED 0
#define YELLOW 1
#define TIE 2

#define HEX_BOARD_FULL 0x007F7F7F7F7F7F7Full
#define HEX_SINGLE_COLUMN 0x0101010101010101ull

// bitshift instructions for: |  -  \  / win conditions
int winDirections[] =        {8, 1, 7, 9};

typedef struct {
    uint64_t mask; // Each bit represents a position filled/unfilled with any piece
    uint64_t current_player; // Each bit represents a position filled/unfilled with a the current player's piece
} Board;

int stackHeight(Board *bd, int slot) {
    uint64_t column = (bd->mask) & (HEX_SINGLE_COLUMN << slot);

    // Efficient assembly for counting bits in the 64-bit integer mask
    return __builtin_popcountll(column);
}

int numMoves(Board *bd){
    return __builtin_popcountll(bd->mask);
}

void displayBoard(Board *bd, bool isRedTurn) {
    uint64_t slots_red;
    uint64_t slots_yellow;

    if(isRedTurn){
        slots_red = bd->current_player;
        slots_yellow = bd->mask ^ bd->current_player;
    } else {
        slots_yellow = bd->current_player;
        slots_red = bd->mask ^ bd->current_player;
    }

    printf("╔══╦══╦══╦══╦══╦══╦══╗\n");
    for(int r = HEIGHT - 1; r >= 0; r--) {
        printf("║");
        for(int c = 0; c < WIDTH; c++) {
            uint64_t pos = (r * (WIDTH + 1) + c); // WIDTH + 1: Add extra zero column for padding, efficient win conditions
            if(((slots_yellow >> pos) & 0x1) == 1 ){
                printf("🟡");
            } else if (((slots_red >> pos) & 0x1) == 1) {
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

void placeInPos(Board *bd, uint64_t pos) {
    // Flip the current player mask to the other player
    // by XORing with the total board mask
    bd->current_player ^= bd->mask;

    // Add the piece to the total board mask
    // This placed piece will appear in current_player in the next board swap
    bd->mask |= (0x1ull << pos);
}

bool play(Board *bd, int slot) {
    int sh = stackHeight(bd, slot);
        if(sh >= 6) {
            return false;
        }

    placeInPos(bd, sh * (WIDTH + 1) + slot);
    return true;
}

bool playerWins(Board *bd) {
    uint64_t player_wins = false;

    uint64_t current_player = bd->mask ^ bd->current_player;
    for(int d = 0; d < 4; d++) { // Try all shift directions encoding a win condition
        int shift = winDirections[d];
        player_wins = (current_player) & 
                        (current_player >> shift) & 
                        (current_player >> (2 * shift)) & 
                        (current_player >> (3 * shift));
        if(player_wins){
            return true;
        }
    }

    return false;
}

int negamax(Board *bd){
    if(bd->mask == HEX_BOARD_FULL){
        return 0;
    }

    // Lower bound of possible score
    int bestScore = -MAX_TURNS;

    for(int s = 0; s < WIDTH; s++){
        if(stackHeight(bd, s) >= 6) // Prevent playing in full slots
            continue;
    
        Board *bd2 = &(*bd);

        play(bd2, s); // Check if the current player can win next move
        if(playerWins(bd2))
            return (WIDTH * HEIGHT) + 1 - (numMoves(bd2) / 2);

        // The current player's score is negative the opponent's score after playing in this slot
        int score = - negamax(bd2);
        if(score > bestScore)
            bestScore = score;
    }

    return bestScore;
}

int main(){
    Board bd = {0x0, 0x0}; // Initialize empty board
    displayBoard(&bd, true);
    int turns = 0;
    
    while(1){
        if(turns % 2 == 0){
            printf("Turn #%d, 🔴 Red to play.\n", turns);
        } else {
            printf("Turn #%d, 🟡 Yellow to play.\n", turns);
        }
        printf("> Place piece in row 1-7: ");

        int slot;
        if(scanf("%d", &slot) == 1 && slot >= 1 && slot <= WIDTH) {
            slot--;
        } else { // User input is not a number, or not 1-7.
            printf("Invalid slot.\n");
            continue;
        }

        if(!play(&bd, slot)){
            printf("Slot %d is already full!\n", (slot + 1));
            continue;
        }
 
        displayBoard(&bd, turns % 2);

        if(playerWins(&bd)){
            if(turns % 2 == 0){
                printf("🔴 Red wins!\n"); exit(0);
            } else {
                printf("🟡 Yellow wins!\n"); exit(0);
            }
        } else if(turns == MAX_TURNS - 1) {
            printf("It's a tie!\n"); exit(0);
        } else {
            turns++;
        }
    }
}
