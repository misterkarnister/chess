
#include "../include/object_init.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <cstring>

King         king_white,         king_black;
Queen        queen_white,        queen_black;
Bishop       bishops_white[2],   bishops_black[2];
Knight       knights_white[2],   knights_black[2];
Rook         rooks_white[2],     rooks_black[2];
Pawn         pawns_white[8],     pawns_black[8];
Queen        promo_queen_white[8],   promo_queen_black[8];
Rook         promo_rook_white[8],    promo_rook_black[8];
Bishop       promo_bishop_white[8],  promo_bishop_black[8];
Knight       promo_knight_white[8],  promo_knight_black[8];
Piece        promo_display[4];
CPU          c1;
Text         letters_text[8],    digits_text[8],     black_won_text,      white_won_text,      draw_text,       pvp_text,       cpu_text;
Object       pawn_white_obj, rook_white_obj, knight_white_obj, bishop_white_obj, queen_white_obj, king_white_obj,
             pawn_black_obj, rook_black_obj, knight_black_obj, bishop_black_obj, queen_black_obj, king_black_obj;
Mouse        m1;
Game         g1;
Technical    t1;

/* ------------------------------------------------------------------ */
/* perft self-test: ./chess --perft                                    */
/* ------------------------------------------------------------------ */
static int run_perft_tests()
{
    struct Case { const char* fen; int depth; int expected; };
    static const Case cases[] =
    {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902},
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 1, 48},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 2, 2039},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3, 97862},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 1, 14},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 2, 191},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 3, 2812},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 4, 43238},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 1, 6},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 2, 264},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 3, 9467},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 1, 44},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 2, 1486},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3, 62379},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 1, 46},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 2, 2079},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3, 89890}
    };
    const int n = (int)(sizeof(cases)/sizeof(cases[0]));
    int failures = 0;

    for(int i = 0; i < n; i++)
    {
        Position p;
        if(!core::fen_load(cases[i].fen, p))
        {
            std::cout<<"FEN PARSE FAIL: "<<cases[i].fen<<"\n";
            failures++;
            continue;
        }
        const int got = core::perft(p, cases[i].depth);
        const bool ok = (got == cases[i].expected);
        if(!ok)
            failures++;
        std::cout<<(ok?"PASS":"FAIL")
                 <<" depth "<<cases[i].depth
                 <<": got "<<got
                 <<" expected "<<cases[i].expected<<"\n";
    }
    std::cout<<(n - failures)<<"/"<<n<<" passed\n";
    return failures ? 1 : 0;
}

int main(int argc, char** argv)
{
    if(argc > 1 && std::strcmp(argv[1], "--perft") == 0)
    {
        return run_perft_tests();
    }

    if(!t1.init())
    {
        SDL_Log("Error init technical\nCode:%d\nError:%s\n", t1.err_code_get(), SDL_GetError());
    }
    else if(!t1.media())
    {
        SDL_Log("Error media technical\nCode:%d\nError:%s\n", t1.err_code_get(), SDL_GetError());
    }
    else
    {
        bool exit = false;
        SDL_Event e;
        g1.game_init();
        while(!exit)
        {
            m1.clicked = false;
            while(SDL_PollEvent(&e))
            {
                if(e.type==SDL_EVENT_QUIT)
                    exit = true;

                m1.mouse_handler();
                switch(g1.state)
                {
                case PLAYING:
                    {
                        switch(e.type)
                        {
                        case SDL_EVENT_MOUSE_BUTTON_DOWN:
                            {
                                m1.clicked = true;
                                g1.input_press();
                                break;
                            }
                        case SDL_EVENT_MOUSE_MOTION:
                            {
                                g1.input_motion();
                                break;
                            }
                        case SDL_EVENT_MOUSE_BUTTON_UP:
                            {
                                g1.input_release();
                                break;
                            }
                        }
                        break;
                    }
                case MAIN_MENU:
                    {
                        switch(e.type)
                        {
                        case SDL_EVENT_MOUSE_BUTTON_DOWN:
                            {
                                m1.clicked = true;
                                if(pvp_text.mouse_in(m1.position_get().x, m1.position_get().y))
                                {
                                    g1.state = PLAYING;
                                    g1.mode = PVP;
                                }
                                else if(cpu_text.mouse_in(m1.position_get().x, m1.position_get().y))
                                {
                                    g1.state = PLAYING;
                                    g1.mode = VS_CPU;
                                }

                                break;
                            }

                        }
                        break;
                    }
                default:
                    break;
                }

            }

            switch(g1.state)
            {
            case PLAYING:
                {
                    /* CPU replies in VS_CPU mode */
                    if( g1.mode == VS_CPU && g1.awaiting_cpu && !g1.promo_pending )
                    {
                        c1.engine_start();
                        c1.full_eval();
                        c1.play_best_move();
                    }

                    SDL_SetRenderDrawColor(t1.renderer_get(), 255, 255, 255, 255);
                    SDL_RenderClear(t1.renderer_get());

                    g1.board_render();
                    g1.pieces_render();
                    c1.eval_render();

                    SDL_RenderPresent(t1.renderer_get());
                    break;
                }
            case BLACK_WON: case WHITE_WON: case DRAW:

                SDL_SetRenderDrawColor(t1.renderer_get(), 255, 255, 255, 255);
                SDL_RenderClear(t1.renderer_get());

                g1.game_end_render();

                SDL_RenderPresent(t1.renderer_get());
                break;
            case MAIN_MENU:
                {
                    SDL_SetRenderDrawColor(t1.renderer_get(), 255, 255, 255, 255);
                    SDL_RenderClear(t1.renderer_get());

                    g1.board_render();
                    g1.pieces_render();
                    g1.main_menu_render();


                    SDL_RenderPresent(t1.renderer_get());
                    break;
                }
            }

        }
        g1.game_close();

    }
    t1.close();
    return 0;
}
