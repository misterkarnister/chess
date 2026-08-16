/** \author Igor Solny
 *
 * \file object_init.h
 *
 * \brief Inicjujemy tu obiekty klas z plików: inline Nazwa_klasy nazwa_obiektu;
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

inline King         king_white,         king_black;
inline Queen        queen_white,        queen_black;
inline Bishop       bishops_white[2],   bishops_black[2];
inline Knight       knights_white[2],   knights_black[2];
inline Rook         rooks_white[2],     rooks_black[2];
inline Pawn         pawns_white[8],     pawns_black[8];
inline Queen        promo_queen_white[8],   promo_queen_black[8];
inline Rook         promo_rook_white[8],    promo_rook_black[8];
inline Bishop       promo_bishop_white[8],  promo_bishop_black[8];
inline Knight       promo_knight_white[8],  promo_knight_black[8];
inline Piece        promo_display[4];
inline Text         letters_text[8],    digits_text[8],     black_won_text,      white_won_text,      draw_text;
inline Mouse        m1;
inline Game         g1;
inline Technical    t1;

#endif // OBJECT_INIT_H_INCLUDED
