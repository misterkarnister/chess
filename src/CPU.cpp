#include "CPU.h"
#include "object_init.h"
#include <SDL3/SDL.h>
#include <sstream>
#include <thread>

CPU::CPU()
{
    max_depth = 40;              /* soft ceiling; iterative deepening governs            */
    search_time_ms = 1500;       /* search budget per move (tune for weaker/stronger play) */
    const unsigned hw = std::thread::hardware_concurrency();
    search_threads = (hw > 0) ? (int)hw : 1;   /* use all available cores by default      */
    evalulation = 0.0;
    best_move = {0, 0, 0, FLAG_NONE};
    has_move = false;
    color_evaluated = WHITE;

    core::zobrist_init();        /* must run once before any search                        */
}

CPU::~CPU() = default;

void CPU::engine_start()
{
    color_evaluated = (Color)g1.pos.side_to_move;
}

void CPU::full_eval()
{
    Move_List ml;
    core::generate_moves(g1.pos, ml);

    if(ml.count == 0)
    {
        has_move = false;
        best_move = {0, 0, 0, FLAG_NONE};
        evalulation = core::in_check(g1.pos) ? (double)-CORE_MATE : 0.0;
        return;
    }

    const int score = core::search_root(g1.pos, max_depth, best_move, search_time_ms, search_threads);
    evalulation = score / 100.0;
    has_move = true;
}

void CPU::play_best_move()
{
    if(has_move)
        g1.apply_move(best_move);
    g1.awaiting_cpu = false;
}

void CPU::eval_render()
{
    std::stringstream ss;
    ss.str("");
    ss<<"Eval:"<<evalulation;
    Text temp;
    temp.text_load(ss.str().c_str(), t1.font_ui_get(), t1.font_ui_color_get());
    temp.render(
                t1.window_width_get() * 0.75 - temp.width_get() * 0.5,
                t1.window_height_get() * 0.15 - temp.height_get() * 0.5
                );
}
