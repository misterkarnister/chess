#ifndef PAWN_H
#define PAWN_H

#include "Piece.h"


class Pawn : public Piece
{
    public:
        Pawn();
        virtual ~Pawn();
        Pawn(const Pawn& other) = delete;
        Pawn& operator=(const Pawn& other) = delete;
        void piece_init(Color c) override;
        void valid_squares_find() override;
        void move(Vec2i chk) override;
        void promote_render();
        bool last_double_move;
        bool promoting;
        bool inited;

    protected:

    private:

};

#endif // PAWN_H
