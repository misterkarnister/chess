#include "Rook.h"
#include "object_init.h"
Rook::Rook()
{
    start_pos = true;
}
Rook::~Rook() = default;

void Rook::piece_init(Color c)
{
    Piece::piece_init(c);
    name.text_load("R", t1.font_piece_get(), color_sdl);
    the_texture = (c == WHITE) ? &rook_white_obj : &rook_black_obj;
}
