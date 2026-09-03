#include "King.h"
#include "object_init.h"
King::King()
{
    start_pos = true;
}
King::~King() = default;

void King::piece_init(Color c)
{
    Piece::piece_init(c);
    name.text_load("K", t1.font_piece_get(), color_sdl);
    the_texture = (c == WHITE) ? &king_white_obj : &king_black_obj;
}
