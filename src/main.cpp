
#include "../include/object_init.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

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

int main(int argc, char** argv)
{
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
                        switch(g1.mode)
                        {
                        case PVP:
                            {
                                switch(e.type)
                {
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    {
                        m1.clicked = true;
                        for(int i=0;i<64;i++)
                        {
                            if(g1.board[i]!=nullptr && g1.board[i]->mouse_in() && g1.turn ==g1.board[i]->color)
                            {
                                g1.board[i]->select();
                            }
                        }

                        break;
                    }
                case SDL_EVENT_MOUSE_MOTION:
                    {

                        for(int i=0;i<64;i++)
                        {
                            if(g1.board[i]!=nullptr && g1.turn ==g1.board[i]->color)
                            {
                                g1.board[i]->drag();
                            }
                        }

                        break;
                    }
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    {
                        for(int i=0;i<64;i++)
                        {
                            if(g1.board[i]!=nullptr && g1.turn ==g1.board[i]->color)
                            {
                                g1.board[i]->snap();
                            }
                        }
                        break;

                    }
                }
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

            }
            switch(g1.state)
            {
            case PLAYING:
                {
                    g1.checked = false;
                    for(int i =0;i<64;i++)
            {
                if(g1.board[i]!=nullptr)
                {
                    if(g1.board[i]->checking())
                    {
                        g1.checked = true;
                        g1.checked_color = g1.board[i]->color==WHITE?BLACK:WHITE;
                        break;
                    }
                }

            }

            for(int i =0;i<4;i++)
                        {
                            promo_display[i].promote();
                        }

            g1.game_judge();

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
        }
        g1.game_close();

    }
    t1.close();
    return 0;
}

