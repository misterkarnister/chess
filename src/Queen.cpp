#include "Queen.h"
#include "object_init.h"
Queen::Queen() = default;
Queen::~Queen() = default;

void Queen::piece_init(Color c)
{
    Piece::piece_init(c);
    name.text_load("Q", t1.font_piece_get(), color_sdl);
    the_texture = (c == WHITE) ? &queen_white_obj : &queen_black_obj;
}
