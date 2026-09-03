#ifndef QUEEN_H
#define QUEEN_H

#include "Piece.h"

class Queen : public Piece
{
    public:
        Queen();
        virtual ~Queen();
        Queen(const Queen& other) = delete;
        Queen& operator=(const Queen& other) = delete;
        void piece_init(Color c) override;

    protected:

    private:
};

#endif // QUEEN_H
