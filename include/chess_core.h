/** \file chess_core.h
 *
 * \brief Lightweight, SDL-free chess engine core: board state, move
 *        generation, move application (copy-make), evaluation and
 *        alpha-beta search. This is the single source of truth for
 *        game logic; the GUI merely renders it.
 */

#ifndef CHESS_CORE_H
#define CHESS_CORE_H

#include <cstdint>
#include <atomic>
#include "structs.h"

/* ------------------------------------------------------------------ */
/* Piece encoding                                                      */
/* ------------------------------------------------------------------ */
/* Signed int8_t: white pieces positive, black negative, empty = 0.   */
enum Piece_Code : int8_t
{
    EMPTY    = 0,
    WPAWN    =  1, WKNIGHT =  2, WBISHOP =  3, WROOK =  4, WQUEEN =  5, WKING =  6,
    BPAWN    = -1, BKNIGHT = -2, BBISHOP = -3, BROOK = -4, BQUEEN = -5, BKING = -6
};

/* Absolute piece types (colorless) */
enum Piece_Type : int8_t
{
    TYPE_PAWN   = 1,
    TYPE_KNIGHT = 2,
    TYPE_BISHOP = 3,
    TYPE_ROOK   = 4,
    TYPE_QUEEN  = 5,
    TYPE_KING   = 6
};

/* ------------------------------------------------------------------ */
/* Squares                                                             */
/* ------------------------------------------------------------------ */
/* Index = y*8 + x with y=0 being rank 8 (a8=0 ... h8=7, a1=56 ... h1=63).
 * Identical to the GUI's historical board[] indexing.                */
enum Square_Const : int8_t
{
    SQ_A8 = 0, SQ_E8 = 4, SQ_H8 = 7,
    SQ_A1 = 56, SQ_E1 = 60, SQ_H1 = 63
};

inline bool square_ok(int x, int y) { return x >= 0 && x < 8 && y >= 0 && y < 8; }

/* ------------------------------------------------------------------ */
/* Castling rights bits                                                */
/* ------------------------------------------------------------------ */
enum Castling_Bits : uint8_t
{
    CASTLE_WK = 1,
    CASTLE_WQ = 2,
    CASTLE_BK = 4,
    CASTLE_BQ = 8
};

/* ------------------------------------------------------------------ */
/* Moves                                                               */
/* ------------------------------------------------------------------ */
enum Move_Flags : uint8_t
{
    FLAG_NONE        = 0,
    FLAG_EP          = 1,   /* en-passant capture                        */
    FLAG_CASTLE      = 2,   /* king moves two files, rook jumps          */
    FLAG_DOUBLE_PUSH = 4    /* pawn double push, sets ep_square          */
};

struct Move
{
    uint8_t from;
    uint8_t to;
    int8_t  promo;              /* promoted piece code (signed), 0 = none */
    uint8_t flags;
};

constexpr int CORE_MAX_MOVES = 256;

struct Move_List
{
    Move moves[CORE_MAX_MOVES];
    int  count;

    void push(Move m)
    {
        if(count < CORE_MAX_MOVES)
            moves[count++] = m;
    }
};

/* ------------------------------------------------------------------ */
/* Position                                                            */
/* ------------------------------------------------------------------ */
/* Small plain struct so search can copy it per node.  Holds a few
 * incremental members (eval / hash / king squares) that make_move
 * maintains so the hot search path avoids full-board rescans.        */
struct Position
{
    int8_t  board[64];
    int8_t  side_to_move;       /* Color enum: WHITE / BLACK             */
    uint8_t castling;           /* Castling_Bits mask                    */
    int8_t  ep_square;          /* target square for en passant, -1 none */
    int     halfmove_clock;     /* for the 50-move rule                  */

    /* incremental state, updated by make_move / initialization */
    int      eval;              /* material+PST, WHITE-perspective (+ = good for white)  */
    uint32_t zkey;              /* Zobrist hash of the whole position                     */
    int8_t   king_square[2];    /* square of each king: [WHITE],[BLACK]; -1 if none      */

    int8_t piece_at(int sq) const { return board[sq]; }
};

constexpr int CORE_MATE   = 100000;
constexpr int CORE_INFINITE = 1000000;

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */
namespace core {

/* setup ----------------------------------------------------------- */
void position_init(Position& p);                 /* standard start position */
bool fen_load(const char* fen, Position& p);     /* parse FEN string        */
void zobrist_init();                             /* build Zobrist tables     */

/* queries --------------------------------------------------------- */
bool in_check_color(const Position& p, int color);          /* is `color`'s king attacked? */
bool in_check(const Position& p);                           /* side-to-move in check?      */
bool is_square_attacked(const Position& p, int sq, int by); /* attacked by `by` color?     */

int  generate_moves(const Position& p, Move_List& out);     /* fully legal moves; returns count */

/* applying moves (copy-make) -------------------------------------- */
void make_move(const Position& src, Position& dst, Move m);

/* search / eval ---------------------------------------------------- */
int  evaluate(const Position& p);               /* centipawns, side-to-move perspective */
int  perft(const Position& p, int depth);       /* node counter for validation          */
int  search_root(const Position& p, int depth, Move& best_out,
                 int time_alloc_ms,
                 int threads = 1);          /* parallel (Lazy SMP) thread count;
                                               1 = single-threaded              */
void search_clear_tables();                     /* wipe TT / history / killers         */
void search_clear_uci_stop();                   /* reset UCI stop flag                  */
extern std::atomic<bool> uci_stop;              /* checked by timed_out() for `stop`   */
uint64_t search_nodes_last();                   /* nodes visited by last search         */

/* debug helpers ---------------------------------------------------- */
void  move_print(Move m);
void  position_print(const Position& p);
const char* square_name(int sq);
uint32_t zkey_recompute(const Position& p);     /* full re-hash, for testing */

} /* namespace core */

#endif /* CHESS_CORE_H */
