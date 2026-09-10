/**
 * file solve.c
 * @author Stirling Gould
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "solve.h"
#include "transposition_table.h"

int toDigit(int ch){
    return ch - 48;
}

void runTest(char *file_name){

    initZobrist();

    // Initialize test stats
    int total = 0;
    int passed = 0;
    int failed = 0;

    FILE *fp = fopen(file_name, "r");

    // Initialize 
    char moves[max_turns + 1];
    int expected_score;
    do {

        // Initialize board
        Board bd = {
            .move_count     = 0,
            .hash_normal    = 0,
            .hash_mirror  = 0
        };
        for(int i = 0; i < STORED_HEIGHT; i++){
            bd.mask[i] = 0x0;
            bd.current_player[i] = 0x0;
        }

        // Scan for a test line
        // If the scan matches no formats, end the test
        // If the scan matches some amount of formats != 2, line is invalid
        int n = fscanf(fp, "%s %d", moves, &expected_score);
        if(n == 0) break;
        if(n != 2){
            total++; failed++; continue;
        }

        for(int c = 0; c < strlen(moves); c++){
            int ch = moves[c];
            // If line is invalid, fail and skip it
            if(!isdigit(ch)){
                while(ch != '\n')
                    ch = getc(fp);

                printf("f");
                failed++; total++; continue;
            }

            play(&bd, toDigit(ch) - 1);
            swapPlayers(&bd);
        }

        displayBoard(&bd, false);

        initTable();

        int score = negamax(&bd, INT16_MIN, INT16_MAX);
        printf("Expected Score: %d\nActual Score: %d\n", expected_score, score);

        if(score == expected_score){
            printf("PASS\n");
            passed++;
        } else {
            printf("FAIL\n");
            failed++;
        }
        total++;

    } while(1);

    printf("Ran Test: \'%s\', on %d Games:", file_name, total);
    printf("\t- %d Passed", passed);
    printf("\t- %d Failed", failed);
}

int main(){
    runTest("Test_L1_R1");
}