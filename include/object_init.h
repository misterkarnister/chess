/** \author Igor Solny
 *
 * \file object_init.h
 *
 * \brief Deklaracje extern obiektów globalnych; definicje w main.cpp
 */


#ifndef OBJECT_INIT_H_INCLUDED
#define OBJECT_INIT_H_INCLUDED

#include "Object.h"
#include "Game.h"
#include "Technical.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include "Mouse.h"
#include "CPU.h"

extern King         king_white,         king_black;
extern Queen        queen_white,        queen_black;
extern Bishop       bishops_white[2],   bishops_black[2];
extern Knight       knights_white[2],   knights_black[2];
extern Rook         rooks_white[2],     rooks_black[2];
extern Pawn         pawns_white[8],     pawns_black[8];
extern Queen        promo_queen_white[8],   promo_queen_black[8];
extern Rook         promo_rook_white[8],    promo_rook_black[8];
extern Bishop       promo_bishop_white[8],  promo_bishop_black[8];
extern Knight       promo_knight_white[8],  promo_knight_black[8];
extern Piece        promo_display[4];
extern CPU          c1;
extern Text         letters_text[8],    digits_text[8],     black_won_text,      white_won_text,      draw_text,    pvp_text,   cpu_text;
extern Object       pawn_white_obj, rook_white_obj, knight_white_obj, bishop_white_obj, queen_white_obj, king_white_obj,
                    pawn_black_obj, rook_black_obj, knight_black_obj, bishop_black_obj, queen_black_obj, king_black_obj;
extern Mouse        m1;
extern Game         g1;
extern Technical    t1;

#endif // OBJECT_INIT_H_INCLUDED
