/**
 * file transposition_table.h
 * Header file for storing evaluated positions for lookup
 * @author Stirling Gould
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

// Resolve circular dependency with solve.h
#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "solve.h"

// Maximum size of the transposition table ~64MB
// Prime number sizes reduce collisions
#define TABLE_ALLOC 512000009

#define FLAG_EXACT 0 // score is exact
#define FLAG_LOWER 1 // score is a beta cutoff
#define FLAG_UPPER 2 // score failed to raise alpha

/**
 * One index for each space for each player, with rando
 * Used to XOR current board's TT hash when a piece is 
 */
extern uint64_t ZOBRIST_TABLE[2][STORED_HEIGHT][STORED_WIDTH];

typedef struct {
    uint64_t hash;
    uint8_t score;
} TTEntry;

/**
 * Fill the the zobrist table with random noise
 */
void initZobrist();

/**
 * Fill the transposition table with blank entries
 */
void initTable();

/**
 * Insert an entry into the transposition table
 */
void tablePut(uint64_t hash, uint8_t score);

/**
 * Get the score of a hash from the transposition table
 */
TTEntry tableGet(uint64_t hash);

/**
 * Returns the canonical hash of the board,
 * which is the min of the normal and mirror hashes
 */
uint64_t lowestHash(Board *bd);

/**
 * Edit a board's hash after a piece has been placed
 */
void updateHash(Board *bd, int piece_index);

#endif // TRANSPOSITION_TABLE_H