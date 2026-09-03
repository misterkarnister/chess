#include "Bishop.h"
#include "object_init.h"
Bishop::Bishop() = default;
Bishop::~Bishop() = default;

void Bishop::piece_init(Color c)
{
    Piece::piece_init(c);
    name.text_load("B", t1.font_piece_get(), color_sdl);
    the_texture = (c == WHITE) ? &bishop_white_obj : &bishop_black_obj;
}
