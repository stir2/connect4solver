/**
 * file transposition_table.c
 * Implementation file for storing evaluated positions for lookup
 * @author Stirling Gould
*/

#include "transposition_table.h"

uint64_t ZOBRIST_TABLE[2][STORED_HEIGHT][STORED_WIDTH];

#define TABLE_SIZE TABLE_ALLOC / sizeof(TTEntry)
TTEntry transpositionTable[TABLE_SIZE];

const int table_size = TABLE_SIZE;

void initZobrist(){
    for (int p = 0; p < 2; p++) {
        for (int c = 0; c < 16; c++) {
            for (int r = 0; r < 16; r++) {
                // Generate a random 64-bit integer
                uint64_t r1 = (uint64_t)rand();
                uint64_t r2 = (uint64_t)rand();
                ZOBRIST_TABLE[p][c][r] = (r1 << 32) | r2;
            }
        }
    }
}

void initTable(){
    printf("Table memory: %d\n", TABLE_ALLOC);
    printf("Memory per struct: %ld\n", sizeof(TTEntry));
    printf("Table size: %d\n", table_size);

    for(int i = 0; i < TABLE_SIZE; i++){
        transpositionTable[i] = (TTEntry){ULLONG_MAX, 0};
    }
}

void tablePut(uint64_t hash, uint8_t score){
    transpositionTable[hash % table_size] = (TTEntry){hash, score};
}

TTEntry tableGet(uint64_t hash){
    return transpositionTable[hash % table_size];
}

uint64_t lowestHash(Board *bd){
    return bd->hash_normal < bd->hash_mirror 
        ? bd->hash_normal : bd->hash_mirror;
}
