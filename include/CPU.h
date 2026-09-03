#ifndef CPU_H
#define CPU_H

#include "structs.h"
#include "chess_core.h"

/** \class CPU
 *
 *  \brief Thin wrapper around the engine core search. Operates on
 *         Game::pos only; knows nothing about sprites.
 */
class CPU
{
    public:
        CPU();
        virtual ~CPU();
        CPU(const CPU& other) = delete;
        CPU& operator=(const CPU& other) = delete;

        /** \brief reads the current position from the game */
        void engine_start();

        /** \brief runs alpha-beta search; stores best move + score */
        void full_eval();

        /** \brief applies the found move to the game (if any) */
        void play_best_move();

        void eval_render();

    protected:

    private:
        int   max_depth;
        int   search_time_ms;   /* per-move search budget               */
        int   search_threads;   /* parallel (Lazy SMP) worker threads   */
        double evalulation;
        Move  best_move;
        bool  has_move;
        Color color_evaluated;
};

#endif // CPU_H
