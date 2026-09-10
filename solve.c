/**
 * file solve.c
 * @author Stirling Gould
*/

#include "solve.h"
#include "transposition_table.h"

const int height = 6;
const int width = 7;
const int max_turns = height * width;
const int EXPLORE_ORDER[7] = {3, 2, 4, 1, 5, 0, 6};
int searched;

int stackHeight(Board *bd, int slot) {
    return __builtin_popcount(bd->mask[slot]); // Efficient assembly for counting bits
}

bool canPlay(Board *bd, int slot){
    return stackHeight(bd, slot) < height;
}

void swapPlayers(Board *bd){
    for(int s = 0; s < width; s++)
        bd->current_player[s] ^= bd->mask[s];
}

/**
 * Place a piece in both board masks and increment move count
 * @param bd the board to edit
 * @param piece_index the index of the piece to place (4 bits select column, 4 bits select row)
 */
void placePiece(Board *bd, uint8_t piece_index) {
    int col = piece_index >> 4;
    int row = piece_index % 16;
    int piece = 0x1 << row;

    bd->current_player[col] |= piece;
    bd->mask[col] |= piece;

    bd->hash_normal ^= ZOBRIST_TABLE[bd->move_count % 2][row][col];
    bd->hash_mirror ^= ZOBRIST_TABLE[bd->move_count % 2][row][width - col - 1];

    bd->move_count++;
}

void play(Board *bd, int slot) {
    placePiece(bd, (slot * STORED_HEIGHT + stackHeight(bd, slot)));
}

bool playerWinsLocal(Board *bd, int slot){

    // The board's current player is now the opponent's, switch back before checking wins
    uint16_t *p = bd->current_player;

    // Check for vertical wins, fastest to compute
    uint16_t col = p[slot];
    for(int i = 0; i < CONNECT - 1; i++)
        col &= (col << 1);

    if(col) return true;

    // Check for horizontal, diagonal wins
    // We can rule out columns < 0 or > (width - CONNECT)
    int min_col = (slot < CONNECT - 1) ? 0 : slot - (CONNECT - 1);
    int max_col = (slot > width - CONNECT) ? width - CONNECT : slot;
    for(int c = min_col; c <= max_col; c++){

        // Initialize with starting column
        uint16_t h = p[c];
        uint16_t d1 = p[c];
        uint16_t d2 = p[c];

        // Bitwise AND the next (CONNECT - 1) columns in the 3 different directions
        for(int i = 1; i < CONNECT; i++){
            h &= p[c + i];                  // No height shift
            d1 &= (p[c + i] >> i);            // Shift next column down
            d2 &= (p[c + i] << i);            // Shift next column up
        }

        // If there is a 1 left in any direction then its a win
        if(h || d1 || d2) return true;
    }
    return false;
}

/**
 * Determines whether a certain move wins the game
 * @param bd the board to check
 * @param slot the winning move candidate
 * @return true if the move is valid and it wins the game
 */
bool isWinningMove(Board *bd, int slot){
    int sh = stackHeight(bd, slot);

    if(sh >= height) return false;

    Board bd2 = *bd; // Create copy of this board
    uint8_t piece_index = (slot * STORED_HEIGHT) + sh;

    // No need to ever swap players, bd2 shall be vanquished in due time >:3
    placePiece(&bd2, piece_index);
    return playerWinsLocal(&bd2, slot);
}

/**
 * -- MOVE THIS BACK TO play.c LATER --
 * Prints out a colorful representation of the board
 * @param bd the board to display
 * @param player_swap is true if bd's current_player has not yet been swapped
 */
void displayBoard(Board *bd, bool player_swap) {
    bool is_red_turn = bd->move_count % 2 == 0;
    
    printf("╔══╦══╦══╦══╦══╦══╦══╗\n");
    for(int r = height - 1; r >= 0; r--){
        printf("║");
        for(int c = 0; c < width; c++){
            // printf("%d%d", (bd->mask[c] >> r) & 0x1, (bd->current_player[c] >> r) & 0x1);
            if(!((bd->mask[c] >> r) & 0x1)){
                printf("  ");
            } else if ((is_red_turn != player_swap) == ((bd->current_player[c] >> r) & 0x1)){
                printf("🔴");
            } else {
                printf("🟡");
            }
            printf("║");
        }
        printf("\n");
    }
    printf("╚━━╩━━╩━━╩━━╩━━╩━━╩━━╝\n");
}

int negamax(Board *bd, int alpha, int beta){
    // getchar();
    // printf("[%d, %d]\n", alpha, beta);
    // displayBoard(bd, false);

    searched++;

    // Upper bound of possible score
    int max = (width * height - 1 - bd->move_count) / 2;
    if(beta > max){
        beta = max;                     // Relax beta to our best possible score
        if(alpha >= beta) return beta;  // Prune if [alpha, beta] window is empty
    }

    uint64_t hash = lowestHash(bd);
    TTEntry bd_eval = tableGet(hash);

    if(bd_eval.hash == hash){
        // printf("Found hash %ld, score %d\n", hash, bd_eval.score);
        return bd_eval.score;
    }

    if(bd->move_count == height * width) return 0;

    // Check if current player can win in 1 move
    for(int s = 0; s < width; s++){
        if(isWinningMove(bd, s)){
            return (width * height + 1 - bd->move_count) / 2;
        }
    }

    for(int i = 0; i < width; i++){
        int s = EXPLORE_ORDER[i];
        if(!canPlay(bd, s)) continue; // Prevent playing in full slots

        Board bd2 = (*bd); // Create copy of this board
        play(&bd2, s);
        swapPlayers(&bd2);
        
        int score = - negamax(&bd2, -beta, -alpha); // Explore the opponent's score in [-beta, -alpha] window
        
        if(score >= beta){
            tablePut(hash, score); // Cache the cutoff
            return score; // Prune if we find a better move than what we were searching for
        } 
        if(score > alpha) alpha = score; // Relax the window for the next search
    }

    tablePut(hash, alpha);
    return alpha;
}
