#include "Bishop.h"
#include "object_init.h"
Bishop::Bishop() = default;
Bishop::~Bishop() = default;

void Bishop::piece_init(Color c)
{

    color = c;
    switch(c)
    {
    case WHITE:
        {
            board_pos.x = g1.bishop_white_idx==0? 2 : 5;
            board_pos.y = g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&bishops_white[g1.bishop_white_idx];

            g1.bishop_white_idx++;
            color_sdl= piece_white;
            the_texture = &bishop_white_obj;
            break;
        }
    case BLACK:
        {
            board_pos.x = g1.bishop_black_idx==0? 2 : 5;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&bishops_black[g1.bishop_black_idx];
            g1.bishop_black_idx++;
            color_sdl = piece_black;
            the_texture = &bishop_black_obj;
            break;
        }
    }
    name.text_load("B", t1.font_piece_get(), color_sdl);
    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);
}
void Bishop::valid_squares_find()
{
    valid_squares_empty();
    scan_direction({1, 1});
    scan_direction({-1, -1});
    scan_direction({-1, 1});
    scan_direction({1, -1});
}
