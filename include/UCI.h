#ifndef UCI_H
#define UCI_H

#include "chess_core.h"
#include <string>
#include <vector>
#include <thread>
#include <atomic>

/** \class UCI
 *
 *  \brief Universal Chess Interface protocol handler.
 *         Reads commands from stdin, writes responses to stdout.
 *         Engine mode: run with --uci flag.
 */
class UCI
{
    public:
        UCI();
        ~UCI();
        UCI(const UCI&) = delete;
        UCI& operator=(const UCI&) = delete;

        /** \brief Main loop: blocks until 'quit' command. */
        void loop();

    private:
        Position pos;

        std::thread       search_thread;
        std::atomic<bool> stop_flag{false};
        std::atomic<bool> searching{false};
        Move              result_best{0, 0, 0, FLAG_NONE};
        int               result_score{0};
        uint64_t          result_nodes{0};
        int               result_depth{0};

        /* engine options */
        int  opt_threads{1};
        int  opt_hash_mb{16};
        int  opt_multi_pv{1};

        /* command handlers */
        void cmd_uci();
        void cmd_isready();
        void cmd_ucinewgame();
        void cmd_setoption(const std::string& line);
        void cmd_position(const std::string& line);
        void cmd_go(const std::string& line);
        void cmd_stop();

        /* search thread entry point */
        struct Search_Limits
        {
            bool infinite;
            int  depth;
            int  nodes;
            int  movetime_ms;
            int  wtime, btime, winc, binc, movestogo;
        };
        void search_thread_fn(Search_Limits lim);

        /* time allocation from go command limits */
        int calc_time_alloc_impl(const Position& p, const Search_Limits& lim) const;

        /* safely join a finished search thread */
        void join_search();

        /* helpers */
        Move parse_uci_move(const std::string& s) const;
        std::string move_to_uci(Move m) const;
        void emit_bestmove();

        static std::string trim(const std::string& s);
        static std::vector<std::string> split(const std::string& s);
};

#endif /* UCI_H */
