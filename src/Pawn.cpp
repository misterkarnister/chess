#include "Pawn.h"
#include "object_init.h"
Pawn::Pawn()
{
    start_pos = true;
}
Pawn::~Pawn() = default;

void Pawn::piece_init(Color c)
{
    Piece::piece_init(c);
    name.text_load("p", t1.font_piece_get(), color_sdl);
    the_texture = (c == WHITE) ? &pawn_white_obj : &pawn_black_obj;
}
