#include "Knight.h"
#include "object_init.h"
Knight::Knight() = default;
Knight::~Knight() = default;

void Knight::piece_init(Color c)
{

    color = c;
    switch(c)
    {
    case WHITE:
        {
            board_pos.x = g1.knight_white_idx==0? 1 : 6;
            board_pos.y =  g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&knights_white[g1.knight_white_idx];

            g1.knight_white_idx++;
            color_sdl= piece_white;
            the_texture = &knight_white_obj;
            break;
        }
    case BLACK:
        {
            board_pos.x = g1.knight_black_idx==0? 1:6;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&knights_black[g1.knight_black_idx];
            g1.knight_black_idx++;
            color_sdl = piece_black;
            the_texture = &knight_black_obj;
            break;
        }
    }
    name.text_load("N", t1.font_piece_get(), color_sdl);
    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
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
