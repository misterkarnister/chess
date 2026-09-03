#include "Knight.h"
#include "object_init.h"
Knight::Knight() = default;
Knight::~Knight() = default;

void Knight::piece_init(Color c)
{
    Piece::piece_init(c);
    name.text_load("N", t1.font_piece_get(), color_sdl);
    the_texture = (c == WHITE) ? &knight_white_obj : &knight_black_obj;
}
