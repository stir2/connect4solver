/**
 * file solve.h
 * Header file for calculating a position's score (finding the best move)
 * @author Stirling Gould
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Resolve circular dependency with transposition_table.h
#ifndef SOLVE_H
#define SOLVE_H

#define STORED_HEIGHT 16
#define STORED_WIDTH 16

// Which connect are we playing? (It doesn't have to be 4...)
#define CONNECT 4

#define HEX_SINGLE_COLUMN 0xFFFF

extern const int height;
extern const int width;
extern const int max_turns;
extern int searched;

typedef struct {
    uint16_t mask[16];              // Each bit represents a position filled/unfilled with any piece
    uint16_t current_player[16];    // Each bit represents a position filled/unfilled with a the current player's piece
    uint64_t hash_normal;           // This Zobrist hash key for this board's transposition table lookup
    uint64_t hash_mirror;           // This Zobrist hash key for this board's mirror transposition table lookup
    uint8_t move_count;             // Increments every turn
} Board;

/**
 * Prints out a colorful representation of the board
 * @param bd the board to display
 * @param player_swap is true if bd's current_player has not yet been swapped
 */
void displayBoard(Board *bd, bool player_swap);

/**
 * Determine whether a slot inside the board is valid
 * @return true if the current stack is < the board height
 */
bool canPlay(Board *bd, int slot);

/**
 * Flip the current player mask to contain a bitboard the opponent's pieces
 * @param bd the board to edit
 */
void swapPlayers(Board *bd);

/**
 * Play a piece in the highest unfilled space of a specified slot
 * @param bd the position to edit
 * @param slot the vertical slot to drop the piece into
 */
void play(Board *bd, int slot);

/**
 * Check only the columns around the last played slot for a win
 * !! Make sure that the current_player mask has not been switched yet
 * @param bd the board to check
 * @param slot the last played slot
 */
bool playerWinsLocal(Board *bd, int slot);

/**
 * Recursively score connect4 position using negamax variant of alpha-beta algorithm.
 * @param bd the position to score
 * @param alpha the score of the best move that the maximizing, current player can take, lower bound
 * @param beta the score of the best move that the minimizing, opposing player can take, upper bound
 * @return Return the score of this position clamped by [alpha, beta] window
 */
int negamax(Board *bd, int alpha, int beta);

#endif // SOLVE_H