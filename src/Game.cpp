#include "Game.h"
#include "object_init.h"
#include <iostream>
#include <string>
Game::Game()
{
    square_dim = 64;
    players_color = WHITE;
    white_color = {225, 200, 220, 220};
    black_color = {158, 106, 100, 220};

    for(int i =0;i<64;i++)
    {
        board[i] = nullptr;

    }

    checked = false;

    checked_color = WHITE;

    mode = PVP;
    state = PLAYING;

    white_valid_squares = 0;
    black_valid_squares = 0;

    turn = WHITE;

}

Game::~Game()
{
    game_close();
}
void Game::square_render(int x, int y, SDL_Color col)
{
    SDL_SetRenderDrawColor(t1.renderer_get(), col.r, col.g, col.b, col.a);
    SDL_FRect to_render=
    {
        .x = static_cast<float>(t1.window_width_get() * 0.1 + x * square_dim),
        .y = static_cast<float>(t1.window_height_get() * 0.1 + y * square_dim),
        .w = static_cast<float>(square_dim),
        .h = static_cast<float>(square_dim)
    };

    SDL_RenderRect(t1.renderer_get(), &to_render);
    SDL_RenderFillRect(t1.renderer_get(), &to_render);
}
void Game::board_render()
{
    switch(players_color)
    {
    case WHITE:
        {
            for(int i =0;i<8;i++)
            {

                for(int j=0;j<8;j++)
                {

                    SDL_Color c;
                    if(j%2==0)
                    {
                        if(i%2==0)
                            c = white_color;
                        else
                            c = black_color;
                    }
                    else
                    {
                        if(i%2==0)
                            c = black_color;
                        else
                            c = white_color;
                    }

                    square_render(i, j, c);
                }
            }
            for(int i=0;i<8;i++)
            {
                letter_render(i, WHITE);
                digit_render(i, WHITE);
                letter_render(i, BLACK);
                digit_render(i, BLACK);

            }
            break;
        }
    case BLACK:
        {
            for(int i =0;i<8;i++)
            {
                for(int j=0;j<8;j++)
                {
                    SDL_Color c;
                    if(j%2!=0)
                    {
                        if(i%2==0)
                            c = white_color;
                        else
                            c = black_color;
                    }
                    else
                    {
                        if(i%2==0)
                            c = black_color;
                        else
                            c = white_color;
                    }

                    square_render(i, j, c);
                }
            }
            for(int i=7;i>=0;i--)
            {
                letter_render(i, WHITE);
                digit_render(i, WHITE);
                letter_render(i, BLACK);
                digit_render(i, BLACK);


            }
            break;
        }
    }

}
void Game::text_generate()
{
    char* lets[] =
    {
        "A", "B", "C", "D", "E", "F", "G", "H"
    };
    for(int i =0;i<8;i++)
    {
        letters_text[i].text_load(lets[i], t1.font_board_get(), t1.font_board_color_get());
        digits_text[i].text_load(std::to_string(i+1).c_str(), t1.font_board_get(), t1.font_board_color_get());
    }
    white_won_text.text_load("White won!", t1.font_ui_get(), t1.font_board_color_get());
    black_won_text.text_load("Black won!", t1.font_ui_get(), t1.font_board_color_get());
    draw_text.text_load("Draw!", t1.font_ui_get(), t1.font_board_color_get());
}

void Game::game_init()
{
    text_generate();


    for(int i=0;i<8;i++)
    {
        pawns_white[i].piece_init(WHITE);
        pawns_black[i].piece_init(BLACK);

    }

    for(int i=0;i<2;i++)
    {
        rooks_white[i].piece_init(WHITE);
        rooks_black[i].piece_init(BLACK);
        knights_white[i].piece_init(WHITE);
        knights_black[i].piece_init(BLACK);
        bishops_white[i].piece_init(WHITE);
        bishops_black[i].piece_init(BLACK);
    }

    queen_white.piece_init(WHITE);
    queen_black.piece_init(BLACK);

    king_white.piece_init(WHITE);
    king_black.piece_init(BLACK);

    valid_squares_find_all();

}
void Game::game_close()
{
    for(int i = 0; i<8;i++)
    {
        letters_text[i].destroy();
        digits_text[i].destroy();
        pawns_white[i].cleanup();
        pawns_black[i].cleanup();

    }
    for(int i=0;i<2;i++)
    {
        rooks_white[i].cleanup();
        rooks_black[i].cleanup();
        knights_white[i].cleanup();
        knights_black[i].cleanup();
        bishops_white[i].cleanup();
        bishops_black[i].cleanup();
    }

    queen_white.cleanup();
    queen_black.cleanup();
    king_white.cleanup();
    king_black.cleanup();

    white_won_text.destroy();
    black_won_text.destroy();
    draw_text.destroy();
}

void Game::letter_render(int column, Color side)
{
    float y;
    switch(side)
    {
    case WHITE:
        {
            y = t1.window_height_get() * 0.09 - letters_text[column].height_get() * 0.5;
            break;
        }
    case BLACK:
        {
            y = t1.window_height_get() * 0.09 + 8 * square_dim + letters_text[column].height_get() * 0.5;
            break;
        }
    }
    switch(players_color)
    {
    case WHITE:
        {
            letters_text[column].render(t1.window_width_get() * 0.1 + (column+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[column].width_get(), y);
            break;
        }
    case BLACK:
        {
            letters_text[7 - column].render(t1.window_width_get() * 0.1 + (column+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[column].width_get(), y);
            break;
        }
    }

}
void Game::digit_render(int row, Color side)
{
    float x;
    switch(side)
    {
    case WHITE:
        {
            x = t1.window_width_get() * 0.09 - letters_text[row].width_get() * 0.5;
            break;
        }
    case BLACK:
        {
            x = t1.window_width_get() * 0.1 + 8 * square_dim + letters_text[row].width_get();
            break;
        }
    }
    switch(players_color)
    {
    case WHITE:
        {
            digits_text[7-row].render(x, t1.window_height_get() * 0.1 + (row+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[row].height_get());
            break;
        }
    case BLACK:
        {
            digits_text[row].render(x, t1.window_height_get() * 0.1 + (row+1) * square_dim - 0.5 * square_dim - 0.5 * letters_text[row].height_get());
            break;
        }
    }

}
bool Game::square_free(Vec2i pos)
{
    if(board[pos.y*8 + pos.x]==nullptr)
        return true;
    return false;
}
void Game::pieces_render()
{
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            board[i]->valid_squares_render();
        }
    }
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            board[i]->render();
            board[i]->hover();
        }
    }
    for(int i =0;i<8;i++)
    {
        if(pawns_black[i].promoting)
        {
            pawns_black[i].promote_render();
            for(int j=0;j<4;j++)
            {
                promo_display[j].hover();
            }
        }
        else if(pawns_white[i].promoting)
        {
            pawns_white[i].promote_render();
            for(int j=0;j<4;j++)
            {
                promo_display[j].hover();
            }
        }

    }
}
void Game::valid_squares_find_all()
{
    white_valid_squares = 0;
    black_valid_squares = 0;
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            board[i]->valid_squares_find();

        }

    }
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            board[i]->valid_squares_remove_checked();
        }

    }
    white_valid_squares = 0;
    black_valid_squares = 0;
    for(int i=0;i<64;i++)
    {
        if(board[i]!=nullptr)
        {
            if(board[i]->color==WHITE)
                white_valid_squares += board[i]->valid_squares.size();
            else
                black_valid_squares += board[i]->valid_squares.size();
        }

    }
}
void Game::game_judge()
{
    if(white_valid_squares == 0)
    {
        if(checked && checked_color == WHITE)
            state = BLACK_WON;
        else
            state = DRAW;
    }
    else if(black_valid_squares == 0)
    {
        if(checked && checked_color == BLACK)
            state = WHITE_WON;
        else
            state = DRAW;
    }
}
void Game::game_end_render()
{

    switch(state)
    {
    case BLACK_WON:
        {
            black_won_text.render(
                                    t1.window_width_get() * 0.5 - 0.5 * black_won_text.width_get(),
                                    t1.window_height_get() * 0.5 - 0.5 * black_won_text.height_get()
                                  );
            break;
        }
    case WHITE_WON:
        {
            white_won_text.render(
                                    t1.window_width_get() * 0.5 - 0.5 * white_won_text.width_get(),
                                    t1.window_height_get() * 0.5 - 0.5 * white_won_text.height_get()
                                  );
            break;
        }
    case DRAW:
        {
            draw_text.render(
                                    t1.window_width_get() * 0.5 - 0.5 * draw_text.width_get(),
                                    t1.window_height_get() * 0.5 - 0.5 * draw_text.height_get()
                                  );
            break;
        }
    }
}

