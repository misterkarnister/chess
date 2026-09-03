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

    protected:

    private:

};

#endif // PAWN_H
