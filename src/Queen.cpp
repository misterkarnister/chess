#include "Queen.h"
#include "object_init.h"
Queen::Queen() = default;
Queen::~Queen() = default;

void Queen::piece_init(Color c)
{

    color = c;

    switch(c)
    {
    case WHITE:
        {
            board_pos.x = g1.players_color==WHITE ? 3 : 4;
            board_pos.y =  g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&queen_white;


            color_sdl= piece_white;
            the_texture = &queen_white_obj;
            break;
        }
    case BLACK:
        {
            board_pos.x = g1.players_color==WHITE ? 3 : 4;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&queen_black;

            color_sdl = piece_black;
            the_texture = &queen_black_obj;
            break;
        }
    }
    name.text_load("Q", t1.font_piece_get(), color_sdl);
    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);
}
void Queen::valid_squares_find()
{
    valid_squares_empty();
    scan_direction({1, 0});
    scan_direction({-1, 0});
    scan_direction({0, 1});
    scan_direction({0, -1});
    scan_direction({1, 1});
    scan_direction({-1, -1});
    scan_direction({-1, 1});
    scan_direction({1, -1});
}
