#include "Technical.h"
#include "object_init.h"

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

Technical::~Technical() = default;

bool Technical::init()
{
    bool success = true;

    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed! \n%s", SDL_GetError());
        err_code = ERR_SDL_INIT;
        success = false;
    }
    else
    {
        window = SDL_CreateWindow("SC2D", 1280, 960, 0);
        if(window == nullptr)
        {
            SDL_Log("SDL_CreateWindow('SC2D', 800, 600, 0) failed!\n%s", SDL_GetError());
            err_code = ERR_SDL_CREATE_WINDOW;
            success = false;
        }
        else
        {
            if(!SDL_GetWindowSize(window, &window_width, &window_height))
            {
                SDL_Log("SDL_GetWindowSize(window, &window_width, &window_height) failed!\n%s", SDL_GetError());
                err_code = ERR_SDL_GET_WINDOW_SIZE;
                success = false;
            }
            else
            {
                renderer = SDL_CreateRenderer(window, nullptr);
                if(renderer==nullptr)
                {
                    SDL_Log("SDL_CreateRenderer(window, nullptr)\n%s", SDL_GetError());
                    err_code = ERR_SDL_CREATE_RENDERER;
                    success = false;
                }
                else
                {
                    if(!TTF_Init())
                    {
                        SDL_Log("TTF_Init()\n%s", SDL_GetError());
                        err_code = ERR_TTF_INIT;
                        success = false;
                    }
                }

            }
        }
    }

    return success;
}

bool Technical::media()
{
    bool success = true;

    font_board = TTF_OpenFont("fonts/ProggyVector Regular.ttf", 14);
    if(font_board==nullptr)
    {
        SDL_Log("Couldnt load font_board: %s", SDL_GetError());
        success = false;
    }

    font_piece = TTF_OpenFont("fonts/ProggyVector Regular.ttf", 20);
    if(font_piece==nullptr)
    {
        SDL_Log("Couldnt load font_piece: %s", SDL_GetError());
        success = false;
    }

    font_ui = TTF_OpenFont("fonts/ProggyVector Regular.ttf", 40);
    if(font_ui==nullptr)
    {
        SDL_Log("Couldnt load font_ui: %s", SDL_GetError());
        success = false;
    }

    font_board_color = {0, 0, 0, 255};
    font_ui_color = {0, 0, 0, 255};

    if(!pawn_white_obj.texture_load("tex/pawn-w.svg"))
    {
        SDL_Log("Couldnt load pawn-w.svg: %s", SDL_GetError());
        success = false;
    }
    if(!rook_white_obj.texture_load("tex/rook-w.svg"))
    {
        SDL_Log("Couldnt load rook-w.svg: %s", SDL_GetError());
        success = false;
    }
    if(!knight_white_obj.texture_load("tex/knight-w.svg"))
    {
        SDL_Log("Couldnt load knight-w.svg: %s", SDL_GetError());
        success = false;
    }
    if(!bishop_white_obj.texture_load("tex/bishop-w.svg"))
    {
        SDL_Log("Couldnt load bishop-w.svg: %s", SDL_GetError());
        success = false;
    }
    if(!queen_white_obj.texture_load("tex/queen-w.svg"))
    {
        SDL_Log("Couldnt load queen-w.svg: %s", SDL_GetError());
        success = false;
    }
    if(!king_white_obj.texture_load("tex/king-w.svg"))
    {
        SDL_Log("Couldnt load king-w.svg: %s", SDL_GetError());
        success = false;
    }

    if(!pawn_black_obj.texture_load("tex/pawn-b.svg"))
    {
        SDL_Log("Couldnt load pawn-b.svg: %s", SDL_GetError());
        success = false;
    }
    if(!rook_black_obj.texture_load("tex/rook-b.svg"))
    {
        SDL_Log("Couldnt load rook-b.svg: %s", SDL_GetError());
        success = false;
    }
    if(!knight_black_obj.texture_load("tex/knight-b.svg"))
    {
        SDL_Log("Couldnt load knight-b.svg: %s", SDL_GetError());
        success = false;
    }
    if(!bishop_black_obj.texture_load("tex/bishop-b.svg"))
    {
        SDL_Log("Couldnt load bishop-b.svg: %s", SDL_GetError());
        success = false;
    }
    if(!queen_black_obj.texture_load("tex/queen-b.svg"))
    {
        SDL_Log("Couldnt load queen-b.svg: %s", SDL_GetError());
        success = false;
    }
    if(!king_black_obj.texture_load("tex/king-b.svg"))
    {
        SDL_Log("Couldnt load king-b.svg: %s", SDL_GetError());
        success = false;
    }

    return success;
}

void Technical::close()
{
    if(renderer!=nullptr)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if(window!=nullptr)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    if(font_board!=nullptr)
    {
        TTF_CloseFont(font_board);
        font_board=nullptr;
    }
    if(font_piece!=nullptr)
    {
        TTF_CloseFont(font_piece);
        font_piece=nullptr;
    }
    if(font_ui!=nullptr)
    {
        TTF_CloseFont(font_ui);
        font_ui=nullptr;
    }

    static bool quit_called = false;
    if(!quit_called)
    {
        TTF_Quit();
        SDL_Quit();
        quit_called = true;
    }
}

SDL_Window* Technical::window_get() const
{
    return window;
}
int Technical::window_height_get() const
{
    return window_height;
}
int Technical::window_width_get() const
{
    return window_width;
}
SDL_Renderer* Technical::renderer_get() const
{
    return renderer;
}
Errors_technical Technical::err_code_get() const
{
    return err_code;
}
int Technical::fps_target_get() const
{
    return fps_target;
}
TTF_Font* Technical::font_board_get() const
{
    return font_board;
}
SDL_Color Technical::font_board_color_get() const
{
    return font_board_color;
}
TTF_Font* Technical::font_piece_get() const
{
    return font_piece;
}
TTF_Font* Technical::font_ui_get() const
{
    return font_ui;
}
SDL_Color Technical::font_ui_color_get() const
{
    return font_ui_color;
}
