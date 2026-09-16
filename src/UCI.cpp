#include "UCI.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>

/* ------------------------------------------------------------------ */
/* helpers                                                             */
/* ------------------------------------------------------------------ */

std::string UCI::trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<std::string> UCI::split(const std::string& s)
{
    std::vector<std::string> tokens;
    std::istringstream iss(s);
    std::string tok;
    while(iss >> tok)
        tokens.push_back(tok);
    return tokens;
}

std::string UCI::move_to_uci(Move m) const
{
    if(m.from == 0 && m.to == 0 && m.promo == 0)
        return "0000";
    std::string s;
    s += core::square_name(m.from);
    s += core::square_name(m.to);
    if(m.promo)
    {
        const char letters[] = "??nbrq";
        s += letters[std::abs(m.promo)];
    }
    return s;
}

Move UCI::parse_uci_move(const std::string& s) const
{
    if(s.length() < 4 || s.length() > 5)
        return {0, 0, 0, FLAG_NONE};

    const int from_x = s[0] - 'a';
    const int from_y = 8 - (s[1] - '0');
    const int to_x   = s[2] - 'a';
    const int to_y   = 8 - (s[3] - '0');

    if(from_x < 0 || from_x > 7 || from_y < 0 || from_y > 7) return {0, 0, 0, FLAG_NONE};
    if(to_x   < 0 || to_x   > 7 || to_y   < 0 || to_y   > 7) return {0, 0, 0, FLAG_NONE};

    const uint8_t from = (uint8_t)(from_y * 8 + from_x);
    const uint8_t to   = (uint8_t)(to_y   * 8 + to_x);

    int8_t promo = 0;
    if(s.length() == 5)
    {
        switch(s[4])
        {
        case 'n': promo = (pos.side_to_move == WHITE) ? WKNIGHT : BKNIGHT; break;
        case 'b': promo = (pos.side_to_move == WHITE) ? WBISHOP : BBISHOP; break;
        case 'r': promo = (pos.side_to_move == WHITE) ? WROOK   : BROOK;   break;
        case 'q': promo = (pos.side_to_move == WHITE) ? WQUEEN  : BQUEEN;  break;
        default: return {0, 0, 0, FLAG_NONE};
        }
    }

    Move_List ml;
    core::generate_moves(pos, ml);
    for(int i = 0; i < ml.count; i++)
    {
        if(ml.moves[i].from == from && ml.moves[i].to == to && ml.moves[i].promo == promo)
            return ml.moves[i];
    }
    return {0, 0, 0, FLAG_NONE};
}

void UCI::emit_bestmove()
{
    std::cout << "bestmove " << move_to_uci(result_best) << std::endl;
}

/* ------------------------------------------------------------------ */
/* constructor / destructor                                            */
/* ------------------------------------------------------------------ */

UCI::UCI()
{
    core::zobrist_init();
    core::position_init(pos);
}

UCI::~UCI()
{
    core::uci_stop.store(true, std::memory_order_relaxed);
    join_search();
}

/* ------------------------------------------------------------------ */
/* main loop                                                           */
/* ------------------------------------------------------------------ */

void UCI::loop()
{
    std::string line;
    while(std::getline(std::cin, line))
    {
        line = trim(line);
        if(line.empty()) continue;

        std::vector<std::string> tokens = split(line);
        const std::string& cmd = tokens[0];

        if(cmd == "uci")
            cmd_uci();
        else if(cmd == "isready")
            cmd_isready();
        else if(cmd == "ucinewgame")
            cmd_ucinewgame();
        else if(cmd == "setoption")
            cmd_setoption(line);
        else if(cmd == "position")
            cmd_position(line);
        else if(cmd == "go")
            cmd_go(line);
        else if(cmd == "stop")
            cmd_stop();
        else if(cmd == "quit")
            break;
        else if(cmd == "d")
            core::position_print(pos);
    }

    /* wait for any running search to finish cleanly */
    core::uci_stop.store(true, std::memory_order_relaxed);
    join_search();
}

void UCI::join_search()
{
    if(search_thread.joinable())
        search_thread.join();
    searching.store(false, std::memory_order_relaxed);
}

/* ------------------------------------------------------------------ */
/* command handlers                                                    */
/* ------------------------------------------------------------------ */

void UCI::cmd_uci()
{
    std::cout << "id name TemperChess" << std::endl;
    std::cout << "id author Temper" << std::endl;
    std::cout << "option name Threads type spin default 1 min 1 max 1024" << std::endl;
    std::cout << "option name Hash type spin default 16 min 1 max 33554432" << std::endl;
    std::cout << "option name MultiPV type spin default 1 min 1 max 218" << std::endl;
    std::cout << "uciok" << std::endl;
}

void UCI::cmd_isready()
{
    std::cout << "readyok" << std::endl;
}

void UCI::cmd_ucinewgame()
{
    core::search_clear_tables();
}

void UCI::cmd_setoption(const std::string& line)
{
    /* format: setoption name <id> [value <x>] */
    auto tokens = split(line);
    std::string name;
    std::string value;
    bool reading_value = false;
    for(size_t i = 1; i < tokens.size(); i++)
    {
        if(tokens[i] == "name" && !reading_value)
        {
            /* accumulate name tokens until "value" or end */
            name.clear();
            while(i + 1 < tokens.size() && tokens[i + 1] != "value")
            {
                if(!name.empty()) name += " ";
                name += tokens[++i];
            }
        }
        else if(tokens[i] == "value")
        {
            reading_value = true;
            value.clear();
            while(i + 1 < tokens.size())
            {
                if(!value.empty()) value += " ";
                value += tokens[++i];
            }
        }
    }

    if(name == "Threads" && !value.empty())
        opt_threads = std::max(1, std::atoi(value.c_str()));
    else if(name == "Hash" && !value.empty())
        opt_hash_mb = std::max(1, std::atoi(value.c_str()));
    else if(name == "MultiPV" && !value.empty())
        opt_multi_pv = std::max(1, std::min(218, std::atoi(value.c_str())));
    else if(name == "Clear Hash")
        core::search_clear_tables();
}

void UCI::cmd_position(const std::string& line)
{
    auto tokens = split(line);
    size_t idx = 1;

    if(idx >= tokens.size()) return;

    if(tokens[idx] == "startpos")
    {
        core::position_init(pos);
        idx++;
    }
    else if(tokens[idx] == "fen")
    {
        idx++;
        std::string fen;
        while(idx < tokens.size() && tokens[idx] != "moves")
        {
            if(!fen.empty()) fen += " ";
            fen += tokens[idx];
            idx++;
        }
        if(!core::fen_load(fen.c_str(), pos))
            core::position_init(pos);
    }
    else
    {
        return;
    }

    if(idx < tokens.size() && tokens[idx] == "moves")
    {
        idx++;
        while(idx < tokens.size())
        {
            Move m = parse_uci_move(tokens[idx]);
            if(m.from == 0 && m.to == 0 && m.promo == 0)
            {
                /* illegal move -- stop replaying */
                break;
            }
            Position nxt;
            core::make_move(pos, nxt, m);
            pos = nxt;
            idx++;
        }
    }
}

void UCI::cmd_go(const std::string& line)
{
    auto tokens = split(line);

    Search_Limits lim;
    lim.infinite    = false;
    lim.depth       = 0;
    lim.nodes       = 0;
    lim.movetime_ms = 0;
    lim.wtime       = 0;
    lim.btime       = 0;
    lim.winc        = 0;
    lim.binc        = 0;
    lim.movestogo   = 0;

    for(size_t i = 1; i < tokens.size(); i++)
    {
        if(tokens[i] == "depth"      && i + 1 < tokens.size()) lim.depth       = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "nodes"     && i + 1 < tokens.size()) lim.nodes       = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "movetime"  && i + 1 < tokens.size()) lim.movetime_ms = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "wtime"     && i + 1 < tokens.size()) lim.wtime       = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "btime"     && i + 1 < tokens.size()) lim.btime       = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "winc"      && i + 1 < tokens.size()) lim.winc        = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "binc"      && i + 1 < tokens.size()) lim.binc        = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "movestogo" && i + 1 < tokens.size()) lim.movestogo   = std::atoi(tokens[++i].c_str());
        else if(tokens[i] == "infinite")                          lim.infinite    = true;
        else if(tokens[i] == "ponder")                            lim.infinite    = true;
    }

    /* stop and join any previous search before starting a new one */
    core::uci_stop.store(true, std::memory_order_relaxed);
    join_search();

    core::uci_stop.store(false, std::memory_order_relaxed);
    stop_flag.store(false, std::memory_order_relaxed);
    searching.store(true, std::memory_order_relaxed);

    search_thread = std::thread(&UCI::search_thread_fn, this, lim);
}

void UCI::cmd_stop()
{
    core::uci_stop.store(true, std::memory_order_relaxed);
    stop_flag.store(true, std::memory_order_relaxed);

    join_search();
}

/* ------------------------------------------------------------------ */
/* time management                                                     */
/* ------------------------------------------------------------------ */

int UCI::calc_time_alloc_impl(const Position& pos, const Search_Limits& lim) const
{
    if(lim.movetime_ms > 0)
        return lim.movetime_ms;

    const int my_time = (pos.side_to_move == WHITE) ? lim.wtime : lim.btime;
    const int my_inc  = (pos.side_to_move == WHITE) ? lim.winc  : lim.binc;

    if(my_time <= 0)
        return lim.infinite ? 60000 : 1000;

    int mtg = lim.movestogo > 0 ? lim.movestogo : 30;
    int alloc = my_time / mtg + my_inc / 2;

    /* clamp: at least 50ms, at most 80% of remaining time */
    if(alloc < 50)         alloc = 50;
    if(alloc > my_time * 8 / 10) alloc = my_time * 8 / 10;

    return alloc;
}

/* ------------------------------------------------------------------ */
/* search thread                                                       */
/* ------------------------------------------------------------------ */

void UCI::search_thread_fn(Search_Limits lim)
{
    const int max_depth = (lim.depth > 0 && !lim.infinite) ? lim.depth : 245;
    const int time_ms   = lim.infinite ? 60000 : calc_time_alloc_impl(pos, lim);

    auto t_start = std::chrono::steady_clock::now();

    Position work = pos;
    Move best = {0, 0, 0, FLAG_NONE};
    int  best_score = 0;
    uint64_t total_nodes = 0;

    /* quick check: are there any legal moves at all? */
    Move_List probe;
    core::generate_moves(work, probe);
    if(probe.count == 0)
    {
        int score = core::in_check(work) ? -(CORE_MATE) : 0;
        bool is_mate = (score <= -(CORE_MATE - 500)) || (score >= (CORE_MATE - 500));
        std::cout << "info depth 0 seldepth 0 nodes 0 nps 0 time 0 multipv 1 score "
                  << (is_mate ? "mate " : "cp ") << (is_mate ? 0 : score)
                  << " pv 0000" << std::endl;
        std::cout << "bestmove 0000" << std::endl;
        result_best  = {0, 0, 0, FLAG_NONE};
        result_score = score;
        result_nodes = 0;
        searching.store(false, std::memory_order_relaxed);
        return;
    }

    for(int d = 1; d <= max_depth; d++)
    {
        auto now = std::chrono::steady_clock::now();
        int elapsed = (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - t_start).count();
        int remaining = time_ms - elapsed;
        if(remaining < 10)
            break;   /* time budget exhausted (also covers infinite-less movetime) */

        core::uci_stop.store(false, std::memory_order_relaxed);

        Move cur_best = {0, 0, 0, FLAG_NONE};
        int cur_score = core::search_root(work, d, cur_best, remaining, opt_threads);

        uint64_t nodes = core::search_nodes_last();
        total_nodes += nodes;

        now = std::chrono::steady_clock::now();
        elapsed = (int)std::chrono::duration_cast<std::chrono::milliseconds>(now - t_start).count();

        if(cur_best.from != 0 || cur_best.to != 0)
        {
            best = cur_best;
            best_score = cur_score;
        }

        bool is_mate = (best_score <= -(CORE_MATE - 500)) || (best_score >= (CORE_MATE - 500));
        int mate_plies = 0;
        if(is_mate)
        {
            if(best_score > 0)
                mate_plies = (CORE_MATE - best_score + 1) / 2;
            else
                mate_plies = -((CORE_MATE + best_score) / 2);
        }

        std::cout << "info depth " << d
                  << " seldepth " << d
                  << " nodes " << nodes
                  << " nps " << (elapsed > 0 ? (nodes * 1000 / (uint64_t)elapsed) : 0)
                  << " time " << elapsed
                  << " multipv 1 score "
                  << (is_mate ? "mate " : "cp ")
                  << (is_mate ? mate_plies : best_score)
                  << " pv " << move_to_uci(best)
                  << std::endl;

        result_depth = d;   /* remember last completed depth for final info */

        /* stop conditions */
        if(core::uci_stop.load(std::memory_order_relaxed))
            break;
        if(is_mate)
            break;
        if(elapsed >= time_ms)
            break;
        if(lim.nodes > 0 && (int)total_nodes >= lim.nodes)
            break;
        if(!lim.infinite && d >= max_depth)
            break;
    }

    result_best  = best;
    result_score = best_score;
    result_nodes = total_nodes;

    auto t_end = std::chrono::steady_clock::now();
    int total_time = (int)std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
    bool is_mate_final = (best_score <= -(CORE_MATE - 500)) || (best_score >= (CORE_MATE - 500));
    int mate_plies_final = 0;
    if(is_mate_final)
    {
        if(best_score > 0)
            mate_plies_final = (CORE_MATE - best_score + 1) / 2;
        else
            mate_plies_final = -((CORE_MATE + best_score) / 2);
    }

    std::cout << "info depth " << result_depth
              << " seldepth " << result_depth
              << " nodes " << result_nodes
              << " nps " << (total_time > 0 ? (result_nodes * 1000 / (uint64_t)total_time) : 0)
              << " time " << total_time
              << " multipv 1 score "
              << (is_mate_final ? "mate " : "cp ")
              << (is_mate_final ? mate_plies_final : result_score)
              << " pv " << move_to_uci(result_best)
              << std::endl;

    searching.store(false, std::memory_order_relaxed);
    core::uci_stop.store(false, std::memory_order_relaxed);

    emit_bestmove();
}
