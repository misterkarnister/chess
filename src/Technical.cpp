#include "../include/Technical.h"
#include "../include/object_init.h"


#include <iostream>

Technical::Technical()
{
    window = nullptr;
    window_width = 0;
    window_height = 0;

    renderer = nullptr;

    err_code = NO_ERROR;

    fps_target = 60;

    font_board = nullptr;
    font_piece = nullptr;
    font_board_color = {255, 255, 255, 255};
}

Technical::~Technical()
{
}

bool init(Technical& t)
{
    bool success = true;

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed! \n%s", SDL_GetError());
        t.err_code = ERR_SDL_INIT;
        success = false;
    }
    else
    {
        t.window = SDL_CreateWindow("SC2D", 1280, 960, 0);
        if(t.window == nullptr)
        {
            SDL_Log("SDL_CreateWindow('SC2D', 800, 600, 0) failed!\n%s", SDL_GetError());
            t.err_code = ERR_SDL_CREATE_WINDOW;
            success = false;
        }
        else
        {
            if(!SDL_GetWindowSize(t.window, &t.window_width, &t.window_height))
            {
                SDL_Log("SDL_GetWindowSize(t.window, &t.window_width, &t.window_height) failed!\n%s", SDL_GetError());
                t.err_code = ERR_SDL_GET_WINDOW_SIZE;
                success = false;
            }
            else
            {
                t.renderer = SDL_CreateRenderer(t.window, nullptr);
                if(t.renderer==nullptr)
                {
                    SDL_Log("SDL_CreateRenderer(t.window, nullptr)\n%s", SDL_GetError());
                    t.err_code = ERR_SDL_CREATE_RENDERER;
                    success = false;
                }
                else
                {
                    if(!TTF_Init())
                    {
                        SDL_Log("TTF_Init()\n%s", SDL_GetError());
                        t.err_code = ERR_TTF_INIT;
                        success = false;
                    }
                }

            }
        }
    }

    return success;
}

bool media(Technical& t)
{
    bool success = true;

    t.font_board = TTF_OpenFont("fonts/ProggyVector Regular.ttf", 14);
    if(t.font_board==nullptr)
    {
        SDL_Log("Couldnt load font_board: %s", SDL_GetError());
        success = false;
    }

    t.font_piece = TTF_OpenFont("fonts/ProggyVector Regular.ttf", 20);
    if(t.font_piece==nullptr)
    {
        SDL_Log("Couldnt load font_piece: %s", SDL_GetError());
        success = false;
    }

    t.font_ui = TTF_OpenFont("fonts/ProggyVector Regular.ttf", 40);
    if(t.font_ui==nullptr)
    {
        SDL_Log("Couldnt load font_ui: %s", SDL_GetError());
        success = false;
    }

    t.font_board_color = {0, 0, 0, 255};
    t.font_ui_color = {0, 0, 0, 255};
    return success;
}

void close(Technical& t)
{
    if(t.renderer!=nullptr)
    {
        SDL_DestroyRenderer(t.renderer);
        t.renderer = nullptr;
    }

    if(t.window!=nullptr)
    {
        SDL_DestroyWindow(t.window);
        t.window = nullptr;
    }

    if(t.font_board!=nullptr)
    {
        TTF_CloseFont(t.font_board);
        t.font_board=nullptr;
    }
    if(t.font_piece!=nullptr)
    {
        TTF_CloseFont(t.font_piece);
        t.font_piece=nullptr;
    }
    if(t.font_ui!=nullptr)
    {
        TTF_CloseFont(t.font_ui);
        t.font_piece=nullptr;
    }

    static bool quit_called = false;
    if(!quit_called)
    {
        TTF_Quit();
        SDL_Quit();
        quit_called = true;
    }
}

SDL_Window* Technical::window_get()
{
    return window;
}
int Technical::window_height_get()
{
    return window_height;
}
int Technical::window_width_get()
{
    return window_width;
}
SDL_Renderer* Technical::renderer_get()
{
    return renderer;
}
Errors_technical Technical::err_code_get()
{
    return err_code;
}
int Technical::fps_target_get()
{
    return fps_target;
}
TTF_Font* Technical::font_board_get()
{
    return font_board;
}
SDL_Color Technical::font_board_color_get()
{
    return font_board_color;
}
TTF_Font* Technical::font_piece_get()
{
    return font_piece;
}
TTF_Font* Technical::font_ui_get()
{
    return font_ui;
}
SDL_Color Technical::font_ui_color_get()
{
    return font_ui_color;
}
