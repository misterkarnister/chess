#include "Rook.h"
#include "object_init.h"
Rook::Rook()
{
    start_pos = true;
}

Rook::~Rook() = default;
void Rook::piece_init(Color c)
{

    color = c;
    switch(c)
    {
    case WHITE:
        {
            board_pos.x = g1.rook_white_idx==0? 0 : 7;
            board_pos.y =  g1.players_color==WHITE ? 7 : 0;
            g1.board[g1.players_color==WHITE? 56 + board_pos.x : 0 + board_pos.x]=&rooks_white[g1.rook_white_idx];

            g1.rook_white_idx++;
            color_sdl= piece_white;
            the_texture = &rook_white_obj;
            break;
        }
    case BLACK:
        {
            board_pos.x = g1.rook_black_idx==0? 0:7;
            board_pos.y =  g1.players_color==WHITE ? 0 : 7;
            g1.board[g1.players_color==WHITE? 0 + board_pos.x : 56 + board_pos.x]=&rooks_black[g1.rook_black_idx];
            g1.rook_black_idx++;
            color_sdl = piece_black;
            the_texture = &rook_black_obj;
            break;
        }
    }
    name.text_load("R", t1.font_piece_get(), color_sdl);
    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);
}
void Rook::valid_squares_find()
{
    valid_squares_empty();
    scan_direction({1, 0});
    scan_direction({-1, 0});
    scan_direction({0, 1});
    scan_direction({0, -1});
}
void Rook::move(Vec2i pos)
{


    g1.board[pos.y*8 + pos.x] = g1.board[board_pos.y*8 + board_pos.x];
    g1.board[board_pos.y*8 + board_pos.x] = nullptr;
    position_update(pos);

    Vec2f newpos = g1.board_to_screen(board_pos, name.width_get(), name.height_get());
    name.position_update(newpos);

    start_pos = false;

    g1.black_valid_squares=0;
    g1.white_valid_squares=0;

    g1.toggle_turn();

    c1.engine_start();
    c1.full_eval();

}
