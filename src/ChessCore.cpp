#include "chess_core.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

namespace {

inline int8_t color_of(int8_t piece) { return piece > 0 ? WHITE : BLACK; }
inline int8_t abs_type(int8_t piece) { return piece < 0 ? (int8_t)-piece : piece; }
inline int8_t enemy_of(int8_t color) { return color == WHITE ? BLACK : WHITE; }

/* knight offsets */
const int KNIGHT_D[8][2] =
{
    {-1,-2},{ 1,-2},{ 2,-1},{ 2, 1},
    { 1, 2},{-1, 2},{-2, 1},{-2,-1}
};

/* king offsets */
const int KING_D[8][2] =
{
    {-1,-1},{ 0,-1},{ 1,-1},{-1, 0},
    { 1, 0},{-1, 1},{ 0, 1},{ 1, 1}
};

/* ray directions */
const int ROOK_D[4][2]   = {{1,0},{-1,0},{0,1},{0,-1}};
const int BISHOP_D[4][2] = {{1,1},{-1,-1},{-1,1},{1,-1}};

/* castling-rights update masks per square: rights &= mask[from] & mask[to] */
const uint8_t CASTLE_MASK[64] =
{
    /* y = 0 : rank 8 */
    0x0F & ~CASTLE_BQ, 0x0F, 0x0F, 0x0F, 0x0F & ~(CASTLE_BK|CASTLE_BQ), 0x0F, 0x0F, 0x0F & ~CASTLE_BK,
    /* y = 1..6 : untouched squares */
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
    /* y = 7 : rank 1 */
    0x0F & ~CASTLE_WQ, 0x0F, 0x0F, 0x0F, 0x0F & ~(CASTLE_WK|CASTLE_WQ), 0x0F, 0x0F, 0x0F & ~CASTLE_WK
};

/* ------------------------------------------------------------------ */
/* Piece-square tables (white perspective; index 0 = a8)               */
/* Simplified Evaluation Function, chessprogramming.org                */
/* ------------------------------------------------------------------ */
const int PST_PAWN[64] =
{
      0,  0,  0,  0,  0,  0,  0,  0,
     50, 50, 50, 50, 50, 50, 50, 50,
     10, 10, 20, 30, 30, 20, 10, 10,
      5,  5, 10, 25, 25, 10,  5,  5,
      0,  0,  0, 20, 20,  0,  0,  0,
      5, -5,-10,  0,  0,-10, -5,  5,
      5, 10, 10,-20,-20, 10, 10,  5,
      0,  0,  0,  0,  0,  0,  0,  0
};
const int PST_KNIGHT[64] =
{
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};
const int PST_BISHOP[64] =
{
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};
const int PST_ROOK[64] =
{
      0,  0,  0,  0,  0,  0,  0,  0,
      5, 10, 10, 10, 10, 10, 10,  5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
     -5,  0,  0,  0,  0,  0,  0, -5,
      0,  0,  0,  5,  5,  0,  0,  0
};
const int PST_QUEEN[64] =
{
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
const int PST_KING[64] =
{
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

const int MATERIAL[7] = { 0, 100, 320, 330, 500, 900, 20000 };
const int* PST[7]      = { nullptr, PST_PAWN, PST_KNIGHT, PST_BISHOP, PST_ROOK, PST_QUEEN, PST_KING };

/* ------------------------------------------------------------------ */
/* Zobrist hashing                                                     */
/* ------------------------------------------------------------------ */
/* 12 piece codes x 64 squares + side-to-move + castling (16) + ep.     */
uint32_t ZOB_PIECE[12][64];
uint32_t ZOB_CASTLING[16];
uint32_t ZOB_EP[65];               /* index 64 = no ep square              */
uint32_t ZOB_SIDE;

inline int  zid(int8_t piece)      { return (piece > 0) ? (piece - 1) : (-piece - 1 + 6); }
inline void add_piece_hash(uint32_t& h, int8_t piece, int sq)
{
    if(piece != EMPTY)
        h ^= ZOB_PIECE[zid(piece)][sq];
}

/* White-perspective centipawn contribution of a piece on a square,
 * matching the (unmirrored-PST) convention of evaluate().          */
inline int sq_piece_value(int8_t piece, int sq)
{
    if(piece == EMPTY) return 0;
    const int type = abs_type(piece);
    int v = MATERIAL[type] + PST[type][sq];
    return (piece > 0) ? v : -v;
}

/* Full re-computation helpers (used for init and self-verification). */
void recompute_meta(const Position& src, Position& dst)
{
    dst.eval = 0;
    dst.zkey = 0;
    dst.king_square[WHITE] = -1;
    dst.king_square[BLACK] = -1;
    for(int sq = 0; sq < 64; sq++)
    {
        const int8_t pc = src.board[sq];
        if(pc == EMPTY) continue;
        dst.eval += sq_piece_value(pc, sq);
        dst.zkey ^= ZOB_PIECE[zid(pc)][sq];
        if(abs_type(pc) == TYPE_KING)
            dst.king_square[(pc > 0) ? WHITE : BLACK] = (int8_t)sq;
    }
    if(src.side_to_move == BLACK)
        dst.zkey ^= ZOB_SIDE;
    dst.zkey ^= ZOB_CASTLING[src.castling];
    dst.zkey ^= (src.ep_square >= 0) ? ZOB_EP[src.ep_square] : ZOB_EP[64];
}

/* ------------------------------------------------------------------ */
/* internal helpers                                                    */
/* ------------------------------------------------------------------ */

int find_king(const Position& p, int color)
{
    const int8_t target = (color == WHITE) ? WKING : BKING;
    for(int i = 0; i < 64; i++)
        if(p.board[i] == target)
            return i;
    return -1;
}

void push_pawn_move(Move_List& out, int from, int to, uint8_t flags)
{
    Move m;
    m.from  = (uint8_t)from;
    m.to    = (uint8_t)to;
    m.promo = 0;
    m.flags = flags;
    out.push(m);
}

void push_pawn_promotions(Move_List& out, int from, int to, bool white, uint8_t flags)
{
    const int8_t base = white ? WQUEEN : BQUEEN;
    const int8_t promos[4] =
    {
        base,
        (int8_t)(white ? WROOK    : BROOK),
        (int8_t)(white ? WBISHOP  : BBISHOP),
        (int8_t)(white ? WKNIGHT  : BKNIGHT)
    };
    for(int i = 0; i < 4; i++)
    {
        Move m;
        m.from  = (uint8_t)from;
        m.to    = (uint8_t)to;
        m.promo = promos[i];
        m.flags = flags;
        out.push(m);
    }
}

void gen_pseudo(const Position& p, Move_List& out)
{
    out.count = 0;
    const int me   = p.side_to_move;
    const bool wme = (me == WHITE);

    for(int sq = 0; sq < 64; sq++)
    {
        const int8_t piece = p.board[sq];
        if(piece == EMPTY || color_of(piece) != me)
            continue;

        const int x = sq % 8;
        const int y = sq / 8;
        const int type = abs_type(piece);

        switch(type)
        {
        case TYPE_PAWN:
        {
            const int dir  = wme ? -1 : 1;
            const int rank_start = wme ? 6 : 1;
            const int rank_promo = wme ? 0 : 7;

            /* forward pushes */
            if(square_ok(x, y + dir) && p.board[(y+dir)*8 + x] == EMPTY)
            {
                if(y + dir == rank_promo)
                    push_pawn_promotions(out, sq, (y+dir)*8 + x, wme, FLAG_NONE);
                else
                    push_pawn_move(out, sq, (y+dir)*8 + x, FLAG_NONE);

                /* double push */
                if(y == rank_start && p.board[(y+2*dir)*8 + x] == EMPTY)
                    push_pawn_move(out, sq, (y+2*dir)*8 + x, FLAG_DOUBLE_PUSH);
            }

            /* captures incl. en passant */
            for(int dx = -1; dx <= 1; dx += 2)
            {
                const int cx = x + dx;
                const int cy = y + dir;
                if(!square_ok(cx, cy))
                    continue;
                const int to = cy*8 + cx;
                const int8_t target = p.board[to];

                if(target != EMPTY && color_of(target) != me)
                {
                    if(cy == rank_promo)
                        push_pawn_promotions(out, sq, to, wme, FLAG_NONE);
                    else
                        push_pawn_move(out, sq, to, FLAG_NONE);
                }
                else if(to == p.ep_square && target == EMPTY)
                {
                    push_pawn_move(out, sq, to, FLAG_EP);
                }
            }
            break;
        }

        case TYPE_KNIGHT:
        {
            for(int i = 0; i < 8; i++)
            {
                const int nx = x + KNIGHT_D[i][0];
                const int ny = y + KNIGHT_D[i][1];
                if(!square_ok(nx, ny))
                    continue;
                const int to = ny*8 + nx;
                const int8_t target = p.board[to];
                if(target == EMPTY || color_of(target) != me)
                    push_pawn_move(out, sq, to, FLAG_NONE);
            }
            break;
        }

        case TYPE_BISHOP:
        case TYPE_ROOK:
        case TYPE_QUEEN:
        {
            const int ndirs = (type == TYPE_BISHOP) ? 4 : 4;
            const int* dirs = (type == TYPE_ROOK) ? &ROOK_D[0][0]
                            : (type == TYPE_BISHOP) ? &BISHOP_D[0][0]
                            : &KING_D[0][0]; /* queen uses all 8 king dirs */
            const int total = (type == TYPE_QUEEN) ? 8 : ndirs;
            for(int i = 0; i < total; i++)
            {
                int nx = x + dirs[i*2];
                int ny = y + dirs[i*2+1];
                while(square_ok(nx, ny))
                {
                    const int to = ny*8 + nx;
                    const int8_t target = p.board[to];
                    if(target == EMPTY)
                    {
                        push_pawn_move(out, sq, to, FLAG_NONE);
                    }
                    else
                    {
                        if(color_of(target) != me)
                            push_pawn_move(out, sq, to, FLAG_NONE);
                        break;
                    }
                    nx += dirs[i*2];
                    ny += dirs[i*2+1];
                }
            }
            break;
        }

        case TYPE_KING:
        {
            for(int i = 0; i < 8; i++)
            {
                const int nx = x + KING_D[i][0];
                const int ny = y + KING_D[i][1];
                if(!square_ok(nx, ny))
                    continue;
                const int to = ny*8 + nx;
                const int8_t target = p.board[to];
                if(target == EMPTY || color_of(target) != me)
                    push_pawn_move(out, sq, to, FLAG_NONE);
            }

            /* castling: rights present, path empty, rook present,
             * king not currently in check, transit square not attacked.
             * Destination square attack is filtered by legality check.   */
            const int them = enemy_of(me);
            if(wme && sq == SQ_E1)
            {
                if((p.castling & CASTLE_WK)
                   && p.board[SQ_E1+1] == EMPTY && p.board[SQ_E1+2] == EMPTY
                   && p.board[SQ_H1] == WROOK
                   && !core::is_square_attacked(p, SQ_E1, them)
                   && !core::is_square_attacked(p, SQ_E1+1, them))
                {
                    Move m = {(uint8_t)sq, (uint8_t)(SQ_E1+2), 0, FLAG_CASTLE};
                    out.push(m);
                }
                if((p.castling & CASTLE_WQ)
                   && p.board[SQ_E1-1] == EMPTY && p.board[SQ_E1-2] == EMPTY
                   && p.board[SQ_E1-3] == EMPTY
                   && p.board[SQ_A1] == WROOK
                   && !core::is_square_attacked(p, SQ_E1, them)
                   && !core::is_square_attacked(p, SQ_E1-1, them))
                {
                    Move m = {(uint8_t)sq, (uint8_t)(SQ_E1-2), 0, FLAG_CASTLE};
                    out.push(m);
                }
            }
            else if(!wme && sq == SQ_E8)
            {
                if((p.castling & CASTLE_BK)
                   && p.board[SQ_E8+1] == EMPTY && p.board[SQ_E8+2] == EMPTY
                   && p.board[SQ_H8] == BROOK
                   && !core::is_square_attacked(p, SQ_E8, them)
                   && !core::is_square_attacked(p, SQ_E8+1, them))
                {
                    Move m = {(uint8_t)sq, (uint8_t)(SQ_E8+2), 0, FLAG_CASTLE};
                    out.push(m);
                }
                if((p.castling & CASTLE_BQ)
                   && p.board[SQ_E8-1] == EMPTY && p.board[SQ_E8-2] == EMPTY
                   && p.board[SQ_E8-3] == EMPTY
                   && p.board[SQ_A8] == BROOK
                   && !core::is_square_attacked(p, SQ_E8, them)
                   && !core::is_square_attacked(p, SQ_E8-1, them))
                {
                    Move m = {(uint8_t)sq, (uint8_t)(SQ_E8-2), 0, FLAG_CASTLE};
                    out.push(m);
                }
            }
            break;
        }

        default:
            break;
        }
    }
}

/* ------------------------------------------------------------------ */
/* negamax                                                             */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/* search infrastructure                                               */
/* ------------------------------------------------------------------ */

enum TTFlag { TT_EXACT = 0, TT_LOWER = 1, TT_UPPER = 2 };

/* Transposition-table entry.  All fields are atomic so the table can be
 * shared lock-free across search threads (Lazy SMP).  Tearing is avoided
 * by per-field atomic load/store; a stale/wrong entry only costs one node. */
struct TTEntry
{
    std::atomic<uint32_t> key  {0};
    std::atomic<int>      depth{0};
    std::atomic<int>      score{0};
    std::atomic<uint8_t>  flag {0};
    std::atomic<Move>     move {Move{0, 0, 0, FLAG_NONE}};
};

constexpr int  MAX_PLY = 64;
constexpr int  TT_BITS = 20;                  /* 2^20 entries            */
constexpr int  TT_SIZE = 1 << TT_BITS;
constexpr int  TT_MASK = TT_SIZE - 1;

TTEntry tt[TT_SIZE];

/* Per-thread search state: everything that must NOT be shared between
 * worker threads.  Each thread owns its own killers / history / node
 * counter and searches from the shared transposition table. */
struct Search_State
{
    uint64_t nodes    = 0;
    Move     killers[2][MAX_PLY] = {};
    int      history[2][64][64]  = {};
};

std::chrono::steady_clock::time_point deadline_tp;
std::atomic<uint64_t>                last_search_nodes{0};

inline bool timed_out()
{
    return std::chrono::steady_clock::now() >= deadline_tp
        || core::uci_stop.load(std::memory_order_relaxed);
}

void tt_clear()
{
    for(int i = 0; i < TT_SIZE; i++)
    {
        tt[i].key.store(0, std::memory_order_relaxed);
        tt[i].depth.store(0, std::memory_order_relaxed);
        tt[i].score.store(0, std::memory_order_relaxed);
        tt[i].flag.store(0, std::memory_order_relaxed);
        tt[i].move.store(Move{0, 0, 0, FLAG_NONE}, std::memory_order_relaxed);
    }
}

inline bool moves_equal(Move a, Move b)
{
    return a.from == b.from && a.to == b.to && a.promo == b.promo && (a.flags & FLAG_EP) == (b.flags & FLAG_EP);
}

inline bool move_captures(const Position& p, Move m)
{
    if(m.flags & FLAG_EP) return true;
    if(p.board[m.to] != EMPTY) return true;
    return false;
}

inline int victim_value(const Position& p, Move m)
{
    if(m.flags & FLAG_EP)
        return MATERIAL[TYPE_PAWN];
    const int8_t t = p.board[m.to];
    return (t != EMPTY) ? MATERIAL[abs_type(t)] : 0;
}

int move_score(const Search_State& ss, const Position& p, Move m, int ply, Move hash_move, bool from_tt)
{
    const int pidx = (ply >= 0 && ply < MAX_PLY) ? ply : MAX_PLY - 1;
    if(from_tt && moves_equal(m, hash_move))
        return 100000000;
    if(moves_equal(ss.killers[0][pidx], m))
        return 90000000;
    if(moves_equal(ss.killers[1][pidx], m))
        return 80000000;
    if(move_captures(p, m))
        return 70000000 + 10 * victim_value(p, m) - MATERIAL[abs_type(p.board[m.from])];
    if(m.promo != 0)
        return 60000000 + MATERIAL[abs_type(m.promo)];
    return ss.history[p.side_to_move][m.from][m.to];
}

void order_moves(const Search_State& ss, const Position& p, Move_List& ml, int ply, Move hash_move, bool from_tt)
{
    /* insertion sort by descending score */
    for(int i = 1; i < ml.count; i++)
    {
        Move key = ml.moves[i];
        const int ks = move_score(ss, p, key, ply, hash_move, from_tt);
        int j = i - 1;
        while(j >= 0 && move_score(ss, p, ml.moves[j], ply, hash_move, from_tt) < ks)
        {
            ml.moves[j + 1] = ml.moves[j];
            j--;
        }
        ml.moves[j + 1] = key;
    }
}

/* captures (and promotions) only, for the quiescence search */
void gen_quiet_captures(const Position& p, Move_List& out)
{
    Move_List all;
    core::generate_moves(p, all);           /* fully legal moves          */
    out.count = 0;
    for(int i = 0; i < all.count; i++)
    {
        Move m = all.moves[i];
        if(move_captures(p, m) || m.promo != 0)
            out.push(m);
    }
}

int quiescence(Search_State& ss, const Position& p, int alpha, int beta, int ply)
{
    if(timed_out())
        return 0;
    if(ply >= MAX_PLY)
        return (p.side_to_move == WHITE) ? p.eval : -p.eval;
    ss.nodes++;

    /* stand-pat evaluation from incremental eval field */
    int stand = (p.side_to_move == WHITE) ? p.eval : -p.eval;
    if(stand >= beta)
        return beta;
    if(stand > alpha)
        alpha = stand;

    Move_List ml;
    gen_quiet_captures(p, ml);
    if(ml.count == 0)
        return alpha;

    Move hash_move = {0, 0, 0, FLAG_NONE};   /* no TT in qsearch           */
    order_moves(ss, p, ml, ply, hash_move, false);

    for(int i = 0; i < ml.count; i++)
    {
        Position nxt;
        core::make_move(p, nxt, ml.moves[i]);
        const int score = -quiescence(ss, nxt, -beta, -alpha, ply + 1);
        if(score >= beta)
            return beta;
        if(score > alpha)
            alpha = score;
        if(timed_out())
            break;
    }
    return alpha;
}

int negamax(Search_State& ss, const Position& p, int depth, int alpha, int beta, int ply)
{
    if((++ss.nodes & 2047) == 0 && timed_out())
        return 0;

    const uint32_t key = p.zkey;
    TTEntry* e = &tt[key & TT_MASK];

    /* TT probe (load each field atomically once) */
    const uint32_t ekey = e->key.load(std::memory_order_relaxed);
    const bool from_tt  = (ekey == key);
    Move   hash_move = {0, 0, 0, FLAG_NONE};
    int    edepth = 0, escore = 0;
    uint8_t eflag = TT_EXACT;
    if(from_tt)
    {
        edepth    = e->depth.load(std::memory_order_relaxed);
        escore    = e->score.load(std::memory_order_relaxed);
        eflag     = e->flag.load(std::memory_order_relaxed);
        hash_move = e->move.load(std::memory_order_relaxed);
        if(edepth >= depth && ply > 0)
        {
            if(eflag == TT_EXACT)                     return escore;
            if(eflag == TT_LOWER && escore > alpha)   alpha = escore;
            if(eflag == TT_UPPER && escore < beta)    beta  = escore;
            if(alpha >= beta)                         return escore;
        }
    }

    if(depth <= 0)
        return quiescence(ss, p, alpha, beta, ply);

    Move_List ml;
    core::generate_moves(p, ml);

    if(ml.count == 0)
        return core::in_check(p) ? -(CORE_MATE - ply) : 0;

    order_moves(ss, p, ml, ply, hash_move, from_tt);

    int best = -CORE_INFINITE;
    Move best_move = {0, 0, 0, FLAG_NONE};
    const int orig_alpha = alpha;

    for(int i = 0; i < ml.count; i++)
    {
        Position nxt;
        core::make_move(p, nxt, ml.moves[i]);
        const int score = -negamax(ss, nxt, depth - 1, -beta, -alpha, ply + 1);

        if(score > best)
        {
            best = score;
            best_move = ml.moves[i];
        }
        if(score > alpha)
            alpha = score;

        if(alpha >= beta)
        {
            /* beta cutoff: update killer / history heuristics for quiet moves */
            if(!move_captures(p, ml.moves[i]))
            {
                const int pidx = (ply >= 0 && ply < MAX_PLY) ? ply : MAX_PLY - 1;
                ss.killers[1][pidx] = ss.killers[0][pidx];
                ss.killers[0][pidx] = ml.moves[i];
                ss.history[p.side_to_move][ml.moves[i].from][ml.moves[i].to] += depth * depth;
            }
            break;
        }
        if(timed_out())
            break;
    }

    /* TT store (gated on deeper/newer entry), then store all fields */
    uint8_t flag;
    if(best <= orig_alpha) flag = TT_UPPER;
    else if(best >= beta)  flag = TT_LOWER;
    else                   flag = TT_EXACT;
    if(!from_tt || depth >= edepth)
    {
        e->key.store(key, std::memory_order_relaxed);
        e->depth.store(depth, std::memory_order_relaxed);
        e->score.store(best, std::memory_order_relaxed);
        e->flag.store(flag, std::memory_order_relaxed);
        e->move.store(best_move, std::memory_order_relaxed);
    }
    return best;
}

/* ------------------------------------------------------------------ */
/* iterative-deepening driver (called by each Lazy SMP worker)         */
/* ------------------------------------------------------------------ */

struct Search_Result
{
    Move     best;
    int      score;
    uint64_t nodes;
};

/* Runs iterative deepening to `max_depth` (within the shared deadline)
 * using private Search_State.  `div_offset` rotates the root move list
 * so parallel workers explore different sub-trees and share via the TT. */
Search_Result run_iterative(Search_State& ss, const Position& p, int max_depth, int div_offset)
{
    Move_List ml;
    core::generate_moves(p, ml);

    if(ml.count == 0)
        return { {0, 0, 0, FLAG_NONE}, core::in_check(p) ? -(CORE_MATE) : 0, ss.nodes };

    /* rotate the root list so threads diverge (all moves still searched) */
    if(div_offset > 0 && ml.count > 1)
    {
        div_offset %= ml.count;
        for(int r = 0; r < div_offset; r++)
        {
            Move first = ml.moves[0];
            for(int k = 0; k < ml.count - 1; k++)
                ml.moves[k] = ml.moves[k + 1];
            ml.moves[ml.count - 1] = first;
        }
    }

    /* seed ordering with the shared TT's cached root move, if any */
    const uint32_t key = p.zkey;
    TTEntry* e = &tt[key & TT_MASK];
    Move rh = {0, 0, 0, FLAG_NONE};
    bool from_tt = false;
    if(e->key.load(std::memory_order_relaxed) == key)
    {
        rh = e->move.load(std::memory_order_relaxed);
        from_tt = true;
    }
    order_moves(ss, p, ml, 0, rh, from_tt);

    Move current_best = ml.moves[0];
    int  final_score  = -CORE_INFINITE;

    for(int d = 1; d <= max_depth; d++)
    {
        const int alpha = -CORE_INFINITE;
        const int beta  =  CORE_INFINITE;
        int   dbest_score = -CORE_INFINITE;
        Move  dbest = ml.moves[0];
        bool  aborted = false;

        for(int i = 0; i < ml.count; i++)
        {
            Position nxt;
            core::make_move(p, nxt, ml.moves[i]);
            const int score = -negamax(ss, nxt, d - 1, -beta, -alpha, 1);
            if(score > dbest_score)
            {
                dbest_score = score;
                dbest = ml.moves[i];
            }
            if(timed_out())
            {
                aborted = true;
                break;
            }
        }

        if(aborted)
            break;

        current_best = dbest;
        final_score  = dbest_score;

        /* promote the best move to the front for the next iteration */
        for(int i = 0; i < ml.count; i++)
            if(moves_equal(ml.moves[i], dbest))
            {
                Move tmp = ml.moves[0];
                ml.moves[0] = ml.moves[i];
                ml.moves[i] = tmp;
                break;
            }
    }

    return { current_best, (final_score <= -CORE_INFINITE) ? 0 : final_score, ss.nodes };
}

/* Lazy SMP harvest: pick the root move with the most votes, breaking
 * ties by the highest total score. */
Search_Result harvest_results(std::vector<Search_Result>& results)
{
    Search_Result best = results[0];
    for(std::size_t i = 1; i < results.size(); i++)
    {
        const Search_Result& r = results[i];
        int cv = 0, rv = 0;
        for(std::size_t j = 0; j < results.size(); j++)
        {
            if(moves_equal(results[j].best, best.best)) cv++;
            if(moves_equal(results[j].best, r.best))    rv++;
        }
        if(rv > cv
          || (rv == cv && r.score > best.score))
            best = r;
    }
    return best;
}

} /* anonymous namespace */

namespace core {

std::atomic<bool> uci_stop{false};

/* ------------------------------------------------------------------ */
/* setup                                                               */
/* ------------------------------------------------------------------ */

void position_init(Position& p)
{
    fen_load("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", p);
}

void zobrist_init()
{
    /* Deterministic PRNG so builds are reproducible. */
    uint64_t st = 0x9E3779B97F4A7C15ull;
    auto rnd = [&st]()
    {
        st ^= st << 13; st ^= st >> 7; st ^= st << 17;
        return (uint32_t)(st >> 32);
    };
    for(int i = 0; i < 12; i++)
        for(int j = 0; j < 64; j++)
            ZOB_PIECE[i][j] = rnd();
    for(int i = 0; i < 16; i++)
        ZOB_CASTLING[i] = rnd();
    for(int i = 0; i < 65; i++)
        ZOB_EP[i] = rnd();
    ZOB_SIDE = rnd();
}

bool fen_load(const char* fen, Position& p)
{
    for(int i = 0; i < 64; i++)
        p.board[i] = EMPTY;
    p.castling       = 0;
    p.ep_square      = -1;
    p.halfmove_clock = 0;
    p.side_to_move   = WHITE;
    p.eval           = 0;
    p.zkey           = 0;
    p.king_square[WHITE] = -1;
    p.king_square[BLACK] = -1;

    if(fen == nullptr)
        return false;

    static const char* order = "pnbrqk";
    int sq = 0;
    const char* c = fen;

    /* 1. board */
    while(*c && *c != ' ')
    {
        if(*c == '/')
        {
            c++;
            continue;
        }
        if(*c >= '1' && *c <= '8')
        {
            sq += *c - '0';
            c++;
            continue;
        }
        int8_t code = EMPTY;
        if(*c >= 'A' && *c <= 'Z')
        {
            const char* hit = strchr(order, *c + ('a' - 'A'));
            if(!hit) return false;
            code = (int8_t)(1 + (hit - order));
        }
        else if(*c >= 'a' && *c <= 'z')
        {
            const char* hit = strchr(order, *c);
            if(!hit) return false;
            code = (int8_t)-(1 + (hit - order));
        }
        else
            return false;

        if(sq > 63) return false;
        p.board[sq++] = code;
        c++;
    }
    if(sq != 64) return false;
    while(*c == ' ') c++;

    /* 2. side to move */
    if(*c == 'w')      p.side_to_move = WHITE;
    else if(*c == 'b') p.side_to_move = BLACK;
    else return false;
    c++;
    while(*c == ' ') c++;

    /* 3. castling */
    if(*c == '-')
        c++;
    else
    {
        while(*c && *c != ' ')
        {
            switch(*c)
            {
            case 'K': p.castling |= CASTLE_WK; break;
            case 'Q': p.castling |= CASTLE_WQ; break;
            case 'k': p.castling |= CASTLE_BK; break;
            case 'q': p.castling |= CASTLE_BQ; break;
            default: return false;
            }
            c++;
        }
    }
    while(*c == ' ') c++;

    /* 4. en passant */
    if(*c == '-')
        c++;
    else if(c[0] >= 'a' && c[0] <= 'h' && c[1] >= '1' && c[1] <= '8')
    {
        const int x = c[0] - 'a';
        const int y = 8 - (c[1] - '0');
        p.ep_square = (int8_t)(y*8 + x);
        c += 2;
    }
    else
        return false;
    while(*c == ' ') c++;

    /* 5. halfmove clock (optional) */
    if(*c)
        p.halfmove_clock = atoi(c);
    /* fullmove number ignored */

    recompute_meta(p, p);   /* fill incremental eval/zkey/king squares */
    return true;
}

/* ------------------------------------------------------------------ */
/* queries                                                             */
/* ------------------------------------------------------------------ */

bool is_square_attacked(const Position& p, int sq, int by)
{
    const int x = sq % 8;
    const int y = sq / 8;

    /* pawns: white attacks toward lower y (rank 8 at y=0) */
    if(by == WHITE)
    {
        if(square_ok(x-1, y+1) && p.board[(y+1)*8 + x-1] == WPAWN) return true;
        if(square_ok(x+1, y+1) && p.board[(y+1)*8 + x+1] == WPAWN) return true;
    }
    else
    {
        if(square_ok(x-1, y-1) && p.board[(y-1)*8 + x-1] == BPAWN) return true;
        if(square_ok(x+1, y-1) && p.board[(y-1)*8 + x+1] == BPAWN) return true;
    }

    /* knights */
    const int8_t kn = (by == WHITE) ? WKNIGHT : BKNIGHT;
    for(int i = 0; i < 8; i++)
    {
        const int nx = x + KNIGHT_D[i][0];
        const int ny = y + KNIGHT_D[i][1];
        if(square_ok(nx, ny) && p.board[ny*8 + nx] == kn)
            return true;
    }

    /* king */
    const int8_t kg = (by == WHITE) ? WKING : BKING;
    for(int i = 0; i < 8; i++)
    {
        const int nx = x + KING_D[i][0];
        const int ny = y + KING_D[i][1];
        if(square_ok(nx, ny) && p.board[ny*8 + nx] == kg)
            return true;
    }

    /* sliding: rook/queen on straight rays */
    const int8_t rk = (by == WHITE) ? WROOK  : BROOK;
    const int8_t qu = (by == WHITE) ? WQUEEN : BQUEEN;
    for(int i = 0; i < 4; i++)
    {
        int nx = x + ROOK_D[i][0];
        int ny = y + ROOK_D[i][1];
        while(square_ok(nx, ny))
        {
            const int8_t t = p.board[ny*8 + nx];
            if(t != EMPTY)
            {
                if(t == rk || t == qu) return true;
                break;
            }
            nx += ROOK_D[i][0];
            ny += ROOK_D[i][1];
        }
    }

    /* sliding: bishop/queen on diagonal rays */
    const int8_t bi = (by == WHITE) ? WBISHOP : BBISHOP;
    for(int i = 0; i < 4; i++)
    {
        int nx = x + BISHOP_D[i][0];
        int ny = y + BISHOP_D[i][1];
        while(square_ok(nx, ny))
        {
            const int8_t t = p.board[ny*8 + nx];
            if(t != EMPTY)
            {
                if(t == bi || t == qu) return true;
                break;
            }
            nx += BISHOP_D[i][0];
            ny += BISHOP_D[i][1];
        }
    }

    return false;
}

bool in_check_color(const Position& p, int color)
{
    int ksq = p.king_square[color];
    if(ksq < 0)
        ksq = find_king(p, color);          /* fallback (defensive) */
    if(ksq < 0)
        return false;
    return is_square_attacked(p, ksq, enemy_of(color));
}

bool in_check(const Position& p)
{
    return in_check_color(p, p.side_to_move);
}

int generate_moves(const Position& p, Move_List& out)
{
    gen_pseudo(p, out);

    const int mover = p.side_to_move;

    /* legality filter: own king may not hang after the move */
    int kept = 0;
    for(int i = 0; i < out.count; i++)
    {
        Position scratch;
        make_move(p, scratch, out.moves[i]);
        if(!in_check_color(scratch, mover))
            out.moves[kept++] = out.moves[i];
    }
    out.count = kept;
    return kept;
}

/* ------------------------------------------------------------------ */
/* applying moves                                                      */
/* ------------------------------------------------------------------ */

void make_move(const Position& src, Position& dst, Move m)
{
    dst = src;

    const int8_t piece    = src.board[m.from];
    const int8_t mover    = color_of(piece);
    const int8_t captured = src.board[m.to];

    /* ---- incremental eval (white-perspective) / zkey deltas ---- */
    int      de = 0;
    uint32_t dz = 0;

    de -= sq_piece_value(piece, m.from);
    dz ^= ZOB_PIECE[zid(piece)][m.from];

    if(m.flags & FLAG_EP)
    {
        const int capsq  = (mover == WHITE) ? m.to + 8 : m.to - 8;
        const int8_t cap = src.board[capsq];
        dst.board[capsq] = EMPTY;
        de -= sq_piece_value(cap, capsq);
        dz ^= ZOB_PIECE[zid(cap)][capsq];
    }
    else if(!(m.flags & FLAG_CASTLE) && captured != EMPTY)
    {
        de -= sq_piece_value(captured, m.to);
        dz ^= ZOB_PIECE[zid(captured)][m.to];
    }

    const int8_t placed = m.promo ? m.promo : piece;
    dst.board[m.to]   = placed;
    dst.board[m.from] = EMPTY;
    de += sq_piece_value(placed, m.to);
    dz ^= ZOB_PIECE[zid(placed)][m.to];

    if(abs_type(piece) == TYPE_KING)
        dst.king_square[mover] = (int8_t)m.to;

    if(m.flags & FLAG_CASTLE)
    {
        if(m.to > m.from)   /* kingside: rook h-file -> f-file */
        {
            const int rf = m.from + 3;
            const int rt = m.from + 1;
            dst.board[rt]  = src.board[rf];
            dst.board[rf]  = EMPTY;
            de -= sq_piece_value(src.board[rf], rf);
            dz ^= ZOB_PIECE[zid(src.board[rf])][rf];
            de += sq_piece_value(src.board[rf], rt);
            dz ^= ZOB_PIECE[zid(src.board[rf])][rt];
        }
        else                /* queenside: rook a-file -> d-file */
        {
            const int rf = m.from - 4;
            const int rt = m.from - 1;
            dst.board[rt]  = src.board[rf];
            dst.board[rf]  = EMPTY;
            de -= sq_piece_value(src.board[rf], rf);
            dz ^= ZOB_PIECE[zid(src.board[rf])][rf];
            de += sq_piece_value(src.board[rf], rt);
            dz ^= ZOB_PIECE[zid(src.board[rf])][rt];
        }
    }

    dst.eval += de;
    dst.zkey ^= dz;

    const uint8_t old_castle = dst.castling;
    dst.castling &= CASTLE_MASK[m.from];
    dst.castling &= CASTLE_MASK[m.to];
    dst.zkey ^= ZOB_CASTLING[old_castle];
    dst.zkey ^= ZOB_CASTLING[dst.castling];

    const int8_t old_ep = dst.ep_square;   /* == src.ep_square */
    dst.ep_square = (m.flags & FLAG_DOUBLE_PUSH)
                  ? (int8_t)((m.from + m.to) / 2)
                  : -1;
    dst.zkey ^= ZOB_EP[old_ep >= 0 ? old_ep : 64];
    dst.zkey ^= ZOB_EP[dst.ep_square >= 0 ? dst.ep_square : 64];

    if(abs_type(piece) == TYPE_PAWN || captured != EMPTY)
        dst.halfmove_clock = 0;
    else
        dst.halfmove_clock = src.halfmove_clock + 1;

    dst.side_to_move = enemy_of(src.side_to_move);
    dst.zkey ^= ZOB_SIDE;
}

/* ------------------------------------------------------------------ */
/* evaluation                                                          */
/* ------------------------------------------------------------------ */

int evaluate(const Position& p)
{
    int score = 0;
    for(int sq = 0; sq < 64; sq++)
    {
        const int8_t piece = p.board[sq];
        if(piece == EMPTY)
            continue;

        const int type = abs_type(piece);
        int value = MATERIAL[type];

        switch(type)
        {
        case TYPE_PAWN:   value += PST_PAWN[sq];   break;
        case TYPE_KNIGHT: value += PST_KNIGHT[sq]; break;
        case TYPE_BISHOP: value += PST_BISHOP[sq]; break;
        case TYPE_ROOK:   value += PST_ROOK[sq];   break;
        case TYPE_QUEEN:  value += PST_QUEEN[sq];  break;
        case TYPE_KING:   value += PST_KING[sq];   break;
        }

        score += (piece > 0) ? value : -value;
    }
    return (p.side_to_move == WHITE) ? score : -score;
}

/* ------------------------------------------------------------------ */
/* perft                                                               */
/* ------------------------------------------------------------------ */

int perft(const Position& p, int depth)
{
    if(depth <= 0)
        return 1;

    Move_List ml;
    generate_moves(p, ml);

    if(depth == 1)
        return ml.count;

    int nodes = 0;
    for(int i = 0; i < ml.count; i++)
    {
        Position next;
        make_move(p, next, ml.moves[i]);
        nodes += perft(next, depth - 1);
    }
    return nodes;
}

/* ------------------------------------------------------------------ */
/* search                                                              */
/* ------------------------------------------------------------------ */

int search_root(const Position& p, int depth, Move& best_out, int time_alloc_ms, int threads)
{
    if(threads < 1)
        threads = 1;

    deadline_tp = std::chrono::steady_clock::now()
                + std::chrono::milliseconds(time_alloc_ms < 0 ? 0 : time_alloc_ms);
    last_search_nodes.store(0, std::memory_order_relaxed);

    best_out = {0, 0, 0, FLAG_NONE};
    Move_List probe;
    generate_moves(p, probe);
    if(probe.count == 0)
        return in_check(p) ? -(CORE_MATE) : 0;

    if(threads == 1)
    {
        /* single-threaded path: no spawning overhead */
        Search_State ss;
        Search_Result r = run_iterative(ss, p, depth, 0);
        last_search_nodes.store(r.nodes, std::memory_order_relaxed);
        best_out = r.best;
        return r.score;
    }

    /* Lazy SMP: N threads search independently (own killers/history/nodes)
     * from the shared transposition table, then we harvest the consensus. */
    std::vector<Search_Result> results(threads);
    std::vector<std::thread>   pool;
    pool.reserve(threads);

    for(int t = 0; t < threads; t++)
    {
        pool.emplace_back([&p, depth, t, &results]()
        {
            Search_State ss;
            results[t] = run_iterative(ss, p, depth, t);   /* distinct element per thread */
        });
    }
    for(auto& th : pool)
        th.join();

    uint64_t total = 0;
    for(auto& r : results)
        total += r.nodes;
    last_search_nodes.store(total, std::memory_order_relaxed);

    Search_Result final = harvest_results(results);
    best_out = final.best;
    return final.score;
}

void search_clear_tables()
{
    tt_clear();
}

void search_clear_uci_stop()
{
    uci_stop.store(false, std::memory_order_relaxed);
}

uint64_t search_nodes_last()
{
    return last_search_nodes.load(std::memory_order_relaxed);
}

/* ------------------------------------------------------------------ */
/* debug helpers                                                       */
/* ------------------------------------------------------------------ */

const char* square_name(int sq)
{
    static char buf[3];
    buf[0] = (char)('a' + (sq % 8));
    buf[1] = (char)('8' - (sq / 8));
    buf[2] = '\0';
    return buf;
}

uint32_t zkey_recompute(const Position& p)
{
    Position tmp;
    recompute_meta(p, tmp);
    return tmp.zkey;
}

void move_print(Move m)
{
    std::printf("%s", square_name(m.from));
    std::printf("%s", square_name(m.to));
    if(m.promo)
    {
        const char* letters = "??nbrq";
        std::printf("%c", letters[abs_type(m.promo)]);
    }
}

void position_print(const Position& p)
{
    for(int y = 0; y < 8; y++)
    {
        std::printf("%d ", 8 - y);
        for(int x = 0; x < 8; x++)
        {
            const int8_t pc = p.board[y*8 + x];
            if(pc == EMPTY)
                std::printf(" .");
            else
            {
                const char* w = " PNBRQK";
                const char* b = " pnbrqk";
                std::printf(" %c", pc > 0 ? w[pc] : b[-pc]);
            }
        }
        std::printf("\n");
    }
    std::printf("  a b c d e f g h\n");
    std::printf("side: %s  castling: %c%c%c%c  ep: %s\n",
                p.side_to_move == WHITE ? "white" : "black",
                (p.castling & CASTLE_WK) ? 'K' : '-',
                (p.castling & CASTLE_WQ) ? 'Q' : '-',
                (p.castling & CASTLE_BK) ? 'k' : '-',
                (p.castling & CASTLE_BQ) ? 'q' : '-',
                p.ep_square >= 0 ? square_name(p.ep_square) : "-");
}

} /* namespace core */
