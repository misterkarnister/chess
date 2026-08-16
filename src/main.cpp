
#include "../include/object_init.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>

int main(int argc, char** argv)
{
    if(!init(t1))
    {
        SDL_Log("Error init technical\nCode:%d\nError:%s\n", t1.err_code_get(), SDL_GetError());
    }
    else if(!media(t1))
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


            SDL_RenderPresent(t1.renderer_get());
                    break;
                }
            case BLACK_WON: case WHITE_WON: case DRAW:

                SDL_SetRenderDrawColor(t1.renderer_get(), 255, 255, 255, 255);
            SDL_RenderClear(t1.renderer_get());

            g1.game_end_render();

            SDL_RenderPresent(t1.renderer_get());
                break;
            }




        }

        g1.game_close();

    }
    close(t1);
    return 0;
}
