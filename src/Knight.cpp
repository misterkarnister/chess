#include "Knight.h"
#include "object_init.h"
Knight::Knight()
{
    //ctor
}

Knight::~Knight()
{
    //dtor
}

void Knight::piece_init(Color c)
{

    color = c;
    static int white_ind = 0;
    static int black_ind = 0;
    switch(c)
    {
    case WHITE:
        {
            board_pos.x = white_ind==0? 1 : 6;
            board_pos.y =  g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&knights_white[white_ind];

            white_ind++;
            color_sdl= piece_white;
            break;
        }
    case BLACK:
        {
            board_pos.x = black_ind==0? 1:6;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&knights_black[black_ind];
            black_ind++;
            color_sdl = piece_black;
            break;
        }
    }
    name.text_load("N", t1.font_piece_get(), color_sdl);
    Vec2f newpos = {static_cast<float>(t1.window_width_get() * 0.1 + (board_pos.x+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.width_get()),static_cast<float>(t1.window_height_get() * 0.1 + (board_pos.y+1) * g1.square_dim - 0.5 * g1.square_dim - 0.5 * name.height_get())};
    name.position_update(newpos);
}
void Knight::valid_squares_find()
{
    valid_squares_empty();

    Vec2i mods[8] =
    {
        {
            -1, 2
        },
        {
            1, 2
        },
        {
            2, 1
        },
        {
            2,-1
        },
        {
            1, -2
        },
        {
            -1, -2
        },
        {
            -2, -1
        },
        {
            -2, 1
        }

    };

    for(Vec2i mod : mods)
    {
        Vec2i chk = board_pos.add(mod);
        if(chk.x>=0 && chk.x<8 && chk.y>=0 && chk.y<8)
        {
            if(g1.square_free(chk) || g1.board[chk.y*8 + chk.x]->color!=color)
            {
                valid_squares.push_back(chk);
            }

        }
    }

}
