#ifndef KNIGHT_H
#define KNIGHT_H

#include "Piece.h"

class Knight : public Piece
{
    public:
        Knight();
        virtual ~Knight();
        Knight(const Knight& other) = delete;
        Knight& operator=(const Knight& other) = delete;

        void piece_init(Color c) override;
    protected:

    private:
};

#endif // KNIGHT_H
